#include "InventoryComponent.h"
#include "WeaponData.h"
#include "WeaponModifier.h"

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
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
	for (int32 i = 0; i < (int32)EResourceType::End; i++)
	{
		Ret.UsedResourceCounts[i] = static_cast<uint8>(ClickedResources[i]);
		Sum += ClickedResources[i];
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

	if (ResourceInventory.Contains(Resource))
	{
		if (ResourceInventory[Resource] + Amount < 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid Add Amont in UInventoryComponent::AddResource"));
			return;
		}

		ResourceInventory[Resource] += Amount;
	}
	else
	{
		if (Amount < 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Negative Initialize in UInventoryComponent::AddResource"));
			return;
		}

		ResourceInventory.Add(Resource, Amount);
	}
}

int32 UInventoryComponent::GetResourceQuantity(UResourceDataAsset* Resource) const
{
	if (!Resource) return 0;

	const int32* FoundAmount = ResourceInventory.Find(Resource);
	return FoundAmount ? *FoundAmount : 0;
}

UResourceDataAsset* UInventoryComponent::GetResourceDataAsset_ByResourceEnum(const int32& Enum)
{
	if (static_cast<EResourceType>(Enum) >= EResourceType::End)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wrong Index in UInventoryComponent::GetResourceDataAsset_ByResourceEnum"));
		return nullptr;
	}

	for (auto& Pair : ResourceInventory)
	{
		if (Pair.Key->ResourceType == static_cast<EResourceType>(Enum))
			return Pair.Key;
	}

	return nullptr;
}

void UInventoryComponent::Clear_ZeroResources()
{
	TArray<UResourceDataAsset*> KeysToRemove;

	for (auto& Pair : ResourceInventory)
	{
		if (Pair.Value == 0)
			KeysToRemove.Add(Pair.Key);
	}

	for (auto& Key : KeysToRemove)
	{
		ResourceInventory.Remove(Key);
	}
}

TArray<UResourceDataAsset*> UInventoryComponent::Get_SortedResources()
{
	TArray<UResourceDataAsset*> Ret;
	for (auto& Pair : ResourceInventory)
	{
		Ret.Push(Pair.Key);
	}

	Ret.Sort([](const UResourceDataAsset& A, const UResourceDataAsset& B)
		{
			return A.ResourceType < B.ResourceType;
		});

	return Ret;
}
