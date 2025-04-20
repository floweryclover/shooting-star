// Copyright 2025 ShootingStar. All Rights Reserved.

#include "FenceGenerator.h"
#include "MapGeneratorComponent.h"

UFenceGenerator::UFenceGenerator()
{
}

void UFenceGenerator::Initialize(UMapGeneratorComponent* InOwner)
{
    Owner = InOwner;

    if (Owner)
    {
        numFences = Owner->GetNumFences();
        fenceMinDistance = Owner->GetFenceMinDistance();
        fenceMesh = Owner->GetFenceMesh();

        // FenceInstancedMeshComponent 가져오기 및 확인
        if (Owner->GetFenceInstancedMeshComponent())
        {
            SetInstancedMeshComponent(Owner->GetFenceInstancedMeshComponent());
            UE_LOG(MapGenerator, Log, TEXT("(Fence) InstancedMeshComponent set successfully"));
        }
        else
            UE_LOG(MapGenerator, Error, TEXT("(Fence) Failed to get InstancedMeshComponent"));
    }
}

void UFenceGenerator::SetInstancedMeshComponent(UInstancedStaticMeshComponent* InMeshComponent) 
{
    if (!InMeshComponent)
    {
        UE_LOG(MapGenerator, Error, TEXT("FenceGenerator: Invalid InstancedMeshComponent!"));
        return;
    }
    FenceInstancedMeshComponent = InMeshComponent;

    FenceInstancedMeshComponent->SetVisibility(true);
    FenceInstancedMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void UFenceGenerator::GenerateObjects()
{
    UE_LOG(MapGenerator, Log, TEXT("(Fence) Generating Fences Started"));

    if (!Owner || !fenceMesh)
    {
        UE_LOG(MapGenerator, Error, TEXT("(Fence) Owner or fenceMesh is not set!"));
        return;
    }

    if (!FenceInstancedMeshComponent)
    {
        UE_LOG(MapGenerator, Error, TEXT("(Fence) InstancedMeshComponent is not set!"));
        return;
    }

    int32 SpawnAttempts = 0;
    int32 PlacedObjects = 0;

    while (PlacedObjects < numFences && SpawnAttempts < numFences * 5)
    {
        if (FMath::RandBool())
        {
            if (GenerateFenceAroundObstacle())
                PlacedObjects++;
        }
        else
        {
            FVector RandomLocation = Owner->GetRandomPosition();
            if (!Owner->CheckLocation(RandomLocation))
            {
                RandomLocation = Owner->FindNearestValidLocation(RandomLocation, 500.f, EObjectMask::FenceMask);
            }

            EPatternType RandomPattern = static_cast<EPatternType>(FMath::RandRange(0, 2));
            TArray<FFenceData> FencePositions;
            GenerateFencePattern(RandomLocation, RandomPattern, FMath::RandRange(400.f, 800.f), FencePositions);

            if (PlaceFencePattern(FencePositions))
                PlacedObjects++;
        }
        SpawnAttempts++;
    }

    UE_LOG(MapGenerator, Log, TEXT("(Fence) Generating Fences Completed"));
}

// 생성할 펜스의 모양을 랜덤하게 결정하고, 펜스의 위치들을 Fvector 배열에 담아주는 함수
// 펜스의 모양은 Rectangle, UShape, LShape, Surrounding이 존재하며 생성할 패턴 enum을 인수로 받는다
void UFenceGenerator::GenerateFencePattern(const FVector& Center, EPatternType PatternType, float Radius, TArray<FFenceData>& OutPositions)
{
    // 맵 범위를 벗어나지 않도록 Radius 제한
    const float MaxRadius = FMath::Min(Radius, Owner->GetMapHalfSize() * 0.8f);
    
    // 패턴의 전체 범위가 맵 안에 있는지 확인
    if (FMath::Abs(Center.X) + MaxRadius >= Owner->GetMapHalfSize() ||
        FMath::Abs(Center.Y) + MaxRadius >= Owner->GetMapHalfSize())
    {
        return;
    }

    switch (PatternType)
    {
        case EPatternType::Rectangle:
            {
                // 세로 방향 펜스
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
                // 가로 방향 펜스
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
            UE_LOG(MapGenerator, Warning, TEXT("(Fence) Invalid PatternType . . ."));
            break;
    }
}

bool UFenceGenerator::PlaceFencePattern(const TArray<FFenceData>& Positions)
{
    if (Positions.Num() == 0 || !FenceInstancedMeshComponent) return false;

    bool bPlacedAny = false;
    for (const FFenceData& FenceData : Positions)
    {
        if (Owner->CheckLocation(FenceData.Location))
        {
            FenceInstancedMeshComponent->AddInstance(
                FTransform(FenceData.Rotation, FenceData.Location)
            );
            Owner->SetObjectRegion(FenceData.Location, fenceMesh, EObjectMask::FenceMask);
            bPlacedAny = true;
        }
    }
    return bPlacedAny;
}

// 현재 맵에 생성되어 있는 Obstacle를 탐색하고, 그중 하나를 선택하여 주변에 펜스를 생성하는 함수
bool UFenceGenerator::GenerateFenceAroundObstacle()
{
    TArray<FVector> ObstacleLocations;
    const int32 MapHalfSize = Owner->GetMapHalfSize(); // mapHalfSize 직접 접근을 getter로 변경
    
    for (int32 X = -MapHalfSize; X < MapHalfSize; X += 100)
    {
        for (int32 Y = -MapHalfSize; Y < MapHalfSize; Y += 100)
        {
            if (Owner->HasObjectAtArray(X, Y, EObjectMask::ObstacleMask))
                ObstacleLocations.Add(FVector(X, Y, 0.f));
        }
    }

    if (ObstacleLocations.Num() == 0)
    {
        UE_LOG(MapGenerator, Warning, TEXT("(Fence) No obstacles found to generate fence around."));
        return false;
    }

    // 탐색이 완료되어 완성된 Obstacle 배열에서 랜덤하게 Obstacle 선택
    FVector TargetLocation = ObstacleLocations[FMath::RandRange(0, ObstacleLocations.Num() - 1)];
    
    // 패턴 랜덤 선택 및 생성
    EPatternType RandomPattern = static_cast<EPatternType>(FMath::RandRange(0, 2));
    
    float randomTemp = FMath::FRandRange(0.f, 1.f);
    float randomRadius = 0.f;
    if (randomTemp <= 0.33f)
        randomRadius = 900.f;
    else if (randomTemp <= 0.66f)
        randomRadius = 1200.f;
    else
        randomRadius = 1500.f;
    
    TArray<FFenceData> FencePositions;
    GenerateFencePattern(TargetLocation, RandomPattern, randomRadius, FencePositions);

    UE_LOG(MapGenerator, Log, TEXT("(Fence) Generating fence pattern around obstacle at %s"), *TargetLocation.ToString());
    return PlaceFencePattern(FencePositions);
}