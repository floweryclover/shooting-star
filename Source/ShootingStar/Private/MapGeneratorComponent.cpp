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
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/BoxElem.h"

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

    // 4. 맵에 미리 배치된 오브젝트 좌표 등록
    for (int32 x = -190; x <= 190; x++) // Windmill
    {
        for (int32 y = -190; y <= 190; y++)
            SetObjectAtArray(x, y, EObjectMask::ObstacleMask);
    }

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

FVector UMapGeneratorComponent::CalculateExtent(const FVector& Location, const UStaticMesh* ObjectMesh, EObjectMask ObjectType) const
{
    if (!ObjectMesh) return FVector::ZeroVector;
    
    FVector Extent;
    UBodySetup* BodySetup = ObjectMesh->GetBodySetup();

    // 실제 충돌 박스 크기 가져오기
    const FKAggregateGeom& AggGeom = ObjectMesh->GetBodySetup()->AggGeom;
    if (BodySetup && BodySetup->AggGeom.BoxElems.Num() > 0)
    {
        // 콜리전 박스가 있는 경우
        const FKBoxElem& BoxElem = AggGeom.BoxElems[0];
        Extent = FVector(BoxElem.X * 0.5f, BoxElem.Y * 0.5f, BoxElem.Z * 0.5f);

        UE_LOG(MapGenerator, Log, TEXT("충돌 박스 로드 성공"));
    }
    else
    {
        UE_LOG(MapGenerator, Log, TEXT("충돌 박스 로드 실패"));
        const FBoxSphereBounds& Bounds = ObjectMesh->GetBounds();
        Extent = Bounds.BoxExtent;
    }

    int32 i;
    uint8 mask = static_cast<uint8>(ObjectType);
        // 최하위 1비트의 위치를 찾아 반환
    for(i = 0; i < 5; ++i)
    {
        if(mask & (1 << i))
            break;
    }

    static const float SafetyMargins[] = {
        200.f,        // ObstacleMask
        100.f,        // SubObstacleMask
        -5.f,         // FenceMask
        50.f,         // ResourceMask
        10.f          // Default/DecoMask
    };
    const float SafetyMargin = SafetyMargins[i];
    Extent += FVector(SafetyMargin, SafetyMargin, 0.f);

    // 펜스인 경우 회전에 따른 Extent 조정
    if (ObjectType == EObjectMask::FenceMask)
    {
        UE_LOG(MapGenerator, Log, TEXT("Fence Margin: %f"), SafetyMargin);
        UE_LOG(MapGenerator, Log, TEXT("Fence Extent: %s"), *Extent.ToString());
        bool bIsHorizontal = false;
        
        // PlaceFence에서 전달된 위치 정보로 회전 상태 판단
        for (const auto& Position : CurrentFencePositions)
        {
            if (Position.Location.Equals(Location))
            {
                bIsHorizontal = FMath::IsNearlyEqual(Position.Rotation.Yaw, 90.0f);
                break;
            }
        }

        if (bIsHorizontal)
        {
            // 가로 방향일 경우 X, Y 값을 교체
            const float TempX = Extent.X;
            Extent.X = Extent.Y;
            Extent.Y = TempX;

            UE_LOG(MapGenerator, Log, TEXT("Switched Fence Extent: %s"), *Extent.ToString());
        }
    }

    return Extent;
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

    FVector Extent = CalculateExtent(Location, ObjectMesh, ObjectType);
    
    // 범위 계산
    const int32 MinX = FMath::Max(FMath::FloorToInt(Location.X - Extent.X), -mapHalfSize);
    const int32 MinY = FMath::Max(FMath::FloorToInt(Location.Y - Extent.Y), -mapHalfSize);
    const int32 MaxX = FMath::Min(FMath::CeilToInt(Location.X + Extent.X), mapHalfSize - 1);
    const int32 MaxY = FMath::Min(FMath::CeilToInt(Location.Y + Extent.Y), mapHalfSize - 1);

    const uint8 ObjectMask = static_cast<uint8>(ObjectType);
    for (int32 X = MinX; X <= MaxX; X++)
    {
        for (int32 Y = MinY; Y <= MaxY; Y++)
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

    // 맵 범위를 벗어나는지 확인
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

    FVector Extent = CalculateExtent(Location, ObjectMesh, ObjectType);
    if (ObjectType == EObjectMask::FenceMask)
    {
        // 플레이어 끼임 방지 오프셋 조정
        if (Extent.X > Extent.Y)
            Extent.Y += 100.f;
        else
            Extent.X += 100.f;
    }

    // 검사 범위 계산
    int32 MinX = FMath::Max(FMath::FloorToInt(Location.X - Extent.X), -mapHalfSize);
    int32 MinY = FMath::Max(FMath::FloorToInt(Location.Y - Extent.Y), -mapHalfSize);
    int32 MaxX = FMath::Min(FMath::CeilToInt(Location.X + Extent.X), mapHalfSize - 1);
    int32 MaxY = FMath::Min(FMath::CeilToInt(Location.Y + Extent.Y), mapHalfSize - 1);

    // 모든 오브젝트 타입을 한 번에 검사하는 마스크
    const uint8 CheckMask = static_cast<uint8>(EObjectMask::ObstacleMask) |
                           static_cast<uint8>(EObjectMask::SubObstacleMask) |
                           static_cast<uint8>(EObjectMask::FenceMask) |
                           static_cast<uint8>(EObjectMask::ResourceMask);

    for (int32 X = MinX; X <= MaxX; X++)
    {
        for (int32 Y = MinY; Y <= MaxY; Y++)
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
    static const int32 NumDirections = 16;
    static const float AnglePerStep = 360.0f / (float)NumDirections;
    static const float RadiusPerStep = 50.f;
    
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
        else
        {
            NewActor->SetTranslucentMaterial(TranslucentMaterial);
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