// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvironmentActorSpawner.generated.h"

UCLASS()
class SHOOTINGSTAR_API AEnvironmentActorSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnvironmentActorSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnConstruction(const FTransform& Transform) override;

public:
	void Spawn();

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere)
	FVector SpawnExtent{};

	UPROPERTY(EditAnywhere)
	float CoolTime{};

	UPROPERTY(EditAnywhere)
	int32 TotalSpawnNumber{};

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	int32 CurSpawnNumber{};
	float TimeSum{};
};
