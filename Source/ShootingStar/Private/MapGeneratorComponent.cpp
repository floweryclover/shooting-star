// Copyright 2025 ShootingStar. All Rights Reserved.

#include "MapGeneratorComponent.h"
#include "ObstacleGenerator.h"
#include "SubObstacleGenerator.h"
#include "FenceGenerator.h"
#include "ResourceGenerator.h"
#include "DecorationGenerator.h"
#include "MapObjectActor.h"
#include "CompetitiveGameMode.h"
#include "SafeZoneActor.h"

DEFINE_LOG_CATEGORY(MapGenerator);

UMapGeneratorComponent::UMapGeneratorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Generator 객체들을 생성자에서 초기화
    obstacleGenerator = CreateDefaultSubobject<UObstacleGenerator>(TEXT("ObstacleGenerator"));
    subObstacleGenerator = CreateDefaultSubobject<USubObstacleGenerator>(TEXT("SubObstacleGenerator"));
    fenceGenerator = CreateDefaultSubobject<UFenceGenerator>(TEXT("FenceGenerator"));
    resourceGenerator = CreateDefaultSubobject<UResourceGenerator>(TEXT("ResourceGenerator")); 
    decorationGenerator = CreateDefaultSubobject<UDecorationGenerator>(TEXT("DecorationGenerator"));
}

void UMapGeneratorComponent::Initialize()
{
    // 1. Map Coordinate 초기화
    InitializeMapCoordinate(mapHalfSize * 2);

    // 2. Map Instanced Mesh Actor 생성
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    MapInstancedMeshActor = GetWorld()->SpawnActor<AMapInstancedMeshActor>(Params);
    if (!IsValid(MapInstancedMeshActor))
    {
        UE_LOG(MapGenerator, Error, TEXT("Failed to spawn MapInstancedMeshActor"));
        return;
    }
    else
    {
        MapInstancedMeshActor->SetReplicates(true);
        MapInstancedMeshActor->Initialize(this);
    }

    // 3. Generators 초기화
    if (IsValid(obstacleGenerator)) obstacleGenerator->Initialize(this);
    if (IsValid(subObstacleGenerator)) subObstacleGenerator->Initialize(this);
    if (IsValid(fenceGenerator)) fenceGenerator->Initialize(this);
    if (IsValid(resourceGenerator)) resourceGenerator->Initialize(this);
    if (IsValid(decorationGenerator)) decorationGenerator->Initialize(this);

    // // 4. Static Actors 등록
    // RegisterMapActors();

    // 5. 절차적 생성 시작
    GenerateMap();
    
    // 6. 스폰 포인트 초기화
    InitializeSpawnPoints();

    UE_LOG(MapGenerator, Log, TEXT("Map Generator initialized successfully"));
}

void UMapGeneratorComponent::InitializeMapCoordinate(int32 GridSize)
{
    mapCoordinate.SetNum(GridSize * GridSize);
}

void UMapGeneratorComponent::GenerateMap()
{
    UE_LOG(MapGenerator, Log, TEXT("Generate Map Started"));

    obstacleGenerator->GenerateObjects();
    subObstacleGenerator->GenerateObjects();
    fenceGenerator->GenerateObjects();
    resourceGenerator->GenerateObjects();
    decorationGenerator->GenerateObjects();

    UE_LOG(MapGenerator, Log, TEXT("Generate Map Completed"));
}

// 좌표 배열에 오브젝트를 설정하는 함수
void UMapGeneratorComponent::SetObjectAtArray(int32 X, int32 Y, EObjectMask ObjectType)
{
    int32 Index = GetIndex(X, Y);
    mapCoordinate[Index] |= static_cast<uint8>(ObjectType); // 비트 설정
}

void UMapGeneratorComponent::ClearObjectTypeFromMap(EObjectMask ObjectType)
{
    uint8 mask = ~static_cast<uint8>(ObjectType); // 제거할 비트만 0으로 만드는 마스크
    for (uint8& coordinate : mapCoordinate)
        coordinate &= mask; // 해당 비트만 클리어
}

void UMapGeneratorComponent::SetObjectRegion(FVector Location, UStaticMesh* ObjectMesh, EObjectMask ObjectType)
{
    if (!ObjectMesh) return;

    // 바운딩 박스 가져오기
    FBoxSphereBounds Bounds = ObjectMesh->GetBounds();
    FVector Extent = Bounds.BoxExtent + FVector(100.f, 100.f, 0.f); // 바운딩 박스의 반경, 실제 크기보다 약간 크게 설정
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
            SetObjectAtArray(X, Y, ObjectType);
    }
}

