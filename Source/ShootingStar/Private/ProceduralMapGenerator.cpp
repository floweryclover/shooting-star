// Copyright 2025 ShootingStar. All Rights Reserved.


#include "ProceduralMapGenerator.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(ProceduralMapGenerator);

#pragma region 생성자 및 초기화
AProceduralMapGenerator::AProceduralMapGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

    // Root Component 설정
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

void AProceduralMapGenerator::BeginPlay()
{
	Super::BeginPlay();

    world = GetWorld();
    if (world == nullptr)
        UE_LOG(ProceduralMapGenerator, Error, TEXT("World is null!"));

    InitializeMapCoordinate(mapHalfSize * 2);
    GenerateMap();
}

void AProceduralMapGenerator::InitializeMapCoordinate(int32 GridSize)
{
    mapCoordinate.SetNum(GridSize * GridSize); // 1차원 배열로 할당
}
#pragma endregion

#pragma region 절차적 맵 생성
// 랜덤 포지션과 랜덤 오브젝트를 선정해 맵을 생성하는 함수
void AProceduralMapGenerator::GenerateMap()
{
    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generate Map Started"));

    GenerateObstacles(); // 장애물 생성
    GenerateSubObstacles(); // 서브 장애물 생성
    GenerateResources(); // 자원 생성
    GenerateDecos(); // 장식물 생성

    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generate Map Completed"));
}

// 건물, 큰 바위 등 주요 Obstalces 생성을 위한 함수
void AProceduralMapGenerator::GenerateObstacles()
{
    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generate Obstacles Started"));

    if (obstacleMeshes.Num() == 0)
    {
        UE_LOG(ProceduralMapGenerator, Error, TEXT("No Static Meshes assigned in obstacleMeshes array!"));
        return;
    }
    
    int32 SpawnAttempts = 0;
    int32 PlacedObjects = 0;

    while (PlacedObjects < numObstacles && SpawnAttempts < numObstacles * 5) // 최대 생성 시도 횟수는 배치할 오브젝트의 5배이다.
    {
        FVector RandomLocation = GetRandomPosition();
        UStaticMesh* RandomMesh = nullptr;

        if (IsLocationValid(RandomLocation))
        {
            int32 RandomIndex = FMath::RandRange(0, obstacleMeshes.Num() - 1);
            
            if(obstacleMeshes.IsValidIndex(RandomIndex))
            {
                RandomMesh = obstacleMeshes[RandomIndex];
                if (PlaceObject(RandomLocation, RandomMesh))
                {
                    PlacedObjects++;
                    UE_LOG(ProceduralMapGenerator, Log, TEXT("%s 메쉬를 %s 위치에 생성 중"), *RandomMesh->GetName(), *RandomLocation.ToString());
                }
            }
            else
            {
                UE_LOG(ProceduralMapGenerator, Error, TEXT("Invalid index selected for obstacleMeshes"));
            }
        }
        else
        {
            UE_LOG(ProceduralMapGenerator, Warning, TEXT("%s은 유효하지 않은 위치입니다."), *RandomLocation.ToString());
        }
        UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Map Attempt . . . %d, %d, %s"), PlacedObjects, SpawnAttempts, RandomMesh ? *RandomMesh->GetName() : TEXT("None"));

        SpawnAttempts++;
    }

    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Obstacles Completed"));
}

// 벽, 울타리 등의 Sub Obstacles 생성을 위한 함수
void AProceduralMapGenerator::GenerateSubObstacles()
{
    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Sub Obstacles Started"));

    if (subObstacleMeshes.Num() == 0)
    {
        UE_LOG(ProceduralMapGenerator, Error, TEXT("No Static Meshes assigned in obstacleMeshes array!"));
        return;
    }
    
    int32 SpawnAttempts = 0;
    int32 PlacedObjects = 0;

    while (PlacedObjects < numSubObstacles && SpawnAttempts < numSubObstacles * 5) // 최대 생성 시도 횟수는 배치할 오브젝트의 5배이다.
    {
        FVector RandomLocation = GetRandomPosition();
        UStaticMesh* RandomMesh = nullptr;

        if (IsLocationValid(RandomLocation))
        {
            int32 RandomIndex = FMath::RandRange(0, subObstacleMeshes.Num() - 1);
            
            if(subObstacleMeshes.IsValidIndex(RandomIndex))
            {
                RandomMesh = subObstacleMeshes[RandomIndex];
                if (PlaceObject(RandomLocation, RandomMesh))
                {
                    PlacedObjects++;
                    UE_LOG(ProceduralMapGenerator, Log, TEXT("%s 메쉬를 %s 위치에 생성 중"), *RandomMesh->GetName(), *RandomLocation.ToString());
                }
            }
            else
            {
                UE_LOG(ProceduralMapGenerator, Error, TEXT("Invalid index selected for Sub Obstacle Meshes"));
            }
        }
        else
        {
            UE_LOG(ProceduralMapGenerator, Warning, TEXT("%s은 유효하지 않은 위치입니다."), *RandomLocation.ToString());
        }
        UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Map Attempt . . . %d, %d, %s"), PlacedObjects, SpawnAttempts, RandomMesh ? *RandomMesh->GetName() : TEXT("None"));

        SpawnAttempts++;
    }

    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Sub Obstacles Completed"));
}

