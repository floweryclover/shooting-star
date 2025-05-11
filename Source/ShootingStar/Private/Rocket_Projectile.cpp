// Copyright 2025 ShootingStar. All Rights Reserved.


#include "Rocket_Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"

ARocket_Projectile::ARocket_Projectile()
{
	PrimaryActorTick.bCanEverTick = false;

	BodyMesh->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));

	Movement->InitialSpeed = 3000.0f;
	Movement->MaxSpeed = 100000.0f;
	Movement->bRotationFollowsVelocity = false;
	Movement->bShouldBounce = false;
	Movement->ProjectileGravityScale = 0.0f;

	projectileDamage = 100.0f;
}

void ARocket_Projectile::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority())
	{
		return;
	}

	SetLifeSpan(1000.0f);
}

void ARocket_Projectile::OnOverlapBegin_Body(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin_Body(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (!HasAuthority())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("ProjectileHit"));
}
