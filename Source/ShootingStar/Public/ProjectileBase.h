// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

enum class ETeam :uint8;

UCLASS()
class SHOOTINGSTAR_API AProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	class UStaticMeshComponent* BodyMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	class USphereComponent* Sphere;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	class UProjectileMovementComponent* Movement;

	UPROPERTY()
	ETeam ShooterTeam;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY()
	UMaterialInterface* bulletHoleDecal = nullptr;
	float projectileDamage = 7.0f;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
	TSubclassOf<UDamageType> DamageTypeClass;

	void SetDamageType(TSubclassOf<UDamageType> InDamageType)
	{
		DamageTypeClass = InDamageType;
	}

	void ProjectileFire(FVector FireDir, AActor* Onwer);
	void SetProjectileVelocity(float Velocity);
	float GetProjectileDamage() { return projectileDamage; };
	void SetProjectileDamage(float Damage) { projectileDamage = Damage; };

	UFUNCTION()
	virtual void OnOverlapBegin_Body(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);
};
