// Copyright 2025 ShootingStar. All Rights Reserved.

#include "ProceduralMapGenerator.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h" // Include for UInstancedStaticMeshComponent
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMeshActor.h" // Include for AStaticMeshActor

DEFINE_LOG_CATEGORY(ProceduralMapGenerator);

#pragma region 생성자 및 초기화
AProceduralMapGenerator::AProceduralMapGenerator()
{
	PrimaryActorTick.bCanEverTick = false;

    // Root Component 설정
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // InstancedStaticMeshComponents 생성
    FenceInstancedMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FenceInstancedMesh"));
    FenceInstancedMeshComponent->SetupAttachment(RootComponent);

    ResourceInstancedMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ResourceInstancedMesh"));
    ResourceInstancedMeshComponent->SetupAttachment(RootComponent);

    DecorationInstancedMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("DecorationInstancedMesh"));
    DecorationInstancedMeshComponent->SetupAttachment(RootComponent);
}

void AProceduralMapGenerator::BeginPlay()
{
	Super::BeginPlay();

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
    GenerateFences(); // 울타리 생성
    GenerateResources(); // 자원 생성
    GenerateDecorations(); // 장식물 생성

    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generate Map Completed"));
}

#pragma region Obstacles
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
        int32 RandomIndex = FMath::RandRange(0, obstacleMeshes.Num() - 1);

        // 위치가 유효하지 않다면 근처의 유효한 위치를 재탐색
        if (!CheckLocation(RandomLocation))
            RandomLocation = FindNearestValidLocation(RandomLocation, obstacleMinDistance, EObjectMask::ObstacleMask);

        // 인덱스 유효성 체크
        if(obstacleMeshes.IsValidIndex(RandomIndex))
        {
            RandomMesh = obstacleMeshes[RandomIndex];

            if (PlaceObject(RandomLocation, RandomMesh))
            {
                PlacedObjects++;
                SetObjectRegion(RandomLocation, RandomMesh, EObjectMask::ObstacleMask);
                UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating %s Meshs at location %s "), RandomMesh ? *RandomMesh->GetName() : TEXT("Unknown"), *RandomLocation.ToString());
            }
        }
        else
            UE_LOG(ProceduralMapGenerator, Error, TEXT("Invalid index selected for obstacleMeshes"));

        UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Map Attempt . . . %d, %d, %s"), PlacedObjects, SpawnAttempts, RandomMesh ? *RandomMesh->GetName() : TEXT("None"));

        SpawnAttempts++;
    }

    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Obstacles Completed"));
}
#pragma endregion

#pragma region Sub Obstacles
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

    // 최대 생성 시도 횟수는 배치할 오브젝트의 5배로 설정
    while (PlacedObjects < numSubObstacles && SpawnAttempts < numSubObstacles * 5)
    {
        FVector RandomLocation = GetRandomPosition();
        UStaticMesh* RandomMesh = nullptr;
        int32 RandomIndex = FMath::RandRange(0, subObstacleMeshes.Num() - 1);
        
        // 위치가 유효하지 않다면 근처의 유효한 위치를 재탐색
        if (!CheckLocation(RandomLocation))
            RandomLocation = FindNearestValidLocation(RandomLocation, 500.f, EObjectMask::SubObstacleMask);

        // 인덱스 유효성 체크
        if (subObstacleMeshes.IsValidIndex(RandomIndex))
        {
            RandomMesh = subObstacleMeshes[RandomIndex];

            if (PlaceObject(RandomLocation, RandomMesh))
            {
                PlacedObjects++;
                SetObjectRegion(RandomLocation, RandomMesh, EObjectMask::SubObstacleMask);
                UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating %s Meshs at location %s "), RandomMesh ? *RandomMesh->GetName() : TEXT("Unknown"), *RandomLocation.ToString());
            }
        }
        else
            UE_LOG(ProceduralMapGenerator, Error, TEXT("Invalid index selected for obstacleMeshes"));

        SpawnAttempts++;
    }

    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Sub Obstacles Completed"));
}
#pragma endregion

