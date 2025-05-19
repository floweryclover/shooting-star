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
#include "TumbleWeed.h"

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
    const double StartTime = FPlatformTime::Seconds();
    
    UE_LOG(MapGenerator, Log, TEXT("Generate Map Started"));

    obstacleGenerator->GenerateObjects();
    subObstacleGenerator->GenerateObjects();
    fenceGenerator->GenerateObjects();
    resourceGenerator->GenerateObjects();
    decorationGenerator->GenerateObjects();

    const double EndTime = FPlatformTime::Seconds();
    UE_LOG(MapGenerator, Log, TEXT("Map Generation completed in %.2f seconds"), EndTime - StartTime);

    UE_LOG(MapGenerator, Log, TEXT("Generate Map Completed"));
}

// 좌표 배열에 오브젝트를 설정하는 함수
void UMapGeneratorComponent::SetObjectAtArray(int32 X, int32 Y, EObjectMask ObjectType)
{
    int32 Index = GetIndex(X, Y);
    mapCoordinate[Index] |= static_cast<uint8>(ObjectType); // 비트 설정
}

void UMapGeneratorComponent::SetObjectRegion(const FVector& Location, const UStaticMesh* ObjectMesh, EObjectMask ObjectType)
{
    if (!ObjectMesh) return;

    const FBoxSphereBounds& Bounds = ObjectMesh->GetBounds();

    // 오브젝트 타입별로 다른 여유 공간을 static 배열로 캐싱
    static const float SafetyMargins[] = {
        300.f,              // ObstacleMask
        200.f,              // SubObstacleMask
        100.f,              // FenceMask
        100.f,              // ResourceMask
        50.f               // Default/DecoMask
    };

    const float SafetyMargin = SafetyMargins[static_cast<int>(ObjectType) - 1];
    const FVector Extent = Bounds.BoxExtent + FVector(SafetyMargin, SafetyMargin, 0.f);
    
    // 범위 계산을 한 번만 수행
    const int32 MinX = FMath::Max(FMath::FloorToInt(Location.X - Extent.X), -mapHalfSize);
    const int32 MinY = FMath::Max(FMath::FloorToInt(Location.Y - Extent.Y), -mapHalfSize);
    const int32 MaxX = FMath::Min(FMath::CeilToInt(Location.X + Extent.X), mapHalfSize - 1);
    const int32 MaxY = FMath::Min(FMath::CeilToInt(Location.Y + Extent.Y), mapHalfSize - 1);

    // 엄격한 검사를 위해 10으로 설정
    const int32 GridStep = 10;

    const uint8 ObjectMask = static_cast<uint8>(ObjectType);
    for (int32 X = MinX; X <= MaxX; X += GridStep)
    {
        for (int32 Y = MinY; Y <= MaxY; Y += GridStep)
            mapCoordinate[GetIndex(X, Y)] |= ObjectMask;
    }
}

void UMapGeneratorComponent::ClearObjectTypeFromMap(EObjectMask ObjectType)
{
    // 제거할 비트만 0으로 만드는 마스크
    uint8 mask = ~static_cast<uint8>(ObjectType);

    for (uint8& coordinate : mapCoordinate)
        coordinate &= mask; // 해당 비트만 클리어
}

//  좌표 배열에 설정된 오브젝트를 확인하는 함수
bool UMapGeneratorComponent::HasObjectAtArray(int32 X, int32 Y, EObjectMask ObjectType) const
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
FVector UMapGeneratorComponent::GetRandomOffsetPosition(const FVector& Origin, float Offset) const
{
    const float X = FMath::RandRange(Origin.X - Offset, Origin.X + Offset);
    const float Y = FMath::RandRange(Origin.Y - Offset, Origin.Y + Offset);
    return FVector(X, Y, 0.f);
}

// 위치 유효성 검사 함수
bool UMapGeneratorComponent::CheckLocation(const FVector& Location) const
{
    const int32 X = FMath::RoundToInt(Location.X);
    const int32 Y = FMath::RoundToInt(Location.Y);

    // 1. 맵 범위를 벗어나는지 확인
    if (!IsInMap(Location))
        return false;

    // 실제 검사 수행
    for (uint8 i = 0; i < static_cast<uint8>(EObjectMask::End); ++i)
    {
        const EObjectMask ObjectType = static_cast<EObjectMask>(1 << i);
        if (HasObjectAtArray(X, Y, ObjectType))
            return false;
    }
    
    return true;
}

bool UMapGeneratorComponent::CheckLocation(const FVector& Location, const UStaticMesh* ObjectMesh, EObjectMask ObjectType) const
{
    if (!ObjectMesh || !IsInMap(Location)) return false;

    float checkMargin;
    switch (ObjectType)
    {
    case EObjectMask::ObstacleMask: checkMargin = 300.f; break;
    case EObjectMask::SubObstacleMask: checkMargin = 200.f; break;
    case EObjectMask::FenceMask: checkMargin = fenceMinDistance * 0.33f; break;
    case EObjectMask::ResourceMask: checkMargin = 100.f; break;
    default: checkMargin = 50.f; break;
    }

    const FBoxSphereBounds Bounds = ObjectMesh->GetBounds();
    const FVector Extent = Bounds.BoxExtent + FVector(checkMargin, checkMargin, 0.f);

    // 검사 범위 계산
    int32 MinX = FMath::Max(FMath::FloorToInt(Location.X - Extent.X), -mapHalfSize);
    int32 MinY = FMath::Max(FMath::FloorToInt(Location.Y - Extent.Y), -mapHalfSize);
    int32 MaxX = FMath::Min(FMath::CeilToInt(Location.X + Extent.X), mapHalfSize - 1);
    int32 MaxY = FMath::Min(FMath::CeilToInt(Location.Y + Extent.Y), mapHalfSize - 1);

    // 최적화된 그리드 간격 (더 큰 간격으로 조정 가능)
    const int32 GridStep = FMath::Max(20, FMath::FloorToInt(checkMargin * 0.1f));

    // 모든 오브젝트 타입을 한 번에 검사하는 마스크
    const uint8 CheckMask = static_cast<uint8>(EObjectMask::ObstacleMask) |
                           static_cast<uint8>(EObjectMask::SubObstacleMask) |
                           static_cast<uint8>(EObjectMask::FenceMask) |
                           static_cast<uint8>(EObjectMask::ResourceMask) |
                           static_cast<uint8>(EObjectMask::DecoMask);

    for (int32 X = MinX; X <= MaxX; X += GridStep)
    {
        for (int32 Y = MinY; Y <= MaxY; Y += GridStep)
        {
            const int32 Index = GetIndex(X, Y);
            if (mapCoordinate[Index] & CheckMask)
                return false;
        }
    }

    return true;
}

