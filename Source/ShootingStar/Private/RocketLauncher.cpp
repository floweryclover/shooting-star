// Copyright 2025 ShootingStar. All Rights Reserved.


#include "RocketLauncher.h"
#include "CompetitivePlayerCharacter.h"
#include "TeamComponent.h"
#include "Rocket_Projectile.h"

ARocketLauncher::ARocketLauncher()
{
	PrimaryActorTick.bCanEverTick = false;
	BodyMesh->SetIsReplicated(true);
	BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	BodyMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>SKELETALGUN(TEXT("/Script/Engine.SkeletalMesh'/Game/Toon_Soldiers_UE5/Meshes/Weapons/SKM_weapon_rocket_launcher.SKM_weapon_rocket_launcher'"));
	if (SKELETALGUN.Succeeded()) {
		BodyMesh->SetSkeletalMesh(SKELETALGUN.Object);
	}

	SetWeaponeLever(WEAPONLEVER::FULLAUTO);

	WeaponName = "RocketLauncher";
	baseKeepAmmo = 60;
	baseLoadedAmmo = 30;
	LoadedAmmo = baseLoadedAmmo;
	limitAmmo = 90;

	WeaponType = WEAPONTYPE::ROCKETLAUNCHER;
	for (int i = 0; i < WeaponeLeverCheck.Num(); i++) WeaponeLeverCheck[i] = true;

	static ConstructorHelpers::FObjectFinder<USoundWave> SoundWaveAsset(TEXT("SoundWave'/Game/Audios/SFX/WeaponSounds/Explosion_Large_1-1.Explosion_Large_1-1'"));
	if (SoundWaveAsset.Succeeded())
	{
		ShootSound = SoundWaveAsset.Object;
	}

}
void ARocketLauncher::BeginPlay()
{
	Super::BeginPlay();
}

UClass* ARocketLauncher::GetStaticClass()
{
	return ARocketLauncher::StaticClass();
}
AGun* ARocketLauncher::SpawnToHand(APawn* owner, FVector loc, FRotator rot)
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnToHand"));
	FActorSpawnParameters spawnParameters;
	spawnParameters.Owner = owner;
	spawnParameters.Instigator = GetInstigator();

	ARocketLauncher* weapon = GetWorld()->SpawnActor<ARocketLauncher>(ARocketLauncher::StaticClass(), loc, rot, spawnParameters);
	return weapon;
}

bool ARocketLauncher::ProjectileFire(FVector loc, FRotator rot, FRotator bulletRot)
{
	Super::ProjectileFire(loc, rot, bulletRot);

	if (GetIsReload())
	{
		return false;
	}
	LastFireTime = GetWorld()->GetTimeSeconds();
	
	FActorSpawnParameters spawnParameter;
	spawnParameter.Owner = GetOwner();
	spawnParameter.Instigator = GetInstigator();
	spawnParameter.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	auto projectile = GetWorld()->SpawnActor<ARocket_Projectile>(RocketClass, loc, rot, spawnParameter);
	if (projectile) {
		projectile->SetReplicates(true);
		projectile->SetActorEnableCollision(true);
		projectile->SetProjectileVelocity(3000.0f);
		projectile->SetProjectileDamage(100.0f);
		projectile->SetActorRotation(rot + FRotator(0.f, -70.f, 0.f));

		AActor* Shooter = GetAttachParentActor(); // Shooter = 캐릭터
		if (UTeamComponent* TeamComp = Shooter->FindComponentByClass<UTeamComponent>())
		{
			projectile->ShooterTeam = TeamComp->GetTeam(); // 발사자의 팀 정보를 저장
			projectile->WeaponFired = this;
		}

		FVector FireDirection = bulletRot.Vector();
		FireDirection.Z = 0; // Z 방향 제거
		FireDirection.Normalize();

		if (ACompetitivePlayerCharacter* OwnerCharacter = Cast<ACompetitivePlayerCharacter>(GetOwner())) {
			// 캐릭터의 공격력을 총알에 전달
			float NewDamage = OwnerCharacter->GetIncreasedDamage() * projectile->GetProjectileDamage();
			projectile->SetProjectileDamage(NewDamage);
		}

		projectile->ProjectileFire(FireDirection, GetOwner());
	}

	return true;
}