// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FenceData.h"
#include "MapEnum.h"
#include "IObjectGenerator.h"
#include "FenceGenerator.generated.h"

UCLASS(Blueprintable)
class SHOOTINGSTAR_API UFenceGenerator : public UObject, public IObjectGenerator
{
    GENERATED_BODY()

public:
    UFenceGenerator();

    virtual void GenerateObjects() override;
    virtual void Initialize(class UMapGeneratorComponent* InOwner) override;
    
    UPROPERTY(EditAnywhere, Category = "Generation Settings")
    int32 numFences;

    UPROPERTY(EditAnywhere, Category = "Generation Settings")
    float fenceMinDistance;

    UPROPERTY(EditAnywhere, Category = "Meshes")
    UStaticMesh* fenceMesh;
private:
    UPROPERTY()
    UMapGeneratorComponent* Owner;

    void GenerateFencePattern(const FVector& Center, EPatternType PatternType, float Radius, TArray<FFenceData>& OutPositions);
    void PlaceFence(const TArray<FFenceData>& Positions);
    bool GenerateFenceAroundObstacle();
    float GetRandomFenceRadius(float minRadius, float maxRadius) const;
};