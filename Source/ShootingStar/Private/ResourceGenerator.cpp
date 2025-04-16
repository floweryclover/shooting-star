// Copyright 2025 ShootingStar. All Rights Reserved.

#include "ResourceGenerator.h"
#include "ProceduralMapGenerator.h"

UResourceGenerator::UResourceGenerator()
{
}

void UResourceGenerator::Initialize(UProceduralMapGenerator* InOwner)
{
    Owner = InOwner;

    if (Owner)
    {
        resourceMeshes = Owner->resourceMeshes;
    }
}

void UResourceGenerator::GenerateObjects()
{
    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Resources Started"));

    if (!Owner)
    {
        UE_LOG(ProceduralMapGenerator, Error, TEXT("Owner is not initialized!"));
        return;
    }
    if (resourceMeshes.Num() == 0)
    {
        UE_LOG(ProceduralMapGenerator, Error, TEXT("No Static Meshes assigned in Resource Mesh array!"));
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

        int32 RandomIndex = FMath::RandRange(0, resourceMeshes.Num() - 1);
        if (resourceMeshes.IsValidIndex(RandomIndex))
        {
            UStaticMesh* RandomMesh = resourceMeshes[RandomIndex];
            if (Owner->PlaceObject(RandomLocation, RandomMesh))
            {
                PlacedObjects++;
                Owner->SetObjectRegion(RandomLocation, RandomMesh, EObjectMask::ResourceMask);
                UE_LOG(ProceduralMapGenerator, Log, TEXT("Generated Resources: %s at %s"), *RandomMesh->GetName(), *RandomLocation.ToString());
            }
        }
        SpawnAttempts++;
    }
}
