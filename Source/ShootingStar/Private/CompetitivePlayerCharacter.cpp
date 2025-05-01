// Copyright 2025 ShootingStar. All Rights Reserved.

#include "CompetitivePlayerCharacter.h"
#include "PickAxe.h"
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
#include "ClientComponent.h"
#include "CompetitiveGameMode.h"
#include "CompetitivePlayerController.h"
#include "TeamComponent.h"
#include "Net/UnrealNetwork.h"
#include "ShootingStar/ShootingStar.h"

ACompetitivePlayerCharacter::ACompetitivePlayerCharacter()
{
	TeamComponent = CreateDefaultSubobject<UTeamComponent>(TEXT("TeamComponent"));
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName("Pawn");

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> Character_SKELETALMESH(
		TEXT("SkeletalMesh'/Game/Toon_Soldiers_UE5/Meshes/Characters_Prebuilt/SK_Soldier.SK_Soldier'"));
	if (Character_SKELETALMESH.Succeeded())
	{
		// Mesh 설정
		GetMesh()->SetSkeletalMesh(Character_SKELETALMESH.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Character mesh loading failed."));
	}

	GetMesh()->SetCollisionProfileName(TEXT("BodyMesh"));
	GetMesh()->SetGenerateOverlapEvents(true);

	// Don't rotate character to camera direction
	// bUseControllerRotationPitch = false;
	// bUseControllerRotationYaw = false;
	// bUseControllerRotationRoll = false;

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

	SpawnPickAxe();

	Health = MaxHealth;
	if (TeamComponent)
	{
		PlayerTeam = TeamComponent->GetTeam();
		SetTeamMaterial(PlayerTeam);
	}
}

void ACompetitivePlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AnimInstance = Cast<UCharacter_AnimInstance>(GetMesh()->GetAnimInstance());
}

void ACompetitivePlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACompetitivePlayerCharacter, EquippedGun);
	DOREPLIFETIME(ACompetitivePlayerCharacter, EquippedKnife);
	DOREPLIFETIME(ACompetitivePlayerCharacter, KnifeAttackCount);
	DOREPLIFETIME(ACompetitivePlayerCharacter, FireCount);
	DOREPLIFETIME(ACompetitivePlayerCharacter, HitCount);
	DOREPLIFETIME(ACompetitivePlayerCharacter, bDeadNotify);
	DOREPLIFETIME(ACompetitivePlayerCharacter, CurrentWeapon);
}
void ACompetitivePlayerCharacter::SetTeamMaterial(ETeam Team)
{
	switch (Team)
	{
	case ETeam::Blue:
		TeamColor = BlueTeamMaterial;
		break;
	case ETeam::Red:
		TeamColor = RedTeamMaterial;
		break;
	default:
		TeamColor = DefaultMaterial;
		break;
	}
	if (TeamColor)
	{
		GetMesh()->SetMaterial(0, TeamColor);
	}
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

	AGun* SpawnedRifle = GetWorld()->SpawnActor<AGun>(RifleClass);
	SpawnedRifle->SetReplicates(true);
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
	SpawnedKnife->SetReplicates(true);
	SpawnedKnife->SetActorEnableCollision(true);
	EquipKnife(SpawnedKnife);
}

void ACompetitivePlayerCharacter::EquipGun(AGun* GunToEquip)
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerTeam: %d"), (int32)PlayerTeam);
	UnEquipPickAxe();
	if (EquippedGun)
	{
		if (EquippedKnife)
		{
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
			EquippedGun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			                               TEXT("Weapon_R_Socket"));
		}
	}
	ForceNetUpdate();
	RefreshAnimInstance();
}

void ACompetitivePlayerCharacter::EquipKnife(AKnife* KnifeToEquip)
{
	UnEquipPickAxe();
	if (EquippedKnife)
	{
		if (EquippedGun)
		{
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
			EquippedKnife->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			                                 TEXT("Weapon_R_Socket"));
			EquippedKnife->SetknifeDamage(EquippedKnife->GetknifeDamage() * IncreasedDamage);
		}
	}

	ForceNetUpdate();
	RefreshAnimInstance();
}

