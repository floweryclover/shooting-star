#include "InventoryComponent.h"
#include "WeaponData.h"
#include "WeaponModifier.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Net/UnrealNetwork.h"
#include "ShootingStar/ShootingStar.h"

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	SetIsReplicated(true);
	// Init ResourceInventory
	ResourceInventory.Empty();
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	TArray<FAssetData> AssetList;
	FName AssetPath = TEXT("/Game/Data/Resources");

	AssetRegistryModule.Get().GetAssetsByPath(AssetPath, AssetList, true);

	for (const FAssetData& Asset : AssetList)
	{
		UResourceDataAsset* Resource = Cast<UResourceDataAsset>(Asset.GetAsset());
		if (Resource)
		{
			ResourceInventory.Add({ Resource, 0 });
		}
	}

	// Ascending Sort By ResourceType Enum
	ResourceInventory.Sort([](const FResourceInventoryData& A, const FResourceInventoryData& B) {
		return static_cast<int>(A.Resource->ResourceType) < static_cast<int>(B.Resource->ResourceType);
		});
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, ResourceInventory);
	DOREPLIFETIME(UInventoryComponent, ModifierDataTable);
}

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UDataTable> ModifierTableFinder(
		TEXT("/Game/Data/WeaponModifiers.WeaponModifiers"));

	if (ModifierTableFinder.Succeeded())
		ModifierDataTable = ModifierTableFinder.Object;
}

FWeaponData UInventoryComponent::Craft_Weapon(const FWeaponData& SelectWeapon, const TArray<int32>& ClickedResources)
{
	// Count Clicked Sum
	FWeaponData Ret = SelectWeapon;
	int32 Sum{};
	for (int32 i = 0; i < static_cast<int32>(EResourceType::End); i++)
	{
		Ret.UsedResourceCounts[i] = static_cast<uint8>(ClickedResources[i]);
		Sum += ClickedResources[i];
		AddResource(GetResourceDataAsset_ByResourceEnum(i), -ClickedResources[i]);
	}

	//  Determine Type of Weapon
	int32 RandomIndex{ 0 }, RandomNum = rand() % Sum;
	while (true)
	{
		RandomNum -= ClickedResources[RandomIndex];
		if (RandomNum < 0)
			break;
		RandomIndex++;
	}

	// Determine Rarity
	int32 Rarity = rand() % 2; // 0 or 1
	if (ModifierDataTable)
	{
		TArray<FWeaponModifier*> AllRows;
		ModifierDataTable->GetAllRows(TEXT("ModifierSearch"), AllRows);

		for (const FWeaponModifier* Row : AllRows)
		{
			if (Row->ResourceType == (EResourceType)RandomIndex && Row->Rarity == Rarity)
			{
				Ret.WeaponModifier = Row->ModifierText;
				break;
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ModifierDataTable is null!"));
	}

	return Ret;
}

void UInventoryComponent::AddResource(UResourceDataAsset* Resource, int32 Amount)
{
	if (!Resource)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid resource in UInventoryComponent::AddResource"));
		return;
	}

	int ResourceIndex = static_cast<int>(Resource->ResourceType);
	if (ResourceInventory[ResourceIndex].Count + Amount < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Add Amont in UInventoryComponent::AddResource"));
		return;
	}

	ResourceInventory[ResourceIndex].Count += Amount;

	// 클라이언트의 값을 변경하고 있다면 자동으로 OnRep이 호출되지만,
	// 서버는 임의로 호출해 주어야 함.
	if (GetOwnerRole() == ROLE_Authority)
	{
		OnRep_ResourceInventory();
	}
}

int32 UInventoryComponent::GetResourceQuantity(UResourceDataAsset* Resource) const
{
	if (!Resource) return 0;

	return ResourceInventory[static_cast<int>(Resource->ResourceType)].Count;
}

UResourceDataAsset* UInventoryComponent::GetResourceDataAsset_ByResourceEnum(const int32& Enum)
{
	if (static_cast<EResourceType>(Enum) >= EResourceType::End)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wrong Index in UInventoryComponent::GetResourceDataAsset_ByResourceEnum"));
		return nullptr;
	}

	return ResourceInventory[Enum].Resource;
}

void UInventoryComponent::Clear_ZeroResources()
{
	UE_LOG(LogTemp, Warning, TEXT("Call Legacy Code in UInventoryComponent::Clear_ZeroResources"));
	return;
}

TArray<UResourceDataAsset*> UInventoryComponent::Get_OwnedDataAssets()
{
	TArray<UResourceDataAsset*> Ret;
	for (auto& FAA : ResourceInventory)
	{
		if (FAA.Count > 0)
			Ret.Push(FAA.Resource);
	}

	return Ret;
}

void UInventoryComponent::OnRep_ResourceInventory()
{
	OnResourceInventoryUpdated.Broadcast(ResourceInventory);
}