//  좌표 배열에 설정된 오브젝트를 확인하는 함수
bool UMapGeneratorComponent::HasObjectAtArray(int32 X, int32 Y, EObjectMask ObjectType)
{
    int32 Index = GetIndex(X, Y);
    return (mapCoordinate[Index] & static_cast<uint8>(ObjectType)) != 0; // 비트 확인
}

// Map Size 범위 내의 랜덤한 좌표를 FVector로 반환하는 함수
FVector UMapGeneratorComponent::GetRandomPosition()
{
    float X = FMath::RandRange(-mapHalfSize, mapHalfSize);
    float Y = FMath::RandRange(-mapHalfSize, mapHalfSize);
    return FVector(X, Y, 0.f);
}

// 특정 좌표를 기준으로 Offset 이내의 좌표를 FVector로 반환하는 함수
FVector UMapGeneratorComponent::GetRandomOffsetPosition(FVector origin, float offset)
{
    float X = FMath::RandRange(origin.X - offset, origin.X + offset);
    float Y = FMath::RandRange(origin.Y - offset, origin.Y + offset);
    return FVector(X, Y, 0.f);
}

// 해당 위치가 유효한지 검사하는 함수 (충돌 방지)
bool UMapGeneratorComponent::CheckLocation(FVector Location)
{
    // 좌표를 맵 배열의 인덱스로 변환
    int32 X = FMath::RoundToInt(Location.X);
    int32 Y = FMath::RoundToInt(Location.Y);

    // 1. 맵 범위를 벗어나는지 확인
    if (X < -mapHalfSize || X >= mapHalfSize || Y < -mapHalfSize || Y >= mapHalfSize)
    {
        UE_LOG(MapGenerator, Warning, TEXT("Location (%d, %d) is out of bounds."), X, Y);
        return false;
    }

    // 2. 해당 좌표에 이미 오브젝트가 있는지 확인
    if (HasObjectAtArray(X, Y, EObjectMask::ObstacleMask) ||
        HasObjectAtArray(X, Y, EObjectMask::SubObstacleMask) ||
        HasObjectAtArray(X, Y, EObjectMask::FenceMask) ||
        HasObjectAtArray(X, Y, EObjectMask::ResourceMask) ||
        HasObjectAtArray(X, Y, EObjectMask::DecoMask))
    {
        UE_LOG(MapGenerator, Warning, TEXT("Location (%d, %d) is already occupied."), X, Y);
        return false;
    }

    // 위치가 유효함
    return true;
}

FVector UMapGeneratorComponent::FindNearestValidLocation(FVector Origin, float SearchRadius, EObjectMask ObjectType)
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
    
    return FVector::ZeroVector;
}

// 오브젝트를 원하는 위치에 설정하는 함수
bool UMapGeneratorComponent::PlaceObject(FVector Location, UStaticMesh* ObjectMesh)
{
    if (!ObjectMesh || !GetOwner() || !GetWorld())
    {
        UE_LOG(MapGenerator, Error, TEXT("Invalid ObjectMesh or Owner!"));
        return false;
    }

    // StaticMeshActor 생성
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = GetOwner();
    AMapObjectActor* NewActor = GetWorld()->SpawnActor<AMapObjectActor>(AMapObjectActor::StaticClass(), Location, FRotator::ZeroRotator, SpawnParams);
    
    if (NewActor)
    {
        NewActor->SetReplicates(true);
        UStaticMeshComponent* MeshComp = NewActor->GetStaticMeshComponent();
        MeshComp->SetIsReplicated(true);
        MeshComp->SetMobility(EComponentMobility::Movable);
        MeshComp->SetStaticMesh(ObjectMesh);

        MeshComp->SetCanEverAffectNavigation(false);
        // 메쉬 이름이 "SM_tumbleweed_001"인 경우 Player와 Overlap되도록 설정
        FString MeshName = ObjectMesh->GetName();
        if (MeshName.Equals(TEXT("SM_tumbleweed_001")))
        {
            NewActor->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
        }

        NewActor->SetActorLocation(Location);

        UE_LOG(MapGenerator, Log, TEXT("Placed AMapObjectActor at %s"), *Location.ToString());
        return true;
    }

    return false;
}

