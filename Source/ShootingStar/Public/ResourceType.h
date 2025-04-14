#pragma once

#include "CoreMinimal.h"
#include "ResourceType.generated.h"

UENUM(BlueprintType)
enum class EResourceType : uint8
{
    Wood      UMETA(DisplayName = "Wood"),
    Stone     UMETA(DisplayName = "Stone"),
    Iron      UMETA(DisplayName = "Iron"),
    Uranium   UMETA(DisplayName = "Uranium"),
    End       UMETA(DisplayName = "EndPoint")
};