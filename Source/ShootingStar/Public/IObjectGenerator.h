// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IObjectGenerator.generated.h"

UINTERFACE(MinimalAPI)
class UObjectGenerator : public UInterface
{
    GENERATED_BODY()
};

class SHOOTINGSTAR_API IObjectGenerator
{
    GENERATED_BODY()

public:
    virtual void GenerateObjects() = 0;
    virtual void Initialize(class UProceduralMapGenerator* InOwner) = 0;
};
