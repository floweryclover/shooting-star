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
    virtual void Initialize(class UMapGeneratorComponent* InOwner) override;
    virtual void GenerateObjects() override;

    UPROPERTY(EditAnywhere, Category = "Generation Settings")
    int32 numObstacles = 20;

    UPROPERTY(EditAnywhere, Category = "Generation Settings")
    float obstacleMinDistance = 1000.f;

    UPROPERTY(EditAnywhere, Category = "Meshes")
    TArray<UStaticMesh*> obstacleMeshes;

private:
    UPROPERTY()
    UMapGeneratorComponent* Owner;
};
