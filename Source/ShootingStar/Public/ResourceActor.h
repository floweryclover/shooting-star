// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResourceDataAsset.h"
#include "ResourceActor.generated.h"

class UStaticMeshComponent;
class UResource;
class UInventoryComponent;

UCLASS()
class SHOOTINGSTAR_API AResourceActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AResourceActor();
	void Harvest(UInventoryComponent* Inventory);

protected:
	// Called when the game starts or when spawned
	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
private:
	void UpdateVisual();

public:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UResourceDataAsset* ResourceData;
};