#pragma region Fences
void AProceduralMapGenerator::GenerateFences()
{
    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Fences Started"));

    if (fenceMesh == nullptr)
    {
        UE_LOG(ProceduralMapGenerator, Error, TEXT("No Static Meshes assigned in fenceMesh!"));
        return;
    }
    else
        FenceInstancedMeshComponent->SetStaticMesh(fenceMesh);


    int32 SpawnAttempts = 0;
    int32 PlacedObjects = 0;

    while (PlacedObjects < numFences && SpawnAttempts < numFences * 5)
    {
        // 50% 확률로 Obstacle 주변 또는 랜덤 위치에 생성
        if (FMath::RandBool())
        {
            // Obstacle 주변에 생성
            if (GenerateFenceAroundObstacle())
                PlacedObjects++;
        }
        else
        {
            FVector RandomLocation = GetRandomPosition();
            if (!CheckLocation(RandomLocation))
            {
                RandomLocation = FindNearestValidLocation(RandomLocation, 500.f, EObjectMask::FenceMask);
            }

            EPatternType RandomPattern = static_cast<EPatternType>(FMath::RandRange(0, 2));
            TArray<FFenceData> FencePositions;
            GenerateFencePattern(RandomLocation, RandomPattern, FMath::RandRange(400.f, 800.f), FencePositions);

            if (PlaceFencePattern(FencePositions))
                PlacedObjects++;
        }
        SpawnAttempts++;
    }

    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Fences Completed"));
}

// 생성할 펜스의 모양을 랜덤하게 결정하고, 펜스의 위치들을 Fvector 배열에 담아주는 함수
// 펜스의 모양은 Rectangle, UShape, LShape, Surrounding이 존재하며 생성할 패턴 enum을 인수로 받는다
void AProceduralMapGenerator::GenerateFencePattern(const FVector& Center, EPatternType PatternType, float Radius, TArray<FFenceData>& OutPositions)
{    
    switch (PatternType)
    {
        case EPatternType::Rectangle:
            {
                // 가로 방향 펜스
                for (float X = -Radius; X < Radius; X += fenceMinDistance)
                {
                    OutPositions.Add(FFenceData(
                        FVector(Center.X + X, Center.Y + Radius, 0.f),
                        FRotator(0.f, 0.f, 0.f)
                    ));
                    OutPositions.Add(FFenceData(
                        FVector(Center.X + X, Center.Y - Radius, 0.f),
                        FRotator(0.f, 0.f, 0.f)
                    ));
                }
                // 세로 방향 펜스
                for (float Y = -Radius; Y < Radius; Y += fenceMinDistance)
                {
                    OutPositions.Add(FFenceData(
                        FVector(Center.X + Radius, Center.Y + Y, 0.f),
                        FRotator(0.f, 90.f, 0.f)
                    ));
                    OutPositions.Add(FFenceData(
                        FVector(Center.X - Radius, Center.Y + Y, 0.f),
                        FRotator(0.f, 90.f, 0.f)
                    ));
                }
            }
            break;
        case EPatternType::UShape:
            {
                const int32 UWidth = 2;
                const int32 UHeight = 2;
                for (int32 x = -UWidth; x <= UWidth; ++x)
                {
                    for (int32 y = -UHeight; y <= UHeight; ++y)
                    {
                        if (y == -UHeight)
                        {
                            // U자 아래쪽 가로 방향
                            OutPositions.Add(FFenceData(
                                FVector(Center.X + x * fenceMinDistance, Center.Y + y * fenceMinDistance, 0.f),
                                FRotator(0.f, 0.f, 0.f)
                            ));
                        }
                        else if ((x == -UWidth || x == UWidth) && y != UHeight)
                        {
                            // U자 양쪽 세로 방향
                            OutPositions.Add(FFenceData(
                                FVector(Center.X + x * fenceMinDistance, Center.Y + y * fenceMinDistance, 0.f),
                                FRotator(0.f, 90.f, 0.f)
                            ));
                        }
                    }
                }
            }
            break;
        case EPatternType::LShape:
            {
                const int32 LWidth = 2;
                const int32 LHeight = 2;
                for (int32 x = -LWidth; x <= LWidth; ++x)
                {
                    for (int32 y = -LHeight; y <= LHeight; ++y)
                    {
                        if (y == -LHeight)
                        {
                            // L자 아래쪽 가로 방향
                            OutPositions.Add(FFenceData(
                                FVector(Center.X + x * fenceMinDistance, Center.Y + y * fenceMinDistance, 0.f),
                                FRotator(0.f, 0.f, 0.f)
                            ));
                        }
                        else if (x == -LWidth && y != LHeight)
                        {
                            // L자 왼쪽 세로 방향
                            OutPositions.Add(FFenceData(
                                FVector(Center.X + x * fenceMinDistance, Center.Y + y * fenceMinDistance, 0.f),
                                FRotator(0.f, 90.f, 0.f)
                            ));
                        }
                    }
                }
            }
            break;
        default:
            UE_LOG(ProceduralMapGenerator, Warning, TEXT("Invalid PatternType . . ."));
            break;
    }
}

