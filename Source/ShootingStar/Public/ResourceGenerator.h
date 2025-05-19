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
    virtual void Initialize(class UMapGeneratorComponent* InOwner) override;
    virtual void GenerateObjects() override;
    void ClearSpawnedResources();
    
    UResourceDataAsset* SelectResourceDataAsset();
    bool SpawnResourceActor(const FVector& Location, UResourceDataAsset* ResourceData);

    UPROPERTY(EditAnywhere, Category = "Generation Settings")
    int32 numResources = 30;

    UPROPERTY(EditAnywhere, Category = "Resource Settings")
    TSubclassOf<class AResourceActor> ResourceActorClass;

    UPROPERTY(EditAnywhere, Category = "Resource Settings")
    TArray<FResourceSpawnData> ResourceSpawnData;

private:
    UPROPERTY()
    UMapGeneratorComponent* Owner;

    // 현재 생성된 자원 액터들을 추적하기 위한 배열
    UPROPERTY()
    TArray<AResourceActor*> SpawnedResources;
};