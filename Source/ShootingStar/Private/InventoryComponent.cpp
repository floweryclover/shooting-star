#include "InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
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
