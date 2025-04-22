// Copyright 2025 ShootingStar. All Rights Reserved.

#include "CompetitivePlayerCharacter.h"
#include "Gun.h"
#include "Knife.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ResourceActor.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character_AnimInstance.h"

ACompetitivePlayerCharacter::ACompetitivePlayerCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName("Pawn");

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>Character_SKELETALMESH(TEXT("SkeletalMesh'/Game/Toon_Soldiers_UE5/Meshes/Characters_Prebuilt/SK_Soldier.SK_Soldier'"));
	if (Character_SKELETALMESH.Succeeded())
	{ // Mesh 설정
		GetMesh()->SetSkeletalMesh(Character_SKELETALMESH.Object);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Character mesh loading failed."));
	}

	GetMesh()->SetCollisionProfileName(TEXT("BodyMesh"));
	GetMesh()->SetGenerateOverlapEvents(true);
	// PickAxeMesh 생성 및 Attach
	PickAxeMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PickAxe"));
	PickAxeMesh->SetupAttachment(GetMesh(), TEXT("Backpack_Socket"));

	// 초기 트랜스폼 설정
	PickAxeMesh->SetRelativeLocation(FVector(266.293793f, 79.988396f, -53.498161f)); // 원하는 위치로 수정
	PickAxeMesh->SetRelativeRotation(FRotator(11.f, -35.f, 0.f)); // Roll, Pitch, Yaw
	PickAxeMesh->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f)); // 크기 조절

	static ConstructorHelpers::FObjectFinder<USkeletalMesh>PickAxe_SKELETALMESH(TEXT("SkeletalMesh'/Game/lowpoly-mine-assets/source/SKM_Pickaxe.SKM_Pickaxe'"));
	if (PickAxe_SKELETALMESH.Succeeded())
	{ // Mesh 설정
		PickAxeMesh->SetSkeletalMesh(PickAxe_SKELETALMESH.Object);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Pickaxe mesh loading failed."));
	}

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
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

void ACompetitivePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

}
void ACompetitivePlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AnimInstance= Cast<UCharacter_AnimInstance>(GetMesh()->GetAnimInstance());
}

float ACompetitivePlayerCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

void ACompetitivePlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ACompetitivePlayerCharacter::WeaponChange() 
{
	if (!RifleClass)
	{
		UE_LOG(LogTemp, Error, TEXT("RifleClass is nullptr! Check the Blueprint setting."));
		return;
	}

	AGun* SpawnedRifle = GetWorld() -> SpawnActor<AGun>(RifleClass);
	EquipGun(SpawnedRifle);
}
void ACompetitivePlayerCharacter::WeaponKnifeChange()
{
	if (!KnifeClass)
	{
		UE_LOG(LogTemp, Error, TEXT("KnifeClass is nullptr! Check the Blueprint setting."));
		return;
	}

	AKnife* SpawnedKnife = GetWorld()->SpawnActor<AKnife>(KnifeClass);
	EquipKnife(SpawnedKnife);
}

void ACompetitivePlayerCharacter::EquipGun(AGun* GunToEquip)
{
	AnimInstance->IsGunEquipped=true;
	AnimInstance->IsKnifeEquipped = false;
	UnEquipPickAxe();
	if (EquippedGun)
    {
		if (EquippedKnife) {
			EquippedKnife->Destroy();
			EquippedKnife = nullptr;
		}
        EquippedGun->Destroy();
        EquippedGun = nullptr;
    }

    if (GunToEquip)
    {
		EquippedGun = GunToEquip;

        if (EquippedGun)
        {	
            EquippedGun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Weapon_R_Socket"));
        }
    }
}

void ACompetitivePlayerCharacter::EquipKnife(AKnife* KnifeToEquip)
{
	AnimInstance->IsGunEquipped = false;
	AnimInstance->IsKnifeEquipped = true;
	UnEquipPickAxe();
	if (EquippedKnife)
	{
		if (EquippedGun) {
			EquippedGun->Destroy();
			EquippedGun = nullptr;
		}
		EquippedKnife->Destroy();
		EquippedKnife = nullptr;
	}

	if (KnifeToEquip)
	{
		EquippedKnife = KnifeToEquip;

		if (EquippedKnife)
		{
			EquippedKnife->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Weapon_R_Socket"));
		}
	}
}

void ACompetitivePlayerCharacter::Attack()
{
	AnimInstance->IsAttack = true;

	if (EquippedGun == nullptr && EquippedKnife == nullptr)
	{
		EquipPickAxe();
	}
	else if(EquippedGun)
	{
		PullTrigger();
	}
	else if (EquippedKnife)
	{
		AnimInstance->PlayKnifeAttackMontage();
	}
}
void ACompetitivePlayerCharacter::EquipPickAxe()
{
	if (PickAxeMesh)
	{
		PickAxeMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Weapon_R_Socket"));
	}
}

void ACompetitivePlayerCharacter::UnEquipPickAxe()
{
	if (PickAxeMesh)
	{
		PickAxeMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Backpack_Socket"));
	}
}

void ACompetitivePlayerCharacter::PullTrigger()
{
	if (EquippedGun)
	{
		// 총에서 소켓 위치와 회전 가져오기
		FVector MuzzleLoc = EquippedGun->BodyMesh->GetSocketLocation("Muzzle");
		FRotator MuzzleRot = EquippedGun->BodyMesh->GetSocketRotation("Muzzle");
		FRotator BulletFireRot = MuzzleRot; // 총구 방향 그대로 발사

		AnimInstance->PlayFireMontage();
		EquippedGun->ProjectileFire(MuzzleLoc, MuzzleRot, MuzzleRot);
	}
}
float ACompetitivePlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToApply {0.0f};
	if (!IsDead()) {
		DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		DamageToApply = FMath::Min(Health, DamageToApply);
		Health -= DamageToApply;
		UE_LOG(LogTemp, Warning, TEXT("Health left %f"), Health);

		AnimInstance->PlayHitMontage();
	}
	else {
		PlayDeadAnim();
	}

	return DamageToApply;
}
void ACompetitivePlayerCharacter::PlayDeadAnim()
{
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Capsule->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetMesh()->SetCollisionProfileName(TEXT("DeadState"));
	SetActorEnableCollision(true);

	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;

	float MontageLength = AnimInstance->DeadMontage->GetPlayLength();
	UE_LOG(LogTemp, Warning, TEXT("Dead montage length: %f"), MontageLength);

	AnimInstance->PlayDeadMontage();
	UE_LOG(LogTemp, Warning, TEXT("Character is dead!"));
	GetWorldTimerManager().SetTimer(timer, this, &ACompetitivePlayerCharacter::DestroyCharacter, MontageLength, false);
}
void ACompetitivePlayerCharacter::DestroyCharacter()
{
	Destroy();
}