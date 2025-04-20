#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ResourceDataAsset.h"
#include "InventoryComponent.generated.h"

struct FWeaponData;

USTRUCT(BlueprintType)
struct FAA
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    UResourceDataAsset* Resource{ nullptr };

    UPROPERTY(BlueprintReadOnly)
    int32 Count{ 0 };
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHOOTINGSTAR_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:
    UInventoryComponent();

    // 자원 인벤토리: 자원 데이터 에셋 → 수량
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TArray<FAA> ResourceInventory{};

    // Weapon
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    UDataTable* ModifierDataTable{};

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    FWeaponData Craft_Weapon(const FWeaponData& SelectWeapon, const TArray<int32>& ClickedResources);
    
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddResource(UResourceDataAsset* Resource, int32 Amount = 1);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetResourceQuantity(UResourceDataAsset* Resource) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<FAA>& GetAllResources() { return ResourceInventory; }

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    UResourceDataAsset* GetResourceDataAsset_ByResourceEnum(const int32& Enum);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void Clear_ZeroResources();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<UResourceDataAsset*> Get_OwnedDataAssets();
};
