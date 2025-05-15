// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResourceDataAsset.h"
#include "ResourceActor.generated.h"

class UStaticMeshComponent;
class UResource;

UCLASS()
class SHOOTINGSTAR_API AResourceActor : public AActor
{
	GENERATED_BODY()
	
public:
	static constexpr float HitShakeTime = 0.1f;
	static constexpr float HitShakeRadius = 30.0f;
	// Sets default values for this actor's properties
	AResourceActor();

	virtual void Tick(float DeltaSeconds) override;

	const UResourceDataAsset* GetResourceData() const
	{
		return ResourceData;
	}

	void Hit(const FVector& InHitLocation);

	void UpdateMesh_AfterHarvest();
	void UpdateVisual();

protected:
	virtual void BeginPlay() override;
	// Called when the game starts or when spawned
	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
private:
	UPROPERTY(ReplicatedUsing=OnRep_ResourceState)
	EResourceState ResourceState = EResourceState::Large;

public:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_ResourceData)
	UResourceDataAsset* ResourceData;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_ResourceData();

	UFUNCTION()
	void OnRep_ResourceState();

	UPROPERTY(Replicated)
	float RemainingHitShakeTime = 0.0f;

	UPROPERTY(Replicated)
	FVector OriginLocation;

	UPROPERTY(Replicated)
	FVector HitLocation;
};
