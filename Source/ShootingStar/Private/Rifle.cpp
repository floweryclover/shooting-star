// Copyright 2025 ShootingStar. All Rights Reserved.


#include "Rifle.h"
#include "Rifle_Projectile.h"

ARifle::ARifle()
{
	PrimaryActorTick.bCanEverTick = false;
	BodyMesh->SetIsReplicated(true);
	BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	BodyMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>SKELETALGUN(TEXT("/Script/Engine.SkeletalMesh'/Game/Toon_Soldiers_UE5/Meshes/Weapons/SKM_weapon_assault_rifle_A.SKM_weapon_assault_rifle_A'"));
	if (SKELETALGUN.Succeeded()) {
		BodyMesh->SetSkeletalMesh(SKELETALGUN.Object);
	}

	SetWeaponeLever(WEAPONLEVER::FULLAUTO);

	WeaponName = "Rifle";
	baseKeepAmmo = 60;
	baseLoadedAmmo = 30;
	LoadedAmmo = baseLoadedAmmo;
	limitAmmo = 90;

	WeaponType = WEAPONTYPE::RIFLE;
	for (int i = 0; i < WeaponeLeverCheck.Num(); i++) WeaponeLeverCheck[i] = true;

}
void ARifle::BeginPlay()
{
	Super::BeginPlay();
}

UClass* ARifle::GetStaticClass()
{
	return ARifle::StaticClass();
}
AGun* ARifle::SpawnToHand(APawn* owner, FVector loc, FRotator rot)
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnToHand"));
	FActorSpawnParameters spawnParameters;
	spawnParameters.Owner = owner;
	spawnParameters.Instigator = GetInstigator();

	ARifle* weapon = GetWorld()->SpawnActor<ARifle>(ARifle::StaticClass(), loc, rot, spawnParameters);
	return weapon;
}

void ARifle::ProjectileFire(FVector loc, FRotator rot, FRotator bulletRot)
{
	FActorSpawnParameters spawnParameter;
	spawnParameter.Owner = GetOwner();
	//spawnParameter.Instigator = GetInstigator();

	auto projectile = GetWorld()->SpawnActor<ARifle_Projectile>(ARifle_Projectile::StaticClass(), loc, rot, spawnParameter);
	if (projectile) {
		projectile->SetProjectileVelocity(3000.0f);
		projectile->ProjectileFire(bulletRot.Vector(), GetOwner());
	}
}