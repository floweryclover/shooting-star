// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IObjectGenerator.h"
#include "ResourceDataAsset.h"
#include "ResourceGenerator.generated.h"

USTRUCT()
struct FResourceSpawnData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    UResourceDataAsset* ResourceData;

    UPROPERTY(EditAnywhere)
    float SpawnProbability;
};

UCLASS(Blueprintable)
class SHOOTINGSTAR_API UResourceGenerator : public UObject, public IObjectGenerator
{
    GENERATED_BODY()

public:
    UResourceGenerator();

    virtual void GenerateObjects() override;
    virtual void Initialize(class ACompetitiveGameMode* InOwner) override;

    UPROPERTY(EditAnywhere, Category = "Generation Settings")
    int32 numResources = 30;

    UPROPERTY(EditAnywhere, Category = "Resource Settings")
    TSubclassOf<class AResourceActor> ResourceActorClass;

    UPROPERTY(EditAnywhere, Category = "Resource Settings")
    TArray<FResourceSpawnData> ResourceSpawnData;

private:
    UPROPERTY()
    ACompetitiveGameMode* Owner;

    UResourceDataAsset* SelectResourceDataAsset();
    bool SpawnResourceActor(const FVector& Location, UResourceDataAsset* ResourceData);
};