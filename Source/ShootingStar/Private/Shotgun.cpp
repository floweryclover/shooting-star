// Copyright 2025 ShootingStar. All Rights Reserved.


#include "Shotgun.h"
#include "CompetitivePlayerCharacter.h"
#include "TeamComponent.h"
#include "Rifle_Projectile.h"

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

void AShotgun::ProjectileFire(FVector loc, FRotator rot, FRotator bulletRot)
{
	const int32 PelletCount = 8;
	const float SpreadAngle = 5.0f;

	FActorSpawnParameters spawnParameter;
	spawnParameter.Owner = GetOwner();
	spawnParameter.Instigator = GetInstigator();

	for (int32 i = 0; i < PelletCount; i++)
	{
		// 퍼짐 각도 랜덤 생성
		float YawSpread = FMath::FRandRange(-SpreadAngle, SpreadAngle);
		float PitchSpread = FMath::FRandRange(-SpreadAngle, SpreadAngle);
		FRotator SpreadRot = bulletRot + FRotator(PitchSpread, YawSpread, 0.0f);

		auto projectile = GetWorld()->SpawnActor<ARifle_Projectile>(ARifle_Projectile::StaticClass(), loc, rot, spawnParameter);
		if (projectile)
		{
			projectile->SetReplicates(true);
			projectile->SetActorEnableCollision(true);
			projectile->SetProjectileVelocity(3000.0f);

			AActor* Shooter = GetAttachParentActor();
			if (UTeamComponent* TeamComp = Shooter->FindComponentByClass<UTeamComponent>()) {
				projectile->ShooterTeam = TeamComp->GetTeam();
			}

			FVector FireDirection = SpreadRot.Vector();
			FireDirection.Z = 0;
			FireDirection.Normalize();

			if (ACompetitivePlayerCharacter* OwnerCharacter = Cast<ACompetitivePlayerCharacter>(GetOwner())) {
				float NewDamage = OwnerCharacter->IncreasedDamage * projectile->GetProjectileDamage();
				projectile->SetProjectileDamage(NewDamage);
			}

			projectile->ProjectileFire(FireDirection, GetOwner());
		}
	}
}
