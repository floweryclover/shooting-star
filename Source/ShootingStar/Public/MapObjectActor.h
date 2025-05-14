// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MapObjectActor.generated.h"

/**
 * MapGeneratorComponent가 PlaceObject할 때 생성하는 액터.
 */
UCLASS()
class AMapObjectActor final : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	void SetCollisionType(ECollisionEnabled::Type InCollisionType);

	void SetCollisionProfileName(const FName& ProfileName);
	
protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_CollisionType)
	uint8 CollisionType;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_CollisionProfileName)
	FName CollisionProfileName;
	
private:
	UFUNCTION()
	void OnRep_CollisionType();
	
	UFUNCTION()
	void OnRep_CollisionProfileName();
};
