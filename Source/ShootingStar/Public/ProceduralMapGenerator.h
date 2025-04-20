// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include <utility>
#include "MapEnum.h"
#include "ResourceGenerator.h"
#include "ProceduralMapGenerator.generated.h"

class SHOOTINGSTAR_API UObstacleGenerator;
class SHOOTINGSTAR_API USubObstacleGenerator;
class SHOOTINGSTAR_API UFenceGenerator;
class SHOOTINGSTAR_API UResourceGenerator;
class SHOOTINGSTAR_API UDecorationGenerator;

// 디버깅용 로그 카테고리 선언
DECLARE_LOG_CATEGORY_EXTERN(ProceduralMapGenerator, Log, All);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHOOTINGSTAR_API UProceduralMapGenerator : public UActorComponent
{
	GENERATED_BODY()

    friend class UObstacleGenerator;
    friend class USubObstacleGenerator;
    friend class UFenceGenerator;
    friend class UResourceGenerator;
    friend class UDecorationGenerator;
	
public:
	UProceduralMapGenerator();

    void InitializeMapCoordinate(int32 GridSize);

	void GenerateMap();

    void SetObjectAtArray(int32 X, int32 Y, EObjectMask ObjectType);
    void SetObjectRegion(FVector Location, UStaticMesh* ObjectMesh, EObjectMask ObjectType);
    bool HasObjectAtArray(int32 X, int32 Y, EObjectMask ObjectType);

    bool CheckLocation(FVector Location);
    bool PlaceObject(FVector Location, UStaticMesh* ObjectMesh);

    FVector GetRandomPosition();
    FVector GetRandomOffsetPosition(FVector origin, float offset);

    FVector FindNearestValidLocation(FVector Origin, float SearchRadius, EObjectMask ObjectType);
    
    // 2D 좌표 => 1D 변환용 인라인 함수
    // 음수값이 들어올 경우, 맵의 반지름을 더해 배열의 인덱스로 변환한다.
    FORCEINLINE int32 GetIndex(int32 X, int32 Y)
    {
        return (X + mapHalfSize) + ((Y + mapHalfSize) * mapHalfSize * 2);
    }

    UPROPERTY(EditAnywhere, Category = "Resource Settings")
    TSubclassOf<class AResourceActor> ResourceActorClass;

    UPROPERTY(EditAnywhere, Category = "Resource Settings")
    TArray<FResourceSpawnData> ResourceSpawnData;

protected:
	virtual void BeginPlay() override;

private:
    // 맵의 지름은 100m로 설정
    UPROPERTY(EditAnywhere, Category = "Map Settings")
    int32 mapHalfSize = 5000;

    UPROPERTY(EditAnywhere, Category = "Map Settings")
    int32 numObstacles = 20;
    UPROPERTY(EditAnywhere, Category = "Map Settings")
    int32 numSubObstacles = 20;
    UPROPERTY(EditAnywhere, Category = "Map Settings")
    int32 numFences = 15;
    UPROPERTY(EditAnywhere, Category = "Map Settings")
    int32 numResources = 40;
    UPROPERTY(EditAnywhere, Category = "Map Settings")
    int32 numDecos = 30;

    // Obstacles 간 최소 거리
    UPROPERTY(EditAnywhere, Category = "Map Settings")
    float obstacleMinDistance = 1000.f;
    // SubObstacles 간 최소 거리
    UPROPERTY(EditAnywhere, Category = "Map Settings")
    float subObstacleMinDistance = 500.f;
    UPROPERTY(EditAnywhere, Category = "Map Settings")
    float fenceMinDistance = 300.f; // 펜스 간격
    // Decorations 간 최소 거리
    UPROPERTY(EditAnywhere, Category = "Map Settings")
    float decoMinDistance = 50.f;
    // 패턴 생성의 spacing offset 변수
    UPROPERTY(EditAnywhere, Category = "Pattern Settings")
    float patternSpacing = 100.f;
    UPROPERTY(EditAnywhere, Category = "Pattern Settings")
    float clusterRadius = 300.f;
    // 풀, 덤불 등의 군집 크기
    UPROPERTY(EditAnywhere, Category = "Pattern Settings")
    int32 maxClusterNum = 20;

    // 맵 좌표를 저장할 1차원 배열
    UPROPERTY()
    TArray<uint8> mapCoordinate;

    // Plane 액터를 저장할 변수
    UPROPERTY(EditAnywhere, Category = "Map Settings")
    AActor* planeActor;

    UPROPERTY(EditAnywhere, Category = "Meshes for Generation")
    TArray<UStaticMesh*> obstacleMeshes;
    UPROPERTY(EditAnywhere, Category = "Meshes for Generation")
    TArray<UStaticMesh*> subObstacleMeshes;
    UPROPERTY(EditAnywhere, Category = "Meshes for Generation")
    UStaticMesh* fenceMesh;
    UPROPERTY(EditAnywhere, Category = "Meshes for Generation")
    TArray<UStaticMesh*> decoMeshes;

    // 반복 생성될 객체를 위한 InstancedStaticMeshComponent
    UPROPERTY(VisibleAnywhere, Category = "Instanced Meshes")
    UInstancedStaticMeshComponent* FenceInstancedMeshComponent;
    UPROPERTY(VisibleAnywhere, Category = "Instanced Meshes")
    TArray<UInstancedStaticMeshComponent*> DecorationInstancedMeshComponents;

    UPROPERTY(EditAnywhere, Category = "Generators")
    UObstacleGenerator* obstacleGenerator;
    UPROPERTY(EditAnywhere, Category = "Generators")
    USubObstacleGenerator* subObstacleGenerator;
    UPROPERTY(EditAnywhere, Category = "Generators")
    UFenceGenerator* fenceGenerator;
    UPROPERTY(EditAnywhere, Category = "Generators")
    UResourceGenerator* resourceGenerator;
    UPROPERTY(EditAnywhere, Category = "Generators")
    UDecorationGenerator* decorationGenerator;
};