#pragma region Player Spawn
void UMapGeneratorComponent::InitializeSpawnPoints()
{
    PlayerSpawnPoints.Empty();

    // 각 사분면별로 스폰 포인트 생성
    for (int32 Quadrant = 0; Quadrant < 4; Quadrant++)
    {
        for (int32 i = 0; i < NumSpawnPointsPerQuadrant; i++)
        {
            float MinX = (Quadrant == 1 || Quadrant == 2) ? 0 : -mapHalfSize + BorderMargin;
            float MaxX = (Quadrant == 1 || Quadrant == 2) ? mapHalfSize - BorderMargin : 0;
            float MinY = (Quadrant == 2 || Quadrant == 3) ? 0 : -mapHalfSize + BorderMargin;
            float MaxY = (Quadrant == 2 || Quadrant == 3) ? mapHalfSize - BorderMargin : 0;

            FVector SpawnLocation;
            int32 MaxAttempts = 100;
            bool bFoundValidLocation = false;

            // 유효한 스폰 위치를 찾을 때까지 시도
            while (MaxAttempts > 0 && !bFoundValidLocation)
            {
                SpawnLocation = FVector(
                    FMath::RandRange(MinX, MaxX),
                    FMath::RandRange(MinY, MaxY),
                    0.f
                );

                if (IsValidSpawnLocation(SpawnLocation))
                {
                    PlayerSpawnPoints.Add(SpawnLocation);
                    bFoundValidLocation = true;
                }

                MaxAttempts--;
            }
        }
    }

    UE_LOG(MapGenerator, Log, TEXT("Initialized %d spawn points"), PlayerSpawnPoints.Num());
}

bool UMapGeneratorComponent::IsValidSpawnLocation(const FVector& Location)
{
    // 이미 존재하는 스폰 포인트들과의 거리 체크
    for (const FVector& ExistingSpot : PlayerSpawnPoints)
    {
        if (FVector::Dist(Location, ExistingSpot) < MinSpawnPointDistance)
            return false;
    }

    // 해당 위치에 다른 오브젝트가 있는지 체크
    return CheckLocation(Location);
}

FVector UMapGeneratorComponent::GetRandomSpawnLocation()
{
    if (PlayerSpawnPoints.Num() == 0)
    {
        UE_LOG(MapGenerator, Warning, TEXT("No spawn points available!"));
        return FVector::ZeroVector;
    }

    // 사용 가능한 스폰 포인트 중에서 랜덤하게 선택
    const int32 RandomIndex = FMath::RandRange(0, PlayerSpawnPoints.Num() - 1);
    return PlayerSpawnPoints[RandomIndex];
}
#pragma endregion

#pragma region Resource Spawn
FVector UMapGeneratorComponent::GetSupplySpawnLocation()
{
    FVector DropLocation = GetRandomSupplySpawnLocation();

    while (!CheckLocation(DropLocation))
    {
        DropLocation = FindNearestValidLocation(DropLocation, 1500.f, EObjectMask::ResourceMask);
        
        if (DropLocation == FVector::ZeroVector)
            DropLocation = GetRandomSupplySpawnLocation();
        else
            break;
    }

    SetObjectAtArray(
        FMath::FloorToInt(DropLocation.X),
        FMath::FloorToInt(DropLocation.Y),
        EObjectMask::ResourceMask
    );

    return DropLocation;
}

FVector UMapGeneratorComponent::GetRandomSupplySpawnLocation()
{
    // 맵 중앙 기준으로 현재 자기장 반경 내 랜덤 위치 선정
    const float RandomAngle = FMath::RandRange(0.f, 360.f);
    const float CurrentRadius = Cast<ACompetitiveGameMode>(GetOwner())->GetSafeZone()->GetRadius() * 50.f; // scale 고려하여 곱셈
    const float RandomRadius = FMath::RandRange(0.f, CurrentRadius * 0.8f);  // 자기장 80% 이내 위치에 생성
    FVector DropLocation(
        RandomRadius * FMath::Cos(RandomAngle),
        RandomRadius * FMath::Sin(RandomAngle),
        0.f
    );

    return DropLocation;
}
#pragma endregion

void UMapGeneratorComponent::RegenerateResources()
{
    ClearObjectTypeFromMap(EObjectMask::ResourceMask);

    if (IsValid(resourceGenerator))
        resourceGenerator->GenerateObjects();
}