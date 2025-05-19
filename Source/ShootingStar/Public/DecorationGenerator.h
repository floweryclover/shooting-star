// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IObjectGenerator.h"
#include "DecorationGenerator.generated.h"

UCLASS(Blueprintable)
class SHOOTINGSTAR_API UDecorationGenerator : public UObject, public IObjectGenerator
{
    GENERATED_BODY()

public:
    virtual void Initialize(class UMapGeneratorComponent* InOwner) override;
    virtual void GenerateObjects() override;

    UPROPERTY(EditAnywhere, Category = "Generation Settings")
    int32 numDecos = 30;

    UPROPERTY(EditAnywhere, Category = "Generation Settings")
    float decoMinDistance = 50.f;

    UPROPERTY(EditAnywhere, Category = "Generation Settings")
    float clusterRadius = 300.f;

    UPROPERTY(EditAnywhere, Category = "Generation Settings")
    int32 maxClusterNum = 20;

    UPROPERTY(EditAnywhere, Category = "Meshes")
    TArray<UStaticMesh*> decoMeshes;

private:
    UPROPERTY()
    UMapGeneratorComponent* Owner;

    void GenerateClusteredDecorations(FVector origin, float radius, UStaticMesh* decoMesh);
};
