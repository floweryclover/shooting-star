// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IObjectGenerator.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "DecorationGenerator.generated.h"

UCLASS(Blueprintable)
class SHOOTINGSTAR_API UDecorationGenerator : public UObject, public IObjectGenerator
{
    GENERATED_BODY()

public:
    UDecorationGenerator();

    virtual void GenerateObjects() override;
    virtual void Initialize(class ACompetitiveGameMode* InOwner) override;

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
    ACompetitiveGameMode* Owner;

    // Weak Pointer로 설정한다. 컴포넌트의 소유권은 Owner에게 있다.
    UPROPERTY()
    TArray<UInstancedStaticMeshComponent*> DecorationInstancedMeshComponents;

    void GenerateClusteredDecorations(FVector origin, float radius, UStaticMesh* decoMesh);
};
