// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShootingStarCharacter.h"
#include "ShootingStar/Public/Gun.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"

AShootingStarCharacter::AShootingStarCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AShootingStarCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	
	if (!GunClass)
	{
		UE_LOG(LogTemp, Error, TEXT("GunClass is nullptr! Did you forget to set it in the Blueprint?"));
		return;
	}

	Gun = GetWorld()->SpawnActor<AGun>(GunClass);
	if (Gun) 
	{
		Gun->SetOwner(this);
	}
}

bool AShootingStarCharacter::IsDead() const
{
	return Health <= 0;
}

float AShootingStarCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}


void AShootingStarCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}
void AShootingStarCharacter::PullTrigger()
{	
	if (!Gun)
	{
		UE_LOG(LogTemp, Error, TEXT("Gun is nullptr!"));
		return;
	}
	Gun->PullTrigger();
}
float AShootingStarCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToApply = FMath::Min(Health, DamageToApply);
	Health -= DamageToApply;
	UE_LOG(LogTemp, Warning, TEXT("Health left %f"), Health);

	if (IsDead())
	{
		UE_LOG(LogTemp, Warning, TEXT("Character is dead!"));
		// TODO: 죽었을 때 처리 (예: 애니메이션, 게임 오버, 리스폰 등)
		Destroy(); // 캐릭터 삭제
	}

	return DamageToApply;
}
