// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IObjectGenerator.h"
#include "ObstacleGenerator.generated.h"

UCLASS(Blueprintable)
class SHOOTINGSTAR_API UObstacleGenerator : public UObject, public IObjectGenerator
{
    GENERATED_BODY()

public:
    UObstacleGenerator();

    virtual void GenerateObjects() override;
    virtual void Initialize(class UProceduralMapGenerator* InOwner) override;

    UPROPERTY(EditAnywhere, Category = "Generation Settings")
    int32 numObstacles = 20;

    UPROPERTY(EditAnywhere, Category = "Generation Settings")
    float obstacleMinDistance = 1000.f;

    UPROPERTY(EditAnywhere, Category = "Meshes")
    TArray<UStaticMesh*> obstacleMeshes;

private:
    UPROPERTY()
    UProceduralMapGenerator* Owner;
};
