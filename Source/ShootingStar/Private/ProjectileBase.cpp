// Copyright 2025 ShootingStar. All Rights Reserved.


#include "ProjectileBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "TeamComponent.h"
#include "DrawDebugHelpers.h"
#include "Bullet_DamageType.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitialLifeSpan = 2.0f;

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESHBODY"));
	RootComponent = BodyMesh;
	static ConstructorHelpers::FObjectFinder<UStaticMesh>PROJECTILE_BODY(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cone.Cone'"));
	if (PROJECTILE_BODY.Succeeded()) {
		BodyMesh->SetStaticMesh(PROJECTILE_BODY.Object);
	}

	SetActorEnableCollision(false); // 기본적으로 끄고 서버에서만 키도록
	
	BodyMesh->SetRelativeScale3D(FVector(2.0f, 0.025f, 0.025f));
	BodyMesh->SetCollisionProfileName("Projectile");
	BodyMesh->SetGenerateOverlapEvents(true);
	BodyMesh->OnComponentBeginOverlap.AddDynamic(this, &AProjectileBase::OnOverlapBegin_Body);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SPHERECOLLISION"));
	Sphere->SetupAttachment(RootComponent);
	Sphere->SetSphereRadius(2.5f);
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MOVEMENT"));
	Movement->SetUpdatedComponent(RootComponent);
	Movement->InitialSpeed = 1800.0f;
	Movement->MaxSpeed = 100000.0f;
	Movement->bRotationFollowsVelocity = false;
	Movement->bShouldBounce = false;
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}

	SetLifeSpan(3.0f);
	UE_LOG(LogTemp, Warning, TEXT("SetLifeSpan called in BeginPlay"));

	UE_LOG(LogTemp, Warning, TEXT("Projectile BodyMesh Collision Profile: %s"),
		*BodyMesh->GetCollisionProfileName().ToString());
	UE_LOG(LogTemp, Warning, TEXT("GenerateOverlapEvents: %s"),
		BodyMesh->GetGenerateOverlapEvents() ? TEXT("TRUE") : TEXT("FALSE"));
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority())
	{
		return;
	}
}

void AProjectileBase::ProjectileFire(FVector FireDir, AActor* Onwer)
{
	if (!HasAuthority())
	{
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("ProjectileFire"));
	Movement->Velocity = FireDir * Movement->InitialSpeed;
}

void AProjectileBase::SetProjectileVelocity(float Velocity)
{
	if (!HasAuthority())
	{
		return;
	}
	
	Movement->InitialSpeed = Velocity;
}
void AProjectileBase::OnOverlapBegin_Body(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}
	if (OtherActor != GetAttachParentActor())
	{
		UTeamComponent* OtherTeamComponent = OtherActor->FindComponentByClass<UTeamComponent>();

		if (ShooterTeam != OtherTeamComponent->GetTeam())
		{
			DrawDebugSphere(GetWorld(), GetActorLocation(), 20.0f, 12, FColor::Red, false, 2.0f);

			UGameplayStatics::ApplyDamage(OtherActor, projectileDamage, Cast<APlayerController>(GetOwner()), GetOwner(), nullptr);
			//UGameplayStatics::ApplyPointDamage(OtherActor, projectileDamage, GetActorForwardVector(), SweepResult, nullptr, GetOwner(), UBullet_DamageType::StaticClass());
			UE_LOG(LogTemp, Warning, TEXT("Projectil: Player Hit"));
			//UE_LOG(LogTemp, Warning, TEXT("bulletLoc2: %f, %f, %f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);
		}
	}
	this->Destroy();
}