bool AProceduralMapGenerator::PlaceFencePattern(const TArray<FFenceData>& Positions)
{
    if (Positions.Num() == 0) return false;

    bool bPlacedAny = false;
    for (const FFenceData& FenceData : Positions)
    {
        if (CheckLocation(FenceData.Location))
        {
            FenceInstancedMeshComponent->AddInstance(
                FTransform(FenceData.Rotation, FenceData.Location)
            );
            SetObjectRegion(FenceData.Location, fenceMesh, EObjectMask::FenceMask);
            bPlacedAny = true;
        }
    }
    return bPlacedAny;
}

// 현재 맵에 생성되어 있는 Obstacle를 탐색하고, 그중 하나를 선택하여 주변에 펜스를 생성하는 함수
bool AProceduralMapGenerator::GenerateFenceAroundObstacle()
{
    TArray<FVector> ObstacleLocations;
    // 맵에서 Obstacle 위치 찾기
    for (int32 X = -mapHalfSize; X < mapHalfSize; X += 100)
    {
        for (int32 Y = -mapHalfSize; Y < mapHalfSize; Y += 100)
        {
            if (HasObjectAtArray(X, Y, EObjectMask::ObstacleMask))
                ObstacleLocations.Add(FVector(X, Y, 0.f));
        }
    }

    if (ObstacleLocations.Num() == 0)
    {
        UE_LOG(ProceduralMapGenerator, Warning, TEXT("No obstacles found to generate fence around."));
        return false;
    }

    // 탐색이 완료되어 완성된 Obstacle 배열에서 랜덤하게 Obstacle 선택
    FVector TargetLocation = ObstacleLocations[FMath::RandRange(0, ObstacleLocations.Num() - 1)];
    
    // 패턴 랜덤 선택 및 생성
    EPatternType RandomPattern = static_cast<EPatternType>(FMath::RandRange(0, 2));
    float RandomRadius = FMath::RandRange(400.f, 1000.f);
    
    TArray<FFenceData> FencePositions;
    GenerateFencePattern(TargetLocation, RandomPattern, RandomRadius, FencePositions);

    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating fence pattern around obstacle at %s"), *TargetLocation.ToString());
    return PlaceFencePattern(FencePositions);
}
#pragma endregion

#pragma region Resources
// 나무, 돌, 철, 우라늄의 자원 생성을 위한 함수
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
        int32 RandomIndex = FMath::RandRange(0, resourceMeshes.Num() - 1);

        // 위치가 유효하지 않다면 근처의 유효한 위치를 재탐색
        if (!CheckLocation(RandomLocation))
            RandomLocation = FindNearestValidLocation(RandomLocation, 500.f, EObjectMask::ResourceMask);

        // 인덱스 유효성 체크
        if(resourceMeshes.IsValidIndex(RandomIndex))
        {
            RandomMesh = resourceMeshes[RandomIndex];

            if (PlaceObject(RandomLocation, RandomMesh))
            {
                PlacedObjects++;
                SetObjectRegion(RandomLocation, RandomMesh, EObjectMask::ResourceMask);
                UE_LOG(ProceduralMapGenerator, Log, TEXT("%s 메쉬를 %s 위치에 생성 중"), RandomMesh ? *RandomMesh->GetName() : TEXT("Unknown"), *RandomLocation.ToString());
            }
        }
        else
            UE_LOG(ProceduralMapGenerator, Error, TEXT("Invalid index selected for resource Meshes"));

        SpawnAttempts++;
    }

    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Resources Completed"));
}
#pragma endregion

