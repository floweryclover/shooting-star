// Copyright 2025 ShootingStar. All Rights Reserved.

#include "ProceduralMapGenerator.h"
#include "ObstacleGenerator.h"
#include "SubObstacleGenerator.h"
#include "FenceGenerator.h"
#include "ResourceGenerator.h"
#include "DecorationGenerator.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h" // Include for UInstancedStaticMeshComponent
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMeshActor.h" // Include for AStaticMeshActor


DEFINE_LOG_CATEGORY(ProceduralMapGenerator);

#pragma region 생성자 및 초기화
UProceduralMapGenerator::UProceduralMapGenerator()
{
	PrimaryComponentTick.bCanEverTick = false;

    // Generator 객체 생성
    obstacleGenerator = CreateDefaultSubobject<UObstacleGenerator>(TEXT("ObstacleGenerator"));
    subObstacleGenerator = CreateDefaultSubobject<USubObstacleGenerator>(TEXT("SubObstacleGenerator"));
    fenceGenerator = CreateDefaultSubobject<UFenceGenerator>(TEXT("FenceGenerator"));
    resourceGenerator = CreateDefaultSubobject<UResourceGenerator>(TEXT("ResourceGenerator"));
    decorationGenerator = CreateDefaultSubobject<UDecorationGenerator>(TEXT("DecorationGenerator"));
    
    // Fence InstancedMeshComponent 생성
    FenceInstancedMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FenceInstancedMesh"));
}

void UProceduralMapGenerator::BeginPlay()
{
    Super::BeginPlay();

    // Owner 존재 여부 확인
    AActor* Owner = GetOwner();
    if (!Owner)
    {
        UE_LOG(ProceduralMapGenerator, Error, TEXT("ProceduralMapGenerator: Owner is not valid!"));
        return;
    }

    // 기존 DecorationInstancedMeshComponents 초기화
    DecorationInstancedMeshComponents.Empty();

    // Decoration 메시 종류별로 새로운 컴포넌트 생성
    for (int32 i = 0; i < decoMeshes.Num(); ++i)
    {
        FString CompName = FString::Printf(TEXT("DecorationInstancedMesh_%d"), i);
        UInstancedStaticMeshComponent* NewComp = NewObject<UInstancedStaticMeshComponent>(Owner, *CompName);
        if (NewComp)
        {
            NewComp->RegisterComponent();
            NewComp->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
            NewComp->SetStaticMesh(decoMeshes[i]);
            DecorationInstancedMeshComponents.Add(NewComp);
            UE_LOG(ProceduralMapGenerator, Log, TEXT("Created DecorationInstancedMeshComponent %d with mesh %s"), 
                i, *decoMeshes[i]->GetName());
        }
    }

    // Fence InstancedMeshComponent 초기화 및 설정
    if (FenceInstancedMeshComponent)
    {
        FenceInstancedMeshComponent->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
        if (fenceMesh)
            FenceInstancedMeshComponent->SetStaticMesh(fenceMesh);
        else
            UE_LOG(ProceduralMapGenerator, Error, TEXT("ProceduralMapGenerator: Fence mesh is not set!"));
        FenceInstancedMeshComponent->RegisterComponent();

        UE_LOG(ProceduralMapGenerator, Log, TEXT("FenceInstancedMeshComponent initialized and registered."));
    }

    // Generator 초기화를 맨 마지막에 수행
    obstacleGenerator->Initialize(this);
    subObstacleGenerator->Initialize(this);
    fenceGenerator->Initialize(this);
    resourceGenerator->Initialize(this);
    decorationGenerator->Initialize(this);

    // Generator 초기화 완료 후 맵 생성 시작
    InitializeMapCoordinate(mapHalfSize * 2);
    GenerateMap();
}

void UProceduralMapGenerator::InitializeMapCoordinate(int32 GridSize)
{
    mapCoordinate.SetNum(GridSize * GridSize); // 1차원 배열로 할당
}
#pragma endregion

#pragma region 절차적 맵 생성
// 랜덤 포지션과 랜덤 오브젝트를 선정해 맵을 생성하는 함수
void UProceduralMapGenerator::GenerateMap()
{
    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generate Map Started"));

    obstacleGenerator->GenerateObjects();
    subObstacleGenerator->GenerateObjects();
    fenceGenerator->GenerateObjects();
    resourceGenerator->GenerateObjects();
    decorationGenerator->GenerateObjects();

    UE_LOG(ProceduralMapGenerator, Log, TEXT("Generate Map Completed"));
}
#pragma endregion

#pragma region Utils&Helpers
// 좌표 배열에 오브젝트를 설정하는 함수
void UProceduralMapGenerator::SetObjectAtArray(int32 X, int32 Y, EObjectMask ObjectType)
{
    int32 Index = GetIndex(X, Y);
    mapCoordinate[Index] |= static_cast<uint8>(ObjectType); // 비트 설정
}

void UProceduralMapGenerator::SetObjectRegion(FVector Location, UStaticMesh* ObjectMesh, EObjectMask ObjectType)
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
bool UProceduralMapGenerator::HasObjectAtArray(int32 X, int32 Y, EObjectMask ObjectType)
{
    int32 Index = GetIndex(X, Y);
    return (mapCoordinate[Index] & static_cast<uint8>(ObjectType)) != 0; // 비트 확인
}

// Map Size 범위 내의 랜덤한 좌표를 FVector로 반환하는 함수
FVector UProceduralMapGenerator::GetRandomPosition()
{
    float X = FMath::RandRange(-mapHalfSize, mapHalfSize);
    float Y = FMath::RandRange(-mapHalfSize, mapHalfSize);
    return FVector(X, Y, 0.f);
}

// 특정 좌표를 기준으로 Offset 이내의 좌표를 FVector로 반환하는 함수
FVector UProceduralMapGenerator::GetRandomOffsetPosition(FVector origin, float offset)
{
    float X = FMath::RandRange(origin.X - offset, origin.X + offset);
    float Y = FMath::RandRange(origin.Y - offset, origin.Y + offset);
    return FVector(X, Y, 0.f);
}

/// 해당 위치가 유효한지 검사하는 함수 (충돌 방지)
/// 충돌 검사에서 좌표 검사로 수정할 예정
bool UProceduralMapGenerator::CheckLocation(FVector Location)
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

FVector UProceduralMapGenerator::FindNearestValidLocation(FVector Origin, float SearchRadius, EObjectMask ObjectType)
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
bool UProceduralMapGenerator::PlaceObject(FVector Location, UStaticMesh* ObjectMesh)
{
    if (!ObjectMesh || !GetOwner() || !GetWorld())
    {
        UE_LOG(ProceduralMapGenerator, Error, TEXT("Invalid ObjectMesh or Owner!"));
        return false;
    }

    // StaticMeshActor 생성
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = GetOwner();
    AStaticMeshActor* NewActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, FRotator::ZeroRotator, SpawnParams);

    if (NewActor)
    {
        NewActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
        NewActor->GetStaticMeshComponent()->SetStaticMesh(ObjectMesh);
        NewActor->SetActorLocation(Location);

        UE_LOG(ProceduralMapGenerator, Log, TEXT("Placed StaticMeshActor at %s"), *Location.ToString());
        return true;
    }

    return false;
}

#pragma endregion