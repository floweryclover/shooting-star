// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IObjectGenerator.h"
#include "ResourceGenerator.generated.h"

UCLASS(Blueprintable)
class SHOOTINGSTAR_API UResourceGenerator : public UObject, public IObjectGenerator
{
    GENERATED_BODY()

public:
    UResourceGenerator();

    virtual void GenerateObjects() override;
    virtual void Initialize(class UProceduralMapGenerator* InOwner) override;

    UPROPERTY(EditAnywhere, Category = "Generation Settings")
    int32 numResources = 30;

    UPROPERTY(EditAnywhere, Category = "Meshes")
    TArray<UStaticMesh*> resourceMeshes;

private:
    UPROPERTY()
    UProceduralMapGenerator* Owner;
};