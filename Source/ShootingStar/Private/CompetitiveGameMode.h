// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MapEnum.h"
#include "ResourceGenerator.h"
#include "ResourceActor.h"
#include "CompetitiveGameMode.generated.h"

class UCompetitiveSystemComponent;

// Generators 전방 선언 추가
class SHOOTINGSTAR_API UObstacleGenerator;
class SHOOTINGSTAR_API USubObstacleGenerator;
class SHOOTINGSTAR_API UFenceGenerator;
class SHOOTINGSTAR_API UResourceGenerator;
class SHOOTINGSTAR_API UDecorationGenerator;

// 디버깅용 로그 카테고리 선언
DECLARE_LOG_CATEGORY_EXTERN(MapGenerator, Log, All);

/**
 * 2대2 3선승 게임 모드입니다.
 */
UCLASS()
class ACompetitiveGameMode final : public AGameModeBase
{
	GENERATED_BODY()

	friend class UObstacleGenerator;
    friend class USubObstacleGenerator;
    friend class UFenceGenerator;
    friend class UResourceGenerator;
    friend class UDecorationGenerator;

public:
	ACompetitiveGameMode();

	//
	// 오버라이드 함수들
	//
	
	virtual void Tick(float DeltaSeconds) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual void Logout(AController* Exiting) override;

	virtual void SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC) override;

	virtual int32 GetNumPlayers() override
	{
		Super::GetNumPlayers();
		return NumPlayers;
	}

	//
	// Getter/Setter
	//
	
	UCompetitiveSystemComponent* GetCompetitiveSystemComponent() const
	{
		return CompetitiveSystemComponent;
	}

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
	FORCEINLINE UInstancedStaticMeshComponent* GetFenceInstancedMeshComponent() const { return FenceInstancedMeshComponent; }
	
	FORCEINLINE int32 GetNumResources() const { return numResources; }
	FORCEINLINE TSubclassOf<AResourceActor> GetResourceActorClass() const { return ResourceActorClass; }
	FORCEINLINE TArray<FResourceSpawnData> GetResourceSpawnData() const { return ResourceSpawnData; }
	
	FORCEINLINE int32 GetNumDecos() const { return numDecos; }
	FORCEINLINE float GetDecoMinDistance() const { return decoMinDistance; }
	FORCEINLINE float GetClusterRadius() const { return clusterRadius; }
	FORCEINLINE int32 GetMaxClusterNum() const { return maxClusterNum; }
	FORCEINLINE TArray<UStaticMesh*> GetDecoMeshes() const { return decoMeshes; }
	FORCEINLINE TArray<UInstancedStaticMeshComponent*> GetDecorationInstancedMeshComponents() const 
	{ return DecorationInstancedMeshComponents; }

	FORCEINLINE int32 GetMapHalfSize() const { return mapHalfSize; }
	FORCEINLINE float GetPatternSpacing() const { return patternSpacing; }

	//
	// 게임 로직 함수들
	//

	UFUNCTION(BlueprintCallable)
	void InteractResource(AController* Controller);

	UFUNCTION(BlueprintCallable)
	void CraftWeapon(AController* Controller, const FWeaponData& Weapon, const TArray<int32>& Resources);

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadOnly)
	int32 NumPlayers;
	
	// 게임 입장 후 게임 시작까지 필요한 시간
	UPROPERTY(BlueprintReadOnly)
	float GameStartSeconds = 10.0f;

	// 게임 종료 후 이동할 레벨
	UPROPERTY(BlueprintReadOnly)
	FName ExitLevel = TEXT("/Game/Levels/Lobby");
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCompetitiveSystemComponent> CompetitiveSystemComponent;

	// 맵 생성 관련 속성과 함수들
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
	
	FORCEINLINE int32 GetIndex(int32 X, int32 Y)
	{
		return (X + mapHalfSize) + ((Y + mapHalfSize) * mapHalfSize * 2);
	}

private:
	// Map Settings
	UPROPERTY(EditAnywhere, Category = "Map Settings")
	int32 mapHalfSize = 5000;

	UPROPERTY(EditAnywhere, Category = "Map Settings")
	int32 numObstacles = 15;
	UPROPERTY(EditAnywhere, Category = "Map Settings")
	int32 numSubObstacles = 20;
	UPROPERTY(EditAnywhere, Category = "Map Settings")
	int32 numFences = 15;
	UPROPERTY(EditAnywhere, Category = "Map Settings")
	int32 numResources = 40;
	UPROPERTY(EditAnywhere, Category = "Map Settings")
	int32 numDecos = 30;

	// Distance Settings
	UPROPERTY(EditAnywhere, Category = "Map Settings")
	float obstacleMinDistance = 1000.f;
	UPROPERTY(EditAnywhere, Category = "Map Settings")
	float subObstacleMinDistance = 500.f;
	UPROPERTY(EditAnywhere, Category = "Map Settings")
	float fenceMinDistance = 300.f;
	UPROPERTY(EditAnywhere, Category = "Map Settings")
	float decoMinDistance = 50.f;

	// Pattern Settings
	UPROPERTY(EditAnywhere, Category = "Pattern Settings")
	float patternSpacing = 100.f;
	UPROPERTY(EditAnywhere, Category = "Pattern Settings")
	float clusterRadius = 300.f;
	UPROPERTY(EditAnywhere, Category = "Pattern Settings")
	int32 maxClusterNum = 20;

	// Map Coordinate
	UPROPERTY()
	TArray<uint8> mapCoordinate;

	// Resource Settings
	UPROPERTY(EditAnywhere, Category = "Resource Settings")
	TSubclassOf<class AResourceActor> ResourceActorClass;
	UPROPERTY(EditAnywhere, Category = "Resource Settings")
	TArray<FResourceSpawnData> ResourceSpawnData;

	// Meshes
	UPROPERTY(EditAnywhere, Category = "Meshes for Generation")
	TArray<UStaticMesh*> obstacleMeshes;
	UPROPERTY(EditAnywhere, Category = "Meshes for Generation")
	TArray<UStaticMesh*> subObstacleMeshes;
	UPROPERTY(EditAnywhere, Category = "Meshes for Generation")
	UStaticMesh* fenceMesh;
	UPROPERTY(EditAnywhere, Category = "Meshes for Generation")
	TArray<UStaticMesh*> decoMeshes;

	// Components
	UPROPERTY(VisibleAnywhere, Category = "Instanced Meshes")
	UInstancedStaticMeshComponent* FenceInstancedMeshComponent;
	UPROPERTY(VisibleAnywhere, Category = "Instanced Meshes")
	TArray<UInstancedStaticMeshComponent*> DecorationInstancedMeshComponents;

	// Generators
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