FVector UMapGeneratorComponent::FindNearestValidLocation(const FVector& Origin, float SearchRadius, const UStaticMesh* ObjectMesh, EObjectMask ObjectType) const
{
    static const int32 NumDirections = 8;
    static const float AnglePerStep = 360.0f / NumDirections;
    static const float RadiusPerStep = 100.f;
    
    // 방향 벡터를 미리 계산
    static TArray<FVector2D> DirectionVectors;
    if (DirectionVectors.Num() == 0)
    {
        DirectionVectors.SetNum(NumDirections);
        for (int32 i = 0; i < NumDirections; ++i)
        {
            float Angle = i * AnglePerStep;
            float Rad = FMath::DegreesToRadians(Angle);
            DirectionVectors[i] = FVector2D(FMath::Cos(Rad), FMath::Sin(Rad));
        }
    }

    // 1m부터 SearchRadius까지 점진적으로 검사
    for (float CurrentRadius = RadiusPerStep; CurrentRadius <= SearchRadius; CurrentRadius += RadiusPerStep)
    {
        // 8방향 검사
        for (int32 i = 0; i < NumDirections; ++i)
        {
            const FVector TestLocation(
                Origin.X + CurrentRadius * DirectionVectors[i].X,
                Origin.Y + CurrentRadius * DirectionVectors[i].Y,
                0.0f
            );
            
            if (IsInMap(TestLocation) && CheckLocation(TestLocation, ObjectMesh, ObjectType))
                return TestLocation;
        }
    }
    
    return FVector::ZeroVector;
}

// 오브젝트를 원하는 위치에 설정하는 함수
bool UMapGeneratorComponent::PlaceObject(const FVector& Location, const UStaticMesh* ObjectMesh)
{
    if (!ObjectMesh || !GetOwner() || !GetWorld())
    {
        UE_LOG(MapGenerator, Error, TEXT("Invalid ObjectMesh or Owner!"));
        return false;
    }

    // StaticMeshActor 생성
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = GetOwner();
    FString MeshName = ObjectMesh->GetName();
    AMapObjectActor* NewActor = GetWorld()->SpawnActor<AMapObjectActor>(
        MeshName.Equals(TEXT("SM_tumbleweed_001")) ? ATumbleWeed::StaticClass() : AMapObjectActor::StaticClass(),
        Location,
        FRotator::ZeroRotator,
        SpawnParams);
    
    if (IsValid(NewActor))
    {
        NewActor->SetReplicates(true);
        UStaticMeshComponent* MeshComp = NewActor->GetStaticMeshComponent();
        MeshComp->SetIsReplicated(true);
        MeshComp->SetMobility(EComponentMobility::Movable);
        MeshComp->SetStaticMesh(const_cast<UStaticMesh*>(ObjectMesh));

        MeshComp->SetCanEverAffectNavigation(false);
        // 메쉬 이름이 "SM_tumbleweed_001"인 경우 Player와 Overlap되도록 설정
        if (MeshName.Equals(TEXT("SM_tumbleweed_001")))
        {
            NewActor->OnActorBeginOverlap.AddDynamic(this, &UMapGeneratorComponent::OnActorBeginOverlapOnTumbleWeedHandler);
            NewActor->OnActorEndOverlap.AddDynamic(this, &UMapGeneratorComponent::OnActorEndOverlapOnTumbleWeedHandler);
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

bool UMapGeneratorComponent::IsValidSpawnLocation(const FVector& Location) const
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
        DropLocation = FindNearestValidLocation(DropLocation, 1500.f, GetSupplyMesh(), EObjectMask::ResourceMask);
        
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
    const float RandomRadius = FMath::Min(FMath::RandRange(0.f, CurrentRadius * 0.8f), mapHalfSize);  // 자기장 80% 이내 위치에 생성
    FVector DropLocation(
        RandomRadius * FMath::Cos(RandomAngle),
        RandomRadius * FMath::Sin(RandomAngle),
        0.f
    );

    return DropLocation;
}

void UMapGeneratorComponent::OnActorBeginOverlapOnTumbleWeedHandler(AActor* OverlappedActor, AActor* OtherActor)
{
    OnActorBeginOverlapOnTumbleWeed.Broadcast(OverlappedActor, OtherActor);
}

void UMapGeneratorComponent::OnActorEndOverlapOnTumbleWeedHandler(AActor* OverlappedActor, AActor* OtherActor)
{
    OnActorEndOverlapOnTumbleWeed.Broadcast(OverlappedActor, OtherActor);
}

void UMapGeneratorComponent::RegenerateResources()
{
    ClearObjectTypeFromMap(EObjectMask::ResourceMask);

    if (IsValid(resourceGenerator))
        resourceGenerator->GenerateObjects();
}
#pragma endregion