// 돌, 철, 우라늄 등의 자원 생성을 위한 함수
void AProceduralMapGenerator::GenerateResources()
{
    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Resources Started"));

    if (resourceMeshes.Num() == 0)
    {
        UE_LOG(ProceduralMapGenerator, Error, TEXT("No Static Meshes assigned in obstacleMeshes array!"));
        return;
    }
    
    int32 SpawnAttempts = 0;
    int32 PlacedObjects = 0;

    while (PlacedObjects < numResources && SpawnAttempts < numResources * 5) // 최대 생성 시도 횟수는 배치할 오브젝트의 5배이다.
    {
        FVector RandomLocation = GetRandomPosition();
        UStaticMesh* RandomMesh = nullptr;

        if (IsLocationValid(RandomLocation))
        {
            int32 RandomIndex = FMath::RandRange(0, resourceMeshes.Num() - 1);
            
            if(resourceMeshes.IsValidIndex(RandomIndex))
            {
                RandomMesh = resourceMeshes[RandomIndex];
                if (PlaceObject(RandomLocation, RandomMesh))
                {
                    PlacedObjects++;
                    UE_LOG(ProceduralMapGenerator, Log, TEXT("%s 메쉬를 %s 위치에 생성 중"), *RandomMesh->GetName(), *RandomLocation.ToString());
                }
            }
            else
            {
                UE_LOG(ProceduralMapGenerator, Error, TEXT("Invalid index selected for resource Meshes"));
            }
        }
        else
        {
            UE_LOG(ProceduralMapGenerator, Warning, TEXT("%s은 유효하지 않은 위치입니다."), *RandomLocation.ToString());
        }
        UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Map Attempt . . . %d, %d, %s"), PlacedObjects, SpawnAttempts, RandomMesh ? *RandomMesh->GetName() : TEXT("None"));

        SpawnAttempts++;
    }

    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Resources Completed"));
}

void AProceduralMapGenerator::GenerateDecos()
{
    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Decos Started"));

    if (decoMeshes.Num() == 0)
    {
        UE_LOG(ProceduralMapGenerator, Error, TEXT("No Static Meshes assigned in obstacleMeshes array!"));
        return;
    }
    
    int32 SpawnAttempts = 0;
    int32 PlacedObjects = 0;

    while (PlacedObjects < numDecos && SpawnAttempts < numDecos * 5) // 최대 생성 시도 횟수는 배치할 오브젝트의 5배이다.
    {
        FVector RandomLocation = GetRandomPosition();
        UStaticMesh* RandomMesh = nullptr;

        if (IsLocationValid(RandomLocation))
        {
            int32 RandomIndex = FMath::RandRange(0, decoMeshes.Num() - 1);
            
            if(decoMeshes.IsValidIndex(RandomIndex))
            {
                RandomMesh = decoMeshes[RandomIndex];
                if (PlaceObject(RandomLocation, RandomMesh))
                {
                    PlacedObjects++;
                    UE_LOG(ProceduralMapGenerator, Log, TEXT("%s 메쉬를 %s 위치에 생성 중"), *RandomMesh->GetName(), *RandomLocation.ToString());
                }
            }
            else
            {
                UE_LOG(ProceduralMapGenerator, Error, TEXT("Invalid index selected for Decorations"));
            }
        }
        else
        {
            UE_LOG(ProceduralMapGenerator, Warning, TEXT("%s은 유효하지 않은 위치입니다."), *RandomLocation.ToString());
        }
        UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Map Attempt . . . %d, %d, %s"), PlacedObjects, SpawnAttempts, RandomMesh ? *RandomMesh->GetName() : TEXT("None"));

        SpawnAttempts++;
    }

    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Decos Completed"));
}

#pragma endregion

#pragma region Utilities & Helpers
// 랜덤한 좌표를 FVector로 반환하는 함수
FORCEINLINE FVector AProceduralMapGenerator::GetRandomPosition()
{
    float X = FMath::RandRange(-mapHalfSize, mapHalfSize);
    float Y = FMath::RandRange(-mapHalfSize, mapHalfSize);
    return FVector(X, Y, 0.f);
}

/// 해당 위치가 유효한지 검사하는 함수 (충돌 방지)
/// 충돌 검사에서 좌표 검사로 수정할 예정
bool AProceduralMapGenerator::IsLocationValid(FVector Location)
{
    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;

    // PlaneActor가 유효하면 무시하도록 설정
    if (planeActor)
    {
        CollisionParams.AddIgnoredActor(planeActor);
    }
    else
    {
        UE_LOG(ProceduralMapGenerator, Warning, TEXT("PlaneActor가 설정되지 않았습니다."));
    }
    
    return !world->SweepSingleByChannel(
        HitResult,
        Location,
        Location,
        FQuat::Identity,
        ECC_WorldStatic,
        FCollisionShape::MakeSphere(50.f), // 충돌 검사 반경 설정
        CollisionParams
    );
}

// 오브젝트를 원하는 위치에 설정하는 함수
bool AProceduralMapGenerator::PlaceObject(FVector Location, UStaticMesh* ObjectMesh)
{
    if (ObjectMesh)
    {
        // StaticMeshComponent 생성
        UStaticMeshComponent* NewMeshComponent = NewObject<UStaticMeshComponent>(this);
        if (NewMeshComponent)
        {
            NewMeshComponent->RegisterComponent();
            NewMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
            NewMeshComponent->SetStaticMesh(ObjectMesh);
            NewMeshComponent->SetWorldLocation(Location);

            UE_LOG(ProceduralMapGenerator, Log, TEXT("Place Object: %s"), *Location.ToString());

            return true;
        }
    }
    return false;
}
#pragma endregion