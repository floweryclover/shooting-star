// Copyright 2025 ShootingStar. All Rights Reserved.


#include "Shotgun_Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"

AShotgun_Projectile::AShotgun_Projectile()
{
	PrimaryActorTick.bCanEverTick = false;

	BodyMesh->SetRelativeScale3D(FVector(0.3f, 0.02f, 0.02f));

	Movement->InitialSpeed = 3000.0f;
	Movement->MaxSpeed = 100000.0f;
	Movement->bRotationFollowsVelocity = false;
	Movement->bShouldBounce = false;
	Movement->ProjectileGravityScale = 0.0f;

	OriginalDamage = 12.0f;
}

void AShotgun_Projectile::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority())
	{
		return;
	}
	
	SetLifeSpan(1000.0f);
}

void AShotgun_Projectile::OnOverlapBegin_Body(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor) || !IsValid(WeaponFired))
	{
		return;
	}
	// 발사한 무기 기준으로 데미지 계산
	const double Distance = FVector::Distance(WeaponFired->GetActorLocation(), GetActorLocation());
	if (Distance > MinimumDamageDistance)
	{
		projectileDamage = OriginalDamage * 0.1f;
	}
	else if (Distance > FullDamageDistance)
	{
		projectileDamage = FMath::Lerp(OriginalDamage, OriginalDamage * 0.1f, (Distance - FullDamageDistance) / (MinimumDamageDistance - FullDamageDistance));
	}
	else
	{
		projectileDamage = OriginalDamage;
	}
	
	Super::OnOverlapBegin_Body(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (!HasAuthority())
	{
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("ProjectileHit"));
}