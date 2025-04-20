#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ResourceDataAsset.h"
#include "InventoryComponent.generated.h"

struct FWeaponData;

USTRUCT(BlueprintType)
struct FResourceInventoryData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    UResourceDataAsset* Resource{ nullptr };

    UPROPERTY(BlueprintReadOnly)
    int32 Count{ 0 };
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FResourceInventoryUpdated, const TArray<FResourceInventoryData>&, ResourceInventory);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHOOTINGSTAR_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    UInventoryComponent();

    UPROPERTY(BlueprintAssignable)
    FResourceInventoryUpdated OnResourceInventoryUpdated;

    // 자원 인벤토리: 자원 데이터 에셋 → 수량
    UPROPERTY(ReplicatedUsing=OnRep_ResourceInventory, VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TArray<FResourceInventoryData> ResourceInventory{};
    
    // Weapon
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Data")
    UDataTable* ModifierDataTable{};

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    FWeaponData Craft_Weapon(const FWeaponData& SelectWeapon, const TArray<int32>& ClickedResources);
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddResource(UResourceDataAsset* Resource, int32 Amount = 1);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetResourceQuantity(UResourceDataAsset* Resource) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<FResourceInventoryData>& GetAllResources() { return ResourceInventory; }

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    UResourceDataAsset* GetResourceDataAsset_ByResourceEnum(const int32& Enum);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void Clear_ZeroResources();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<UResourceDataAsset*> Get_OwnedDataAssets();

private:
    UFUNCTION()
    void OnRep_ResourceInventory();
};
