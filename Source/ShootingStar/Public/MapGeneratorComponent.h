// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MapEnum.h"
#include "ResourceGenerator.h"
#include "ResourceActor.h"
#include "MapInstancedMeshActor.h"
#include "MapGeneratorComponent.generated.h"

// Generators 전방 선언
class SHOOTINGSTAR_API UObstacleGenerator;
class SHOOTINGSTAR_API USubObstacleGenerator;
class SHOOTINGSTAR_API UFenceGenerator;
class SHOOTINGSTAR_API UResourceGenerator;
class SHOOTINGSTAR_API UDecorationGenerator;

DECLARE_LOG_CATEGORY_EXTERN(MapGenerator, Log, All);

/**
 * @brief 맵을 생성하는 컴포넌트입니다.
 * @details 맵의 크기, 장애물, 자원, 장식물 등을 설정하고 생성합니다.
 * @note 이 컴포넌트는 CompetitiveGameMode에 부착되어 사용됩니다.
**/
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UMapGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class UObstacleGenerator;
    friend class USubObstacleGenerator;
    friend class UFenceGenerator;
    friend class UResourceGenerator;
    friend class UDecorationGenerator;

public:	
	UMapGeneratorComponent();

	// Getter functions for Generators
	FORCEINLINE int32 GetNumObstacles() const { return numObstacles; }
	FORCEINLINE float GetObstacleMinDistance() const { return obstacleMinDistance; }
	FORCEINLINE TArray<UStaticMesh*> GetObstacleMeshes() const { return obstacleMeshes; }
	
	FORCEINLINE int32 GetNumSubObstacles() const { return numSubObstacles; }
	FORCEINLINE float GetSubObstacleMinDistance() const { return subObstacleMinDistance; }
	FORCEINLINE TArray<UStaticMesh*> GetSubObstacleMeshes() const { return subObstacleMeshes; }
	
	FORCEINLINE int32 GetNumFences() const { return numFences; }
	FORCEINLINE float GetFenceMinDistance() const { return fenceMinDistance; }
	FORCEINLINE UStaticMesh* GetFenceMesh() const { return fenceMesh; }

	FORCEINLINE int32 GetNumResources() const { return numResources; }
	FORCEINLINE TSubclassOf<AResourceActor> GetResourceActorClass() const { return ResourceActorClass; }
	FORCEINLINE TArray<FResourceSpawnData> GetResourceSpawnData() const { return ResourceSpawnData; }
	
	FORCEINLINE int32 GetNumDecos() const { return numDecos; }
	FORCEINLINE float GetDecoMinDistance() const { return decoMinDistance; }
	FORCEINLINE float GetClusterRadius() const { return clusterRadius; }
	FORCEINLINE int32 GetMaxClusterNum() const { return maxClusterNum; }
	FORCEINLINE TArray<UStaticMesh*> GetDecoMeshes() const { return decoMeshes; }

	FORCEINLINE float GetFenceGenerationProbability() const { return fenceGenerationProbability; }

	FORCEINLINE int32 GetMapHalfSize() const { return mapHalfSize; }
	FORCEINLINE float GetPatternSpacing() const { return patternSpacing; }

	FORCEINLINE AMapInstancedMeshActor* GetMapInstancedMeshActor() const { return MapInstancedMeshActor; }

	FORCEINLINE const TArray<FVector>& GetPlayerSpawnPoints() const { return PlayerSpawnPoints; }
	
	void Initialize();
	void InitializeMapCoordinate(int32 GridSize);
	void GenerateMap();
	void SetObjectAtArray(int32 X, int32 Y, EObjectMask ObjectType);
	void SetObjectRegion(FVector Location, UStaticMesh* ObjectMesh, EObjectMask ObjectType);
	void ClearObjectTypeFromMap(EObjectMask ObjectType);
	bool HasObjectAtArray(int32 X, int32 Y, EObjectMask ObjectType);
	bool CheckLocation(FVector Location);
	bool PlaceObject(FVector Location, UStaticMesh* ObjectMesh);

	// 자원을 재생성하는 함수
    void RegenerateResources();

	FVector GetRandomPosition();
	FVector GetRandomOffsetPosition(FVector origin, float offset);
	FVector FindNearestValidLocation(FVector Origin, float SearchRadius, EObjectMask ObjectType);
	
	
	FORCEINLINE int32 GetIndex(int32 X, int32 Y)
	{
		return (X + mapHalfSize) + ((Y + mapHalfSize) * mapHalfSize * 2);
	}

	// 스폰 포인트 관련 함수
	void InitializeSpawnPoints();
	bool IsValidSpawnLocation(const FVector& Location);
	UFUNCTION(BlueprintCallable, Category = "Spawn System")
	FVector GetRandomSpawnLocation();

	FVector GetSupplySpawnLocation();

