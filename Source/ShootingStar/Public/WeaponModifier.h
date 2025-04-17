// WeaponModifier.h

#pragma once

#include "CoreMinimal.h"
#include "ResourceType.h"
#include "WeaponModifier.generated.h"

USTRUCT(BlueprintType)
struct FWeaponModifier : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EResourceType ResourceType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Rarity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText ModifierText;
};
