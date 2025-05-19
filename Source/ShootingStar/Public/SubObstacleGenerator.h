// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IObjectGenerator.h"
#include "SubObstacleGenerator.generated.h"

UCLASS(Blueprintable)
class SHOOTINGSTAR_API USubObstacleGenerator : public UObject, public IObjectGenerator
{
    GENERATED_BODY()

public:
    virtual void Initialize(class UMapGeneratorComponent* InOwner) override;
    virtual void GenerateObjects() override;

    UPROPERTY(EditAnywhere, Category = "Generation Settings")
    int32 numSubObstacles = 20;

    UPROPERTY(EditAnywhere, Category = "Generation Settings")
    float subObstacleMinDistance = 500.f;

    UPROPERTY(EditAnywhere, Category = "Meshes")
    TArray<UStaticMesh*> subObstacleMeshes;

private:
    UPROPERTY()
    UMapGeneratorComponent* Owner;
};