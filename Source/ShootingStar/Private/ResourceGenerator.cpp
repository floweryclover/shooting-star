// Copyright 2025 ShootingStar. All Rights Reserved.

#include "ResourceGenerator.h"
#include "MapGeneratorComponent.h"
#include "ResourceActor.h"

void UResourceGenerator::Initialize(UMapGeneratorComponent* InOwner)
{
    Owner = InOwner;

    if (Owner)
    {
        numResources = Owner->GetNumResources();
        ResourceActorClass = Owner->GetResourceActorClass();
        ResourceSpawnData = Owner->GetResourceSpawnData();
    }
    else
    {
        UE_LOG(MapGenerator, Error, TEXT("(Resource) Owner is not valid!"));
    }
}

void UResourceGenerator::GenerateObjects()
{
    // 기존 자원들 제거
    ClearSpawnedResources();
    
    UE_LOG(MapGenerator, Log, TEXT("(Resource) Generating Resources Started"));

    if (!ResourceActorClass)
    {
        UE_LOG(MapGenerator, Error, TEXT("(Resource) ResourceActorClass is not set!"));
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

            if (RandomLocation == FVector::ZeroVector)
                continue;
        }

        UResourceDataAsset* SelectedResource = SelectResourceDataAsset();
        if (SelectedResource && SpawnResourceActor(RandomLocation, SelectedResource))
        {
            PlacedObjects++;
            Owner->SetObjectRegion(RandomLocation, SelectedResource->LargeMesh, EObjectMask::ResourceMask);
            UE_LOG(MapGenerator, Log, TEXT("(Resource) Generated %s at Location: X=%.1f Y=%.1f"), 
                *SelectedResource->DisplayName.ToString(), RandomLocation.X, RandomLocation.Y);
        }
        SpawnAttempts++;
    }

    UE_LOG(MapGenerator, Log, TEXT("(Resource) Generating Resources Completed"));
}

UResourceDataAsset* UResourceGenerator::SelectResourceDataAsset()
{
    if (ResourceSpawnData.Num() == 0) return nullptr;

    float TotalProbability = 1.0f;
    float RandomValue = FMath::FRand();
    float AccumulatedProbability = 0.0f;

    for (const FResourceSpawnData& SpawnData : ResourceSpawnData)
    {
        AccumulatedProbability += SpawnData.SpawnProbability;
        if (RandomValue <= AccumulatedProbability)
            return SpawnData.ResourceData;
    }

    UE_LOG(MapGenerator, Warning, TEXT("No resource data selected based on probabilities, so returning 나무 as fallback."));
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
        SpawnedResources.Add(ResourceActor);
        return true;
    }

    return false;
}

void UResourceGenerator::ClearSpawnedResources()
{
    // 기존에 스폰된 자원들 제거
    for (AResourceActor* Resource : SpawnedResources)
    {
        if (IsValid(Resource))
            Resource->Destroy();
    }
    SpawnedResources.Empty();

    UE_LOG(MapGenerator, Log, TEXT("(Resource) Cleared all spawned resources"));
}