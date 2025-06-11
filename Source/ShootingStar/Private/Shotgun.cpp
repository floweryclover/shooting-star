// Copyright 2025 ShootingStar. All Rights Reserved.


#include "Shotgun.h"
#include "CompetitivePlayerCharacter.h"
#include "TeamComponent.h"
#include "Rifle_Projectile.h"
#include "Shotgun_Projectile.h"

AShotgun::AShotgun()
{
	PrimaryActorTick.bCanEverTick = false;
	BodyMesh->SetIsReplicated(true);
	BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	BodyMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>SKELETALGUN(TEXT("SkeletalMesh'/Game/Toon_Soldiers_UE5/Meshes/Weapons/SKM_weapon_shotgun.SKM_weapon_shotgun'"));
	if (SKELETALGUN.Succeeded()) {
		BodyMesh->SetSkeletalMesh(SKELETALGUN.Object);
	}

	SetWeaponeLever(WEAPONLEVER::FULLAUTO);

	WeaponName = "Shotgun";
	baseKeepAmmo = 20;
	baseLoadedAmmo = 8;
	LoadedAmmo = baseLoadedAmmo;
	limitAmmo = 40;

	WeaponType = WEAPONTYPE::SHOTGUN;
	for (int i = 0; i < WeaponeLeverCheck.Num(); i++) WeaponeLeverCheck[i] = true;

}
void AShotgun::BeginPlay()
{
	Super::BeginPlay();
}

UClass* AShotgun::GetStaticClass()
{
	return AShotgun::StaticClass();
}
AGun* AShotgun::SpawnToHand(APawn* owner, FVector loc, FRotator rot)
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnToHand"));
	FActorSpawnParameters spawnParameters;
	spawnParameters.Owner = owner;
	spawnParameters.Instigator = GetInstigator();

	AShotgun* weapon = GetWorld()->SpawnActor<AShotgun>(AShotgun::StaticClass(), loc, rot, spawnParameters);
	return weapon;
}

bool AShotgun::ProjectileFire(FVector loc, FRotator rot, FRotator bulletRot)
{
	Super::ProjectileFire(loc, rot, bulletRot);

	if (GetIsReload())
	{
		return false;
	}
	LastFireTime = GetWorld()->GetTimeSeconds();
	
	const int32 PelletCount = 8;
	const float AngleStep = 6.0f; // 각 총알 사이 각도 차이
	const int32 HalfPellets = PelletCount / 2;

	FActorSpawnParameters spawnParameter;
	spawnParameter.Owner = GetOwner();
	spawnParameter.Instigator = GetInstigator();

	for (int32 i = 0; i < PelletCount; i++)
	{
		int32 IndexOffset = i - HalfPellets;
		if (PelletCount % 2 == 0 && i >= HalfPellets)
			IndexOffset += 1;

		// 고정된 각도 스프레드
		float YawOffset = IndexOffset * AngleStep;
		FRotator SpreadRot = bulletRot + FRotator(0.0f, YawOffset, 0.0f); // Pitch는 고정, Yaw만 조정

		auto projectile = GetWorld()->SpawnActor<AShotgun_Projectile>(AShotgun_Projectile::StaticClass(), loc, rot, spawnParameter);
		if (projectile)
		{
			projectile->SetReplicates(true);
			projectile->SetActorEnableCollision(true);
			projectile->SetProjectileVelocity(3000.0f);
			projectile->SetProjectileDamage(6.0f);

			AActor* Shooter = GetAttachParentActor();
			if (UTeamComponent* TeamComp = Shooter->FindComponentByClass<UTeamComponent>()) {
				projectile->ShooterTeam = TeamComp->GetTeam();
				projectile->WeaponFired = this;
			}

			FVector FireDirection = SpreadRot.Vector();
			FireDirection.Z = 0;
			FireDirection.Normalize();

			if (ACompetitivePlayerCharacter* OwnerCharacter = Cast<ACompetitivePlayerCharacter>(GetOwner())) {
				float NewDamage = OwnerCharacter->GetIncreasedDamage() * projectile->GetProjectileDamage();
				projectile->SetProjectileDamage(NewDamage);
				projectile->SetDamageType(DamageTypeClass);
			}
			if (IsValid(DamageTypeClass))
			{
				projectile->DamageTypeClass = this->DamageTypeClass;
			}
			projectile->ProjectileFire(FireDirection, GetOwner());
		}
	}

	return true;
}
