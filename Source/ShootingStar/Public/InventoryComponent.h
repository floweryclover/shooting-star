#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ResourceDataAsset.h"
#include "InventoryComponent.generated.h"

struct FWeaponData;

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
    TMap<UResourceDataAsset*, int32> ResourceInventory;

    // Weapon
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    UDataTable* ModifierDataTable{};

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    FWeaponData Craft_Weapon(const FWeaponData& SelectWeapon, const TArray<int32>& ClickedResources);
    
    // 자원 추가
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddResource(UResourceDataAsset* Resource, int32 Amount = 1);

    // 자원 수량 가져오기
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetResourceQuantity(UResourceDataAsset* Resource) const;

    // 전체 인벤토리 가져오기 (블루프린트에서 ForEach 사용 가능)
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TMap<UResourceDataAsset*, int32>& GetAllResources() { return ResourceInventory; }

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    UResourceDataAsset* GetResourceDataAsset_ByResourceEnum(const int32& Enum);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void Clear_ZeroResources();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    TArray<UResourceDataAsset*> Get_SortedResources();
};
