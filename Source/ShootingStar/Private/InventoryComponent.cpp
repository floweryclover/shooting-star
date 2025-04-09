#include "InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::AddResource(UResourceDataAsset* Resource, int32 Amount)
{
    if (!Resource || Amount <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid resource or amount."));
        return;
    }

    if (ResourceInventory.Contains(Resource))
    {
        ResourceInventory[Resource] += Amount;
    }
    else
    {
        ResourceInventory.Add(Resource, Amount);
    }

    UE_LOG(LogTemp, Log, TEXT("Added %d of %s. Total: %d"),
        Amount,
        *Resource->DisplayName.ToString(),
        ResourceInventory[Resource]);
}

int32 UInventoryComponent::GetResourceQuantity(UResourceDataAsset* Resource) const
{
    if (!Resource) return 0;

    const int32* FoundAmount = ResourceInventory.Find(Resource);
    return FoundAmount ? *FoundAmount : 0;
}
