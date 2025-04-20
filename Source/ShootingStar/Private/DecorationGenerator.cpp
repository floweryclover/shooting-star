// Copyright 2025 ShootingStar. All Rights Reserved.

#include "DecorationGenerator.h"
#include "ProceduralMapGenerator.h"

UDecorationGenerator::UDecorationGenerator()
{
}

void UDecorationGenerator::Initialize(UProceduralMapGenerator* InOwner)
{
    Owner = InOwner;

    if (Owner)
    {
        numDecos = Owner->numDecos;
        decoMinDistance = Owner->decoMinDistance;
        clusterRadius = Owner->clusterRadius;
        maxClusterNum = Owner->maxClusterNum;
        decoMeshes = Owner->decoMeshes;

        // Owner의 컴포넌트들을 로컬 배열로 복사
        DecorationInstancedMeshComponents = Owner->DecorationInstancedMeshComponents;
        
        // 컴포넌트 설정
        for (int32 i = 0; i < DecorationInstancedMeshComponents.Num(); ++i)
        {
            if (UInstancedStaticMeshComponent* Component = DecorationInstancedMeshComponents[i])
            {
                Component->SetVisibility(true);
                Component->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
                UE_LOG(ProceduralMapGenerator, Log, TEXT("(Decoration) InstancedMeshComponent %d set successfully"), i);
            }
        }
    }
}

void UDecorationGenerator::GenerateObjects()
{
    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Decos Started"));

    if (!Owner)
    {
        UE_LOG(ProceduralMapGenerator, Error, TEXT("Owner is not initialized!"));
        return;
    }
    if (decoMeshes.Num() == 0)
    {
        UE_LOG(ProceduralMapGenerator, Error, TEXT("No Static Meshes assigned in deco Mesh array!"));
        return;
    }

    int32 SpawnAttempts = 0;
    int32 PlacedObjects = 0;

    while (PlacedObjects < numDecos && SpawnAttempts < numDecos * 5)
    {
        FVector RandomLocation = Owner->GetRandomPosition();
        if (!Owner->CheckLocation(RandomLocation))
        {
            RandomLocation = Owner->FindNearestValidLocation(RandomLocation, decoMinDistance, EObjectMask::DecoMask);
        }

        int32 RandomIndex = FMath::RandRange(0, decoMeshes.Num() - 1);
        if (decoMeshes.IsValidIndex(RandomIndex))
        {
            UStaticMesh* RandomMesh = decoMeshes[RandomIndex];
            if (FMath::RandBool())
            {
                GenerateClusteredDecorations(RandomLocation, clusterRadius, RandomMesh);
                PlacedObjects++;
            }
            else if (Owner->PlaceObject(RandomLocation, RandomMesh))
            {
                Owner->SetObjectRegion(RandomLocation, RandomMesh, EObjectMask::DecoMask);
                PlacedObjects++;
            }
        }
        else
            UE_LOG(ProceduralMapGenerator, Error, TEXT("Invalid index selected for Decorations"));

        SpawnAttempts++;
    }

    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Decos Completed"));
}

void UDecorationGenerator::GenerateClusteredDecorations(FVector origin, float radius, UStaticMesh* decoMesh)
{
    if (!decoMesh) return;

    // 군집의 개수를 랜덤으로 결정
    int32 clusterCount = FMath::RandRange(5, maxClusterNum);

    for (int32 i = 0; i < clusterCount; ++i)
    {
        // 군집의 위치를 랜덤으로 결정
        FVector offset = FMath::VRand() * FMath::RandRange(0.f, radius);
        offset.Z = 0.f; // Z축은 고정
        FVector location = origin + offset;

        // 위치가 유효한지 확인 후 배치
        if (Owner->CheckLocation(location))
        {
            Owner->PlaceObject(location, decoMesh);
            Owner->SetObjectRegion(location, decoMesh, EObjectMask::DecoMask);
        }
    }
}
