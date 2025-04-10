#pragma once

#include "CoreMinimal.h"
#include "FenceData.generated.h"

USTRUCT()
struct FFenceData
{
    GENERATED_BODY()

    FVector Location;
    FRotator Rotation;

    FFenceData() : Location(FVector::ZeroVector), Rotation(FRotator::ZeroRotator) {}
    FFenceData(FVector InLocation, FRotator InRotation) 
        : Location(InLocation), Rotation(InRotation) {}
};