#pragma region Decorations
void AProceduralMapGenerator::GenerateDecorations()
{
    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Decos Started"));

    if (decoMeshes.Num() == 0)
    {
        UE_LOG(ProceduralMapGenerator, Error, TEXT("No Static Meshes assigned in obstacleMeshes array!"));
        return;
    }
    
    int32 SpawnAttempts = 0;
    int32 PlacedObjects = 0;

    // 최대 생성 시도 횟수는 배치할 오브젝트의 5배이다.
    while (PlacedObjects < numDecos && SpawnAttempts < numDecos * 5)
    {
        FVector RandomLocation = GetRandomPosition();
        UStaticMesh* RandomMesh = nullptr;
        int32 RandomIndex = FMath::RandRange(0, decoMeshes.Num() - 1);

        // 위치가 유효하지 않다면 근처의 유효한 위치를 재탐색
        if (!CheckLocation(RandomLocation))
            RandomLocation = FindNearestValidLocation(RandomLocation, 500.f, EObjectMask::DecoMask);

        // 인덱스 유효성 체크
        if(decoMeshes.IsValidIndex(RandomIndex))
        {
            RandomMesh = decoMeshes[RandomIndex];

            // 50%의 확률로 군집형 장식물 생성
            if (FMath::RandBool())
            {
                GenerateClusteredDecorations(RandomLocation, clusterRadius, RandomMesh);
                PlacedObjects++;
            }
            else if (PlaceObject(RandomLocation, RandomMesh))
            {
                PlacedObjects++;
                SetObjectRegion(RandomLocation, RandomMesh, EObjectMask::DecoMask);
            }

            UE_LOG(ProceduralMapGenerator, Log, TEXT("%s 메쉬를 %s 위치에 생성 중"), RandomMesh ? *RandomMesh->GetName() : TEXT("Unknown"), *RandomLocation.ToString());
        }
        else
            UE_LOG(ProceduralMapGenerator, Error, TEXT("Invalid index selected for Decorations"));

        SpawnAttempts++;
    }

    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generating Decos Completed"));
}

void AProceduralMapGenerator::GenerateClusteredDecorations(FVector origin, float radius, UStaticMesh* decoMesh)
{
    int32 NumDecos = FMath::RandRange(5, maxClusterSize);
    
    for (int32 i = 0; i < NumDecos; ++i)
    {
        float RandomRadius = FMath::RandRange(0.f, radius);
        float RandomAngle = FMath::RandRange(0.f, 360.f);
        
        FVector Offset(
            RandomRadius * FMath::Cos(FMath::DegreesToRadians(RandomAngle)),
            RandomRadius * FMath::Sin(FMath::DegreesToRadians(RandomAngle)),
            0.f
        );
        
        FVector Location = origin + Offset;
        if (CheckLocation(Location))
        {
            PlaceObject(Location, decoMesh);
            SetObjectRegion(Location, decoMesh, EObjectMask::DecoMask);
        }
    }
}
#pragma endregion

#pragma region Utils&Helpers
// 좌표 배열에 오브젝트를 설정하는 함수
void AProceduralMapGenerator::SetObjectAtArray(int32 X, int32 Y, EObjectMask ObjectType)
{
    int32 Index = GetIndex(X, Y);
    mapCoordinate[Index] |= static_cast<uint8>(ObjectType); // 비트 설정
}

void AProceduralMapGenerator::SetObjectRegion(FVector Location, UStaticMesh* ObjectMesh, EObjectMask ObjectType)
{
    if (!ObjectMesh) return;

    // 바운딩 박스 가져오기
    FBoxSphereBounds Bounds = ObjectMesh->GetBounds();
    FVector Extent = Bounds.BoxExtent; // 바운딩 박스의 반경
    FVector Min = Location - Extent;  // 최소 좌표
    FVector Max = Location + Extent;  // 최대 좌표

    // 맵 좌표계로 변환
    int32 MinX = FMath::FloorToInt(Min.X);
    int32 MinY = FMath::FloorToInt(Min.Y);
    int32 MaxX = FMath::CeilToInt(Max.X);
    int32 MaxY = FMath::CeilToInt(Max.Y);

    if (MinX < -mapHalfSize) MinX = -mapHalfSize;
    if (MinY < -mapHalfSize) MinY = -mapHalfSize;
    if (MaxX >= mapHalfSize) MaxX = mapHalfSize - 1;
    if (MaxY >= mapHalfSize) MaxY = mapHalfSize - 1;

    // 해당 영역의 모든 좌표를 설정
    for (int32 X = MinX; X <= MaxX; ++X)
    {
        for (int32 Y = MinY; Y <= MaxY; ++Y)
        {
            SetObjectAtArray(X, Y, ObjectType);
        }
    }
}

