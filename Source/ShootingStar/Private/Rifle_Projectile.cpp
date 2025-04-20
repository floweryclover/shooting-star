// Copyright 2025 ShootingStar. All Rights Reserved.


#include "Rifle_Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"

ARifle_Projectile::ARifle_Projectile()
{
	PrimaryActorTick.bCanEverTick = false;

	BodyMesh->SetRelativeScale3D(FVector(0.3f, 0.02f, 0.02f));

	Movement->InitialSpeed = 3000.0f;
	Movement->MaxSpeed = 100000.0f;
	Movement->bRotationFollowsVelocity = false;
	Movement->bShouldBounce = false;
	Movement->ProjectileGravityScale = 0.0f;

	projectileDamage = 12.0f;
}

void ARifle_Projectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(1000.0f);
}

void ARifle_Projectile::OnOverlapBegin_Body(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin_Body(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	UE_LOG(LogTemp, Warning, TEXT("ProjectileHit"));
}