// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MapObjectActor.generated.h"

/**
 * MapGeneratorComponent가 PlaceObject할 때 생성하는 액터.
 */
UCLASS()
class AMapObjectActor : public AStaticMeshActor
{
	GENERATED_BODY()

public:
    void SetTranslucentMaterial(UMaterialInterface* InMaterial);
	void SetTranslucent(bool bShouldBeTranslucent);

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    UMaterialInterface* DefaultMaterial;

    UPROPERTY()
    UMaterialInterface* TranslucentMaterial;

    bool bIsTranslucent = false;

//
// public:
// 	void SetCollisionType(ECollisionEnabled::Type InCollisionType);
//
// 	void SetCollisionProfileName(const FName& ProfileName);
// 	
// protected:
// 	virtual void BeginPlay() override;
//
// 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
// 	
// 	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_CollisionType)
// 	uint8 CollisionType = ECollisionEnabled::QueryAndPhysics;
//
// 	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_CollisionProfileName)
// 	FName CollisionProfileName = TEXT("BlockAll");
// 	
// private:
// 	UFUNCTION()
// 	void OnRep_CollisionType();
// 	
// 	UFUNCTION()
// 	void OnRep_CollisionProfileName();
};