//  좌표 배열에 설정된 오브젝트를 확인하는 함수
bool AProceduralMapGenerator::HasObjectAtArray(int32 X, int32 Y, EObjectMask ObjectType)
{
    int32 Index = GetIndex(X, Y);
    return (mapCoordinate[Index] & static_cast<uint8>(ObjectType)) != 0; // 비트 확인
}

// Map Size 범위 내의 랜덤한 좌표를 FVector로 반환하는 함수
FVector AProceduralMapGenerator::GetRandomPosition()
{
    float X = FMath::RandRange(-mapHalfSize, mapHalfSize);
    float Y = FMath::RandRange(-mapHalfSize, mapHalfSize);
    return FVector(X, Y, 0.f);
}

// 특정 좌표를 기준으로 Offset 이내의 좌표를 FVector로 반환하는 함수
FVector AProceduralMapGenerator::GetRandomOffsetPosition(FVector origin, float offset)
{
    float X = FMath::RandRange(origin.X - offset, origin.X + offset);
    float Y = FMath::RandRange(origin.Y - offset, origin.Y + offset);
    return FVector(X, Y, 0.f);
}

/// 해당 위치가 유효한지 검사하는 함수 (충돌 방지)
/// 충돌 검사에서 좌표 검사로 수정할 예정
bool AProceduralMapGenerator::CheckLocation(FVector Location)
{
    // 좌표를 맵 배열의 인덱스로 변환
    int32 X = FMath::RoundToInt(Location.X);
    int32 Y = FMath::RoundToInt(Location.Y);

    // 1. 맵 범위를 벗어나는지 확인
    if (X < -mapHalfSize || X >= mapHalfSize || Y < -mapHalfSize || Y >= mapHalfSize)
    {
        UE_LOG(ProceduralMapGenerator, Warning, TEXT("Location (%d, %d) is out of bounds."), X, Y);
        return false;
    }

    // 2. 해당 좌표에 이미 오브젝트가 있는지 확인
    if (HasObjectAtArray(X, Y, EObjectMask::ObstacleMask) ||
        HasObjectAtArray(X, Y, EObjectMask::SubObstacleMask) ||
        HasObjectAtArray(X, Y, EObjectMask::FenceMask) ||
        HasObjectAtArray(X, Y, EObjectMask::ResourceMask) ||
        HasObjectAtArray(X, Y, EObjectMask::DecoMask))
    {
        UE_LOG(ProceduralMapGenerator, Warning, TEXT("Location (%d, %d) is already occupied."), X, Y);
        return false;
    }

    // 위치가 유효함
    return true;
}

FVector AProceduralMapGenerator::FindNearestValidLocation(FVector Origin, float SearchRadius, EObjectMask ObjectType)
{
    const int32 NumDirections = 8;
    const float AngleStep = 360.0f / NumDirections;
    
    for (float CurrentRadius = 100.f; CurrentRadius <= SearchRadius; CurrentRadius += 100.f)
    {
        for (int32 i = 0; i < NumDirections; ++i)
        {
            float Angle = i * AngleStep;
            FVector Offset(
                CurrentRadius * FMath::Cos(FMath::DegreesToRadians(Angle)),
                CurrentRadius * FMath::Sin(FMath::DegreesToRadians(Angle)),
                0.0f
            );
            
            FVector TestLocation = Origin + Offset;
            if (CheckLocation(TestLocation))
                return TestLocation;
        }
    }
    
    return Origin;
}

// 오브젝트를 원하는 위치에 설정하는 함수
bool AProceduralMapGenerator::PlaceObject(FVector Location, UStaticMesh* ObjectMesh)
{
    if (!ObjectMesh)
    {
        UE_LOG(ProceduralMapGenerator, Error, TEXT("ObjectMesh is nullptr!"));
        return false;
    }

    // StaticMeshActor 생성
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    AStaticMeshActor* NewActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, FRotator::ZeroRotator, SpawnParams);

    if (NewActor)
    {
        NewActor->GetStaticMeshComponent()->SetStaticMesh(ObjectMesh);
        NewActor->SetActorLocation(Location);
        NewActor->SetMobility(EComponentMobility::Static);

        UE_LOG(ProceduralMapGenerator, Log, TEXT("Placed StaticMeshActor at %s"), *Location.ToString());
        return true;
    }

    return false;
}

#pragma endregion