void ACompetitivePlayerCharacter::Attack()
{
	AnimInstance->IsAttack = true;

	if (EquippedGun == nullptr && EquippedKnife == nullptr)
	{
		EquipPickAxe();
		OnRep_KnifeAttackCount();
		KnifeAttackCount += 1;
	}
	else if (EquippedGun)
	{
		PullTrigger();
	}
	else if (EquippedKnife)
	{
		if (!bCanKnifeAttack)
		{
			UE_LOG(LogTemp, Warning, TEXT("Knife attack is on cooldown!"));
			return; // 쿨타임이 끝나지 않으면 공격하지 않음
		}
		bCanKnifeAttack = false;
		GetWorld()->GetTimerManager().SetTimer(KnifeAttackCoolDownTimer, this,
		                                       &ACompetitivePlayerCharacter::ResetKnifeAttackCooldown,
		                                       KnifeAttackCooldown, false);
		OnRep_KnifeAttackCount();
		KnifeAttackCount += 1;
	}
}

void ACompetitivePlayerCharacter::SpawnPickAxe()
{
	SpawnedPickAxe = GetWorld()->SpawnActor<APickAxe>(PickAxeClass);
	SpawnedPickAxe->SetReplicates(true);

	FVector NewLocationOffset(-250.293793f / 2, -70.988396f / 2, 56.498161f / 2);
	SpawnedPickAxe->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
	                                  TEXT("Backpack_Socket"));
	SpawnedPickAxe->SetActorRelativeLocation(NewLocationOffset);
}

void ACompetitivePlayerCharacter::EquipPickAxe()
{
	if (SpawnedPickAxe)
	{
		SpawnedPickAxe->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		                                  TEXT("Weapon_R_Socket"));
	}
	else
	{
		// If the pickaxe hasn't been spawned yet, spawn it and then attach it.
		if (PickAxeClass)
		{
			SpawnedPickAxe = GetWorld()->SpawnActor<APickAxe>(PickAxeClass);
			if (SpawnedPickAxe)
			{
				FVector NewLocationOffset(-250.293793f / 2, -70.988396f / 2, 56.498161f / 2);
				SpawnedPickAxe->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				                                  TEXT("Weapon_R_Socket"));
				SpawnedPickAxe->SetActorRelativeLocation(NewLocationOffset);
			}
		}
	}
}

void ACompetitivePlayerCharacter::UnEquipPickAxe()
{
	if (SpawnedPickAxe)
	{
		SpawnedPickAxe->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		                                  TEXT("Backpack_Socket"));
	}
	else
	{
		// If the pickaxe hasn't been spawned yet, spawn it and then attach it.
		if (PickAxeClass)
		{
			SpawnedPickAxe = GetWorld()->SpawnActor<APickAxe>(PickAxeClass);
			if (SpawnedPickAxe)
			{
				FVector NewLocationOffset(-250.293793f / 2, -70.988396f / 2, 56.498161f / 2);
				SpawnedPickAxe->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
				                                  TEXT("Backpack_Socket"));
				SpawnedPickAxe->SetActorRelativeLocation(NewLocationOffset);
			}
		}
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

		EquippedGun->ProjectileFire(MuzzleLoc, MuzzleRot, MuzzleRot);

		FireCount += 1;
		OnRep_FireCount();
	}
}

float ACompetitivePlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                              class AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToApply{0.0f};

	DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToApply = FMath::Min(Health, DamageToApply);
	Health -= DamageToApply * 100 / Armor;
	UE_LOG(LogTemp, Warning, TEXT("Health left %f"), Health);
	HitCount += 1;
	OnRep_HitCount();
	
	if (IsDead()) {
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

	bDeadNotify = true;
	OnRep_bDeadNotify();

	UE_LOG(LogTemp, Warning, TEXT("Character is dead!"));
	GetWorldTimerManager().SetTimer(timer, this, &ACompetitivePlayerCharacter::DestroyCharacter, MontageLength, false);
}

void ACompetitivePlayerCharacter::DestroyCharacter()
{
	if (SpawnedPickAxe) 
	{
		SpawnedPickAxe->Destroy();
	}
	if (EquippedGun)
	{
		EquippedGun->Destroy();
	}
	if (EquippedKnife)
	{
		EquippedKnife->Destroy();
	}
	Cast<ACompetitiveGameMode>(GetWorld()->GetAuthGameMode())->RespawnPlayer(GetController());
	Destroy();
}

void ACompetitivePlayerCharacter::DashStart()
{
	FVector Velocity = GetVelocity();

	FVector DashDirection = FVector(Velocity.X, Velocity.Y, 0.f);

	// 움직이고 있는 방향이 있다면, 그 방향으로 대쉬
	if (DashDirection.SizeSquared() > KINDA_SMALL_NUMBER)
	{
		DashDirection.Normalize();
	}
	else
	{
		// 움직이고 있지 않다면, 캐릭터 앞 방향으로 대쉬
		DashDirection = GetActorForwardVector();
	}

	GetCharacterMovement()->BrakingFrictionFactor = 0.f;
	LaunchCharacter(DashDirection * 5000.f, true, true);
	GetWorldTimerManager().SetTimer(DashTimer, this, &ACompetitivePlayerCharacter::DashEnd, 0.1f, false);
}

void ACompetitivePlayerCharacter::DashEnd()
{
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->BrakingFrictionFactor = 2.f;
}

void ACompetitivePlayerCharacter::SetWeaponData(const FWeaponData& NewWeaponData)
{
	CurrentWeapon = NewWeaponData;
	OnRep_CurrentWeapon();

	FString WeaponNameStr = CurrentWeapon.WeaponName.ToString();

	for (uint8 i = 0; i < static_cast<uint8>(EResourceType::End); ++i)
	{
		// 현재 리소스 타입
		EResourceType ResourceType = static_cast<EResourceType>(i);

		// 유효한 인덱스 체크
		if (CurrentWeapon.UsedResourceCounts.IsValidIndex(i))
		{
			uint8 UsedCount = CurrentWeapon.UsedResourceCounts[i];

			// UsedCount를 이용해 능력치를 수정
			if (UsedCount > 0)
			{
				// 예시: 리소스당 공격력 +2
				switch (ResourceType)
				{
				case EResourceType::Wood:
					GetCharacterMovement()->MaxWalkSpeed *= 1.1f * UsedCount;
					UE_LOG(LogTemp, Log, TEXT("Wood used, speed increased by 10%% per count. New MaxWalkSpeed: %f"),
					       GetCharacterMovement()->MaxWalkSpeed);
					break;
				case EResourceType::Stone:
					Armor *= 1.1f * UsedCount;
					UE_LOG(LogTemp, Log, TEXT("Stone used, defense increased by 10%% per count. New defense: %f"),
					       Armor);
					break;
				case EResourceType::Iron:
					IncreasedDamage *= 1.2f * UsedCount;
					break;
				case EResourceType::Uranium:
					break;
				default:
					break;
				}
			}
		}
	}
	if (WeaponNameStr == TEXT("총"))
	{
		WeaponChange();
	}
	else if (WeaponNameStr == TEXT("칼"))
	{
		// 칼 장착 로직
		WeaponKnifeChange();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("알 수 없는 무기입니다: %s"), *WeaponNameStr);
	}
}

FWeaponData ACompetitivePlayerCharacter::GetWeaponData()
{
	return CurrentWeapon;
}

void ACompetitivePlayerCharacter::KnifeAttackStart()
{
	if (EquippedKnife)
	{
		EquippedKnife->AttackHitBox->SetGenerateOverlapEvents(true);
	}
}

void ACompetitivePlayerCharacter::KnifeAttackEnd()
{
	if (EquippedKnife)
	{
		EquippedKnife->AttackHitBox->SetGenerateOverlapEvents(false);
	}
}

void ACompetitivePlayerCharacter::ResetKnifeAttackCooldown()
{
	bCanKnifeAttack = true;
	UE_LOG(LogTemp, Log, TEXT("Knife attack cooldown reset."));
}

void ACompetitivePlayerCharacter::OnRep_EquippedGun()
{
	RefreshAnimInstance();
}

void ACompetitivePlayerCharacter::OnRep_EquippedKnife()
{
	RefreshAnimInstance();
}

void ACompetitivePlayerCharacter::OnRep_FireCount()
{
	AnimInstance->PlayFireMontage();
}

void ACompetitivePlayerCharacter::OnRep_KnifeAttackCount()
{
	AnimInstance->PlayKnifeAttackMontage();
}

void ACompetitivePlayerCharacter::OnRep_HitCount()
{
	AnimInstance->PlayHitMontage();
}

void ACompetitivePlayerCharacter::OnRep_bDeadNotify()
{
	AnimInstance->PlayDeadMontage();
}

void ACompetitivePlayerCharacter::OnRep_CurrentWeapon()
{
	OnWeaponChanged.Broadcast(CurrentWeapon);
}

void ACompetitivePlayerCharacter::RefreshAnimInstance()
{
	AnimInstance->IsKnifeEquipped = IsValid(EquippedKnife);
	AnimInstance->IsGunEquipped = IsValid(EquippedGun);
}