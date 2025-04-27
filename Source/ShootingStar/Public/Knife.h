// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Knife.generated.h"

UCLASS(Blueprintable)
class SHOOTINGSTAR_API AKnife : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKnife();

	UFUNCTION()
	virtual void OnOverlapBegin_Body(UPrimitiveComponent* OverlappedComp, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, 
		const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Base)
	class USkeletalMeshComponent* BodyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* Sound;
	float knifeDamage = 30.0f;
	float GetknifeDamage() { return knifeDamage; };
	void SetknifeDamage(float Damage) { knifeDamage = Damage; };
	bool bIsAttacking;
	UPROPERTY(VisibleAnywhere)
	bool bHitBoxActive;
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* AttackHitBox;
	bool bHasDamaged = false;
	void ResetDamageFlag();
	FTimerHandle ResetDamageFlagHandle;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
