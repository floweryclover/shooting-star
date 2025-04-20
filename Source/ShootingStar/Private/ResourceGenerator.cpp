// Copyright 2025 ShootingStar. All Rights Reserved.

#include "ResourceGenerator.h"
#include "ProceduralMapGenerator.h"
#include "ResourceActor.h"

UResourceGenerator::UResourceGenerator()
{
}

void UResourceGenerator::Initialize(UProceduralMapGenerator* InOwner)
{
    Owner = InOwner;

    if (Owner)
    {
        numResources = Owner->numResources;
        ResourceActorClass = Owner->ResourceActorClass;
        ResourceSpawnData = Owner->ResourceSpawnData;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("(Resource) Owner is not valid!"));
    }
}

void UResourceGenerator::GenerateObjects()
{
    UE_LOG(ProceduralMapGenerator, Log, TEXT("(Resource) Generating Resources Started"));

    if (!ResourceActorClass)
    {
        UE_LOG(LogTemp, Error, TEXT("(Resource) ResourceActorClass is not set!"));
        return;
    }

    int32 SpawnAttempts = 0;
    int32 PlacedObjects = 0;

    while (PlacedObjects < numResources && SpawnAttempts < numResources * 5)
    {
        FVector RandomLocation = Owner->GetRandomPosition();
        if (!Owner->CheckLocation(RandomLocation))
        {
            RandomLocation = Owner->FindNearestValidLocation(RandomLocation, 500.f, EObjectMask::ResourceMask);
        }

        UResourceDataAsset* SelectedResource = SelectResourceDataAsset();
        if (SelectedResource && SpawnResourceActor(RandomLocation, SelectedResource))
        {
            PlacedObjects++;
            Owner->SetObjectRegion(RandomLocation, SelectedResource->Mesh, EObjectMask::ResourceMask);
            UE_LOG(ProceduralMapGenerator, Log, TEXT("(Resource) Generated %s at Location: X=%.1f Y=%.1f"), 
                *SelectedResource->DisplayName.ToString(), RandomLocation.X, RandomLocation.Y);
        }
        SpawnAttempts++;
    }

    UE_LOG(LogTemp, Log, TEXT("(Resource) Placed %d resources after %d attempts"), PlacedObjects, SpawnAttempts);

    UE_LOG(ProceduralMapGenerator, Log, TEXT("(Resource) Generating Resources Completed"));
}

UResourceDataAsset* UResourceGenerator::SelectResourceDataAsset()
{
    if (ResourceSpawnData.Num() == 0) return nullptr;

    float TotalProbability = 1.0f;
    float RandomValue = FMath::FRand();
    float AccumulatedProbability = 0.0f;

    UE_LOG(ProceduralMapGenerator, Log, TEXT("Selecting resource data asset... in %d Resource Spawn Data "), ResourceSpawnData.Num());
    for (const FResourceSpawnData& SpawnData : ResourceSpawnData)
    {
        AccumulatedProbability += SpawnData.SpawnProbability;
        if (RandomValue <= AccumulatedProbability)
        {
            UE_LOG(ProceduralMapGenerator, Log, TEXT("Selected resource: %s with probability: %.2f"), 
                *SpawnData.ResourceData->DisplayName.ToString(), SpawnData.SpawnProbability);
            return SpawnData.ResourceData;
        }
    }

    UE_LOG(ProceduralMapGenerator, Warning, TEXT("No resource data selected based on probabilities, so returning 나무 as fallback."));
    // 어느 자원도 선택되지 않았다면, fallback으로 첫번째 자원(나무)을 반환
    return ResourceSpawnData[0].ResourceData;
}

bool UResourceGenerator::SpawnResourceActor(const FVector& Location, UResourceDataAsset* ResourceData)
{
    if (!Owner || !ResourceActorClass || !ResourceData) return false;

    UWorld* World = Owner->GetWorld();
    if (!World) return false;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = Owner->GetOwner();

    AResourceActor* ResourceActor = World->SpawnActor<AResourceActor>(
        ResourceActorClass,
        Location,
        FRotator::ZeroRotator,
        SpawnParams
    );

    if (ResourceActor)
    {
        ResourceActor->ResourceData = ResourceData;
        ResourceActor->UpdateVisual();
        return true;
    }

    return false;
}