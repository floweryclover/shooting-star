// Copyright 2025 ShootingStar. All Rights Reserved.


#include "ProceduralMapGenerator.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(ProceduralMapGenerator);

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

    InitializeMapCoordinate(MapHalfSize * 2);
    GenerateMap();
}

void AProceduralMapGenerator::InitializeMapCoordinate(int32 GridSize)
{
    mapCoordinate.SetNum(GridSize * GridSize); // 1차원 배열로 할당
}

// 랜덤 포지션과 랜덤 오브젝트를 선정해 맵을 생성하는 함수
void AProceduralMapGenerator::GenerateMap()
{
    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generate Map Start"));

    if (ObjectMeshes.Num() == 0)
    {
        UE_LOG(ProceduralMapGenerator, Error, TEXT("No Static Meshes assigned in ObjectMeshes array!"));
        return;
    }
    else
        UE_LOG(ProceduralMapGenerator, Log, TEXT("ObjectMeshes Num: %d"), ObjectMeshes.Num());
    
    int32 SpawnAttempts = 0;
    int32 PlacedObjects = 0;

    while (PlacedObjects < NumObjects && SpawnAttempts < NumObjects * 5)
    {
        FVector RandomLocation = GetRandomPosition();
        UStaticMesh* RandomMesh = nullptr;

        if (IsLocationValid(RandomLocation))
        {
            int32 RandomIndex = FMath::RandRange(0, ObjectMeshes.Num() - 1);
            
            if(ObjectMeshes.IsValidIndex(RandomIndex))
            {
                RandomMesh = ObjectMeshes[RandomIndex];
                if (PlaceObject(RandomLocation, RandomMesh))
                {
                    PlacedObjects++;
                    UE_LOG(ProceduralMapGenerator, Log, TEXT("%s 메쉬를 %s 위치에 생성 중"), *RandomMesh->GetName(), *RandomLocation.ToString());
                }
            }
            else
            {
                UE_LOG(ProceduralMapGenerator, Error, TEXT("Invalid index selected for ObjectMeshes"));
            }
        }
        else
        {
            UE_LOG(ProceduralMapGenerator, Warning, TEXT("%s은 유효하지 않은 위치입니다."), *RandomLocation.ToString());
        }
        UE_LOG(ProceduralMapGenerator, Log, TEXT("Generate Map Attempt . . . %d, %d, %s"), PlacedObjects, SpawnAttempts, RandomMesh ? *RandomMesh->GetName() : TEXT("None"));

        SpawnAttempts++;
    }
}

void AProceduralMapGenerator::GenerateBuildings()
{
    // Generate Buildings
}

void AProceduralMapGenerator::GenerateWalls()
{
    // Generate Walls
}

void AProceduralMapGenerator::GenerateResources()
{
    // Generate Resources
}

// 랜덤한 좌표를 FVector로 반환하는 함수
FORCEINLINE FVector AProceduralMapGenerator::GetRandomPosition()
{
    float X = FMath::RandRange(-MapHalfSize, MapHalfSize);
    float Y = FMath::RandRange(-MapHalfSize, MapHalfSize);
    return FVector(X, Y, 0.f);
}

/// 해당 위치가 유효한지 검사하는 함수 (충돌 방지)
/// 충돌 검사에서 좌표 검사로 수정할 예정
bool AProceduralMapGenerator::IsLocationValid(FVector Location)
{
    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;

    // PlaneActor가 유효하면 무시하도록 설정
    if (PlaneActor)
    {
        CollisionParams.AddIgnoredActor(PlaneActor);
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

            UE_LOG(ProceduralMapGenerator, Log, TEXT("Place Object 실행: %s"), *Location.ToString());

            return true;
        }
    }
    return false;
}