protected:
	FVector GetRandomSupplySpawnLocation();

	// Map Settings
	UPROPERTY(EditAnywhere, Category = "Map Settings", meta = (ClampMin = "1000", ClampMax = "10000"))
	int32 mapHalfSize = 2500;

	UPROPERTY(EditAnywhere, Category = "Map Settings", meta = (ClampMin = "1", ClampMax = "20"))
	int32 numObstacles = 5;
	UPROPERTY(EditAnywhere, Category = "Map Settings", meta = (ClampMin = "1", ClampMax = "50"))
	int32 numSubObstacles = 15;
	UPROPERTY(EditAnywhere, Category = "Map Settings", meta = (ClampMin = "1", ClampMax = "20"))
	int32 numFences = 5;
	UPROPERTY(EditAnywhere, Category = "Map Settings", meta = (ClampMin = "1", ClampMax = "100"))
	int32 numResources = 25;
	UPROPERTY(EditAnywhere, Category = "Map Settings", meta = (ClampMin = "1", ClampMax = "100"))
	int32 numDecos = 20;

	// Distance Settings
	UPROPERTY(EditAnywhere, Category = "Distance Settings", meta = (ClampMin = "100.0", ClampMax = "5000.0"))
	float obstacleMinDistance = 1000.f;
	UPROPERTY(EditAnywhere, Category = "Distance Settings", meta = (ClampMin = "100.0", ClampMax = "2000.0"))
	float subObstacleMinDistance = 500.f;
	UPROPERTY(EditAnywhere, Category = "Distance Settings", meta = (ClampMin = "50.0", ClampMax = "1000.0"))
	float fenceMinDistance = 300.f;
	UPROPERTY(EditAnywhere, Category = "Distance Settings", meta = (ClampMin = "10.0", ClampMax = "200.0"))
	float decoMinDistance = 50.f;
	
	// 펜스 생성 확률
	UPROPERTY(EditAnywhere, Category = "Fence Settings", meta = (ClampMin = "1", ClampMax = "100"))
	float fenceGenerationProbability = 80.f;

	// Pattern Settings
	UPROPERTY(EditAnywhere, Category = "Pattern Settings", meta = (ClampMin = "50.0", ClampMax = "500.0"))
	float patternSpacing = 100.f;
	UPROPERTY(EditAnywhere, Category = "Pattern Settings", meta = (ClampMin = "100.0", ClampMax = "1000.0"))
	float clusterRadius = 500.f;
	UPROPERTY(EditAnywhere, Category = "Pattern Settings", meta = (ClampMin = "1", ClampMax = "100"))
	int32 maxClusterNum = 20;

	// For Instanced Meshes
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Map Instanced Static Meshes")
	AMapInstancedMeshActor* MapInstancedMeshActor;
	
	// Map Coordinate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map Data")
	TArray<uint8> mapCoordinate;

	// Resource Settings
	UPROPERTY(EditAnywhere, Category = "Resource Settings")
	TSubclassOf<class AResourceActor> ResourceActorClass;
	UPROPERTY(EditAnywhere, Category = "Resource Settings")
	TArray<FResourceSpawnData> ResourceSpawnData;

	// Meshes
	UPROPERTY(EditAnywhere, Category = "Meshes")
	TArray<UStaticMesh*> obstacleMeshes;
	UPROPERTY(EditAnywhere, Category = "Meshes")
	TArray<UStaticMesh*> subObstacleMeshes;
	UPROPERTY(EditAnywhere, Category = "Meshes")
	UStaticMesh* fenceMesh;
	UPROPERTY(EditAnywhere, Category = "Meshes")
	TArray<UStaticMesh*> decoMeshes;

	// Generators
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generators")
	UObstacleGenerator* obstacleGenerator;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generators")
	USubObstacleGenerator* subObstacleGenerator;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generators")
	UFenceGenerator* fenceGenerator;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generators")
	UResourceGenerator* resourceGenerator;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generators")
	UDecorationGenerator* decorationGenerator;

	// 스폰 포인트 설정
	UPROPERTY(EditAnywhere, Category = "Spawn Settings")
	int32 NumSpawnPointsPerQuadrant = 5; // 각 사분면당 스폰 포인트 개수

	UPROPERTY(VisibleAnywhere, Category = "Spawn Settings")
	TArray<FVector> PlayerSpawnPoints;

	// 스폰 포인트 간 최소 거리
	UPROPERTY(EditAnywhere, Category = "Spawn Settings")
	float MinSpawnPointDistance = 1000.f;

	// 맵 경계로부터의 최소 거리
	UPROPERTY(EditAnywhere, Category = "Spawn Settings")
	float BorderMargin = 1000.f;
};
