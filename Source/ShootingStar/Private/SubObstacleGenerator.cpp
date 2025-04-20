// Copyright 2025 ShootingStar. All Rights Reserved.

#include "SubObstacleGenerator.h"
#include "MapGeneratorComponent.h"

USubObstacleGenerator::USubObstacleGenerator()
{
}

void USubObstacleGenerator::Initialize(UMapGeneratorComponent* InOwner)
{
    Owner = InOwner;

    if (Owner)
    {
        numSubObstacles = Owner->GetNumSubObstacles();
        subObstacleMinDistance = Owner->GetSubObstacleMinDistance();
        subObstacleMeshes = Owner->GetSubObstacleMeshes();
    }
}

void USubObstacleGenerator::GenerateObjects()
{
    if (!Owner)
    {
        UE_LOG(MapGenerator, Error, TEXT("Owner is not initialized!"));
        return;
    }
    if (subObstacleMeshes.Num() == 0)
    {
        UE_LOG(MapGenerator, Error, TEXT("No Static Meshes assigned in subObstacleMeshes array!"));
        return;
    }

    int32 SpawnAttempts = 0;
    int32 PlacedObjects = 0;

    while (PlacedObjects < numSubObstacles && SpawnAttempts < numSubObstacles * 5)
    {
        FVector RandomLocation = Owner->GetRandomPosition();
        if (!Owner->CheckLocation(RandomLocation))
        {
            RandomLocation = Owner->FindNearestValidLocation(RandomLocation, subObstacleMinDistance, EObjectMask::SubObstacleMask);
        }

        int32 RandomIndex = FMath::RandRange(0, subObstacleMeshes.Num() - 1);
        if (subObstacleMeshes.IsValidIndex(RandomIndex))
        {
            UStaticMesh* RandomMesh = subObstacleMeshes[RandomIndex];
            if (Owner->PlaceObject(RandomLocation, RandomMesh))
            {
                PlacedObjects++;
                Owner->SetObjectRegion(RandomLocation, RandomMesh, EObjectMask::SubObstacleMask);
                UE_LOG(MapGenerator, Log, TEXT("Generated SubObstacle: %s at %s"), *RandomMesh->GetName(), *RandomLocation.ToString());
            }
        }
        SpawnAttempts++;
    }
}
