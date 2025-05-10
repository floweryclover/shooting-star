// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Gun.generated.h"


UENUM(BlueprintType)
enum class WEAPONLEVER : uint8 {
	FULLAUTO,
	TRIPLE,
	SINGLEFIRE
};

UENUM(BlueprintType)
enum class WEAPONTYPE : uint8 {
	RIFLE,
	PISTOL,
	SHOTGUN,
	ROCKETLAUNCHER
};

UCLASS()
class SHOOTINGSTAR_API AGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGun();

	void PullTrigger();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Base)
	class USkeletalMeshComponent* BodyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	int LoadedAmmo = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	WEAPONLEVER WeaponeLever;
	WEAPONTYPE WeaponType;

	bool IsEmpty = false;
	bool IsReload = false;

	UAnimMontage* fireMontage;
	UAnimMontage* reloadMontage;

	int baseKeepAmmo = 0;
	int baseLoadedAmmo = 0;
	int limitAmmo = 0;
	FName WeaponName = "";

	TArray<bool> WeaponeLeverCheck;

	class USoundBase* EmptySound;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY()
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere)
	float MaxRange = 1000;

	UPROPERTY(EditAnywhere)
	float Damage = 10;

	WEAPONLEVER GetWeaponeLever() { return WeaponeLever; }; void SetWeaponeLever(WEAPONLEVER set) { WeaponeLever = set; };
	WEAPONTYPE GetWeaponType() { return WeaponType; }; void SetWeaponType(WEAPONTYPE set) { WeaponType = set; };

	bool GetIsEmpty() { return IsEmpty; }; void SetIsEmpty(bool set) { IsEmpty = set; };
	bool GetIsReload() { return IsReload; }; void SetIsReload(bool set) { IsReload = set; };

	virtual UClass* GetStaticClass();
	int GetBaseKeepAmmo() { return baseKeepAmmo; }
	int GetBaseLoadedAmmo() { return baseLoadedAmmo; }
	FName GetWeaponName() { return WeaponName; }
	int GetLimitAmmo() { return limitAmmo; }

	virtual AGun* SpawnToHand(APawn* owner, FVector loc, FRotator rot);

	virtual void ProjectileFire(FVector loc, FRotator rot, FRotator bulletRot);
	virtual void PlayFireMontage();
	virtual void StopFireMontage();
	virtual void PlayReloadMontage();
	virtual void StopReloadMontage();

};
