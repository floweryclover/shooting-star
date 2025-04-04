// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMapGenerator.generated.h"

// 디버깅용 로그 카테고리 선언
DECLARE_LOG_CATEGORY_EXTERN(ProceduralMapGenerator, Log, All);

UCLASS()
class AProceduralMapGenerator : public AActor
{
	GENERATED_BODY()
	
public:
	AProceduralMapGenerator();

    void InitializeMapCoordinate(int32 GridSize);
	void GenerateMap();
    void GenerateObstacles();
    void GenerateSubObstacles();
    void GenerateResources();
    void GenerateDecos();

    bool IsLocationValid(FVector Location);
    bool PlaceObject(FVector Location, UStaticMesh* ObjectMesh);

    // 2D 좌표 => 1D 변환용 인라인 함수
    FORCEINLINE int32 GetIndex(int32 X, int32 Y, int32 Size) { return X + (Y * Size); }
    FVector GetRandomPosition();

protected:
	virtual void BeginPlay() override;

private:
    UPROPERTY(EditAnywhere, Category = "Map Settings")
    int32 mapHalfSize = 9000;

    UPROPERTY(EditAnywhere, Category = "Map Settings")
    int32 numObstacles = 20;
    UPROPERTY(EditAnywhere, Category = "Map Settings")
    int32 numSubObstacles = 20;
    UPROPERTY(EditAnywhere, Category = "Map Settings")
    int32 numResources = 30;
    UPROPERTY(EditAnywhere, Category = "Map Settings")
    int32 numDecos = 30;

    // 맵 좌표를 저장할 1차원 배열
    UPROPERTY(VisibleAnywhere, Category = "Map Settings")
    TArray<int32> mapCoordinate;

    // Plane 액터를 저장할 변수
    UPROPERTY(EditAnywhere, Category = "Procedural Generation")
    AActor* planeActor;

    UPROPERTY(EditAnywhere, Category = "Procedural Generation")
    TArray<UStaticMesh*> obstacleMeshes;
    UPROPERTY(EditAnywhere, Category = "Procedural Generation")
    TArray<UStaticMesh*> subObstacleMeshes;
    UPROPERTY(EditAnywhere, Category = "Procedural Generation")
    TArray<UStaticMesh*> resourceMeshes;
    UPROPERTY(EditAnywhere, Category = "Procedural Generation")
    TArray<UStaticMesh*> decoMeshes;

    UWorld* world; // 현재 World 포인터
};