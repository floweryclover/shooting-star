// Copyright 2025 ShootingStar. All Rights Reserved.

#include "CompetitivePlayerCharacter.h"
#include "PickAxe.h"
#include "Gun.h"
#include "Knife.h"
#include "TeamComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Bullet_DamageType.h"
#include "DoT_DamageType.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "ResourceActor.h"
#include "Character_AnimInstance.h"
#include "CompetitiveGameMode.h"
#include "CompetitiveSystemComponent.h"
#include "InventoryComponent.h"
#include "SupplyActor.h"
#include "Net/UnrealNetwork.h"
#include "ShootingStar/ShootingStar.h"

ACompetitivePlayerCharacter::ACompetitivePlayerCharacter()
{
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	TeamComponent = CreateDefaultSubobject<UTeamComponent>(TEXT("TeamComponent"));
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(70.f, 96.0f);
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
	
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

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

	if (TeamComponent)
	{
		SetTeamMaterial(TeamComponent->GetTeam());
		TeamComponent->OnTeamChanged.AddDynamic(this, &ACompetitivePlayerCharacter::OnTeamChanged);
	}

	if (!HasAuthority())
	{
		return;
	}
#pragma region Server
	FActorSpawnParameters Params;
	Params.Owner = GetController();
	Params.Instigator = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	SpawnedPickAxe = GetWorld()->SpawnActor<APickAxe>(PickAxeClass, Params);
	SpawnedPickAxe->SetReplicates(true);
	SpawnedPickAxe->SetActorEnableCollision(true);
	Health = MaxHealth;
	OnRep_Health();
#pragma endregion Server
}

void ACompetitivePlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AnimInstance = Cast<UCharacter_AnimInstance>(GetMesh()->GetAnimInstance());
}

void ACompetitivePlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACompetitivePlayerCharacter, SpawnedPickAxe);
	DOREPLIFETIME(ACompetitivePlayerCharacter, EquippedGun);
	DOREPLIFETIME(ACompetitivePlayerCharacter, EquippedKnife);
	DOREPLIFETIME(ACompetitivePlayerCharacter, KnifeAttackCount);
	DOREPLIFETIME(ACompetitivePlayerCharacter, FireCount);
	DOREPLIFETIME(ACompetitivePlayerCharacter, HitCount);
	DOREPLIFETIME(ACompetitivePlayerCharacter, CurrentWeapon);
	DOREPLIFETIME(ACompetitivePlayerCharacter, PlayerName);
	DOREPLIFETIME(ACompetitivePlayerCharacter, Health);
	DOREPLIFETIME(ACompetitivePlayerCharacter, MaxHealth);
	DOREPLIFETIME(ACompetitivePlayerCharacter, MiningCount);
	DOREPLIFETIME(ACompetitivePlayerCharacter, LastInteractTime);
}

void ACompetitivePlayerCharacter::SetTeamMaterial(const ETeam Team)
{
	UMaterialInterface* TeamColor;
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

float ACompetitivePlayerCharacter::GetHealth() const
{
	return Health;
}

void ACompetitivePlayerCharacter::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const bool bIsInteracting = LastInteractTime != 0.0f && CurrentTime < LastInteractTime + InteractTimeRequired;
	if (IsValid(EquippedGun))
	{
		EquippedGun->SetActorHiddenInGame(bIsInteracting);
	}
	if (IsValid(EquippedKnife))
	{
		EquippedKnife->SetActorHiddenInGame(bIsInteracting);
	}

	if (!HasAuthority())
	{
		return;
	}
#pragma region Server
	Tick_HandleResourceInteraction(DeltaSeconds);
	Tick_HandleKnifeAttack(DeltaSeconds);
#pragma endregion Server
}

void ACompetitivePlayerCharacter::Destroyed()
{
	if (HasAuthority())
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
	}
	
	Super::Destroyed();
}

void ACompetitivePlayerCharacter::WeaponChange()
{
	FAIL_IF_NOT_SERVER();

#pragma region Server
	if (!RifleClass)
	{
		UE_LOG(LogTemp, Error, TEXT("RifleClass is nullptr! Check the Blueprint setting."));
		return;
	}

	FActorSpawnParameters Params;
	Params.Owner = GetController();
	Params.Instigator = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AGun* SpawnedRifle = GetWorld()->SpawnActor<AGun>(RifleClass, Params);
	SpawnedRifle->SetReplicates(true);
	EquipGun(SpawnedRifle);
#pragma endregion Server
}
void ACompetitivePlayerCharacter::WeaponShotgunChange()
{
	FAIL_IF_NOT_SERVER();

#pragma region Server
	if (!ShotgunClass)
	{
		UE_LOG(LogTemp, Error, TEXT("RifleClass is nullptr! Check the Blueprint setting."));
		return;
	}

	FActorSpawnParameters Params;
	Params.Owner = GetController();
	Params.Instigator = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AGun* SpawnedRifle = GetWorld()->SpawnActor<AGun>(ShotgunClass, Params);
	SpawnedRifle->SetReplicates(true);
	EquipGun(SpawnedRifle);
#pragma endregion Server
}

void ACompetitivePlayerCharacter::WeaponKnifeChange()
{
	FAIL_IF_NOT_SERVER();

#pragma region Server
	if (!KnifeClass)
	{
		UE_LOG(LogTemp, Error, TEXT("KnifeClass is nullptr! Check the Blueprint setting."));
		return;
	}

	FActorSpawnParameters Params;
	Params.Owner = GetController();
	Params.Instigator = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AKnife* SpawnedKnife = GetWorld()->SpawnActor<AKnife>(KnifeClass, Params);
	SpawnedKnife->SetReplicates(true);
	SpawnedKnife->SetActorEnableCollision(true);
	EquipKnife(SpawnedKnife);
#pragma endregion  Server
}

void ACompetitivePlayerCharacter::EquipGun(AGun* GunToEquip)
{
	FAIL_IF_NOT_SERVER();

#pragma region Server
	UE_LOG(LogTemp, Warning, TEXT("PlayerTeam: %d"), (int32)TeamComponent->GetTeam());
	UnEquipPickAxe();
	if (IsValid(EquippedKnife))
	{
		EquippedKnife->Destroy();
		EquippedKnife = nullptr;
	}
	
	if (IsValid(EquippedGun))
	{
		EquippedGun->Destroy();
	}

	EquippedGun = GunToEquip;
	OnRep_EquippedGun();
	OnRep_EquippedKnife();
#pragma endregion Server
}
void ACompetitivePlayerCharacter::EquipRocketLauncher()
{
	FAIL_IF_NOT_SERVER();

#pragma region Server
	UnEquipPickAxe();
	if (IsValid(EquippedKnife))
	{
		EquippedKnife->Destroy();
		EquippedKnife = nullptr;
	}
	if (IsValid(EquippedGun))
	{
		EquippedGun->Destroy();
	}
	FActorSpawnParameters Params;
	Params.Owner = GetController();
	Params.Instigator = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AGun* SpawnedRocketLauncher = GetWorld()->SpawnActor<AGun>(RocketLauncherClass, Params);
	SpawnedRocketLauncher->SetReplicates(true);
	SpawnedRocketLauncher->SetActorEnableCollision(true);

	EquippedGun = SpawnedRocketLauncher;
	OnRep_EquippedGun();
	OnRep_EquippedKnife();
#pragma endregion Server
}

void ACompetitivePlayerCharacter::EquipKnife(AKnife* KnifeToEquip)
{
	FAIL_IF_NOT_SERVER();

#pragma region Server
	UnEquipPickAxe();
	if (IsValid(EquippedGun))
	{
		EquippedGun->Destroy();
		EquippedGun = nullptr;
	}
	
	if (IsValid(EquippedKnife))
	{
		EquippedKnife->Destroy();
	}

	if (IsValid(KnifeToEquip))
	{
		KnifeToEquip->SetKnifeDamage(KnifeToEquip->GetKnifeDamage() * IncreasedDamage);
	}

	EquippedKnife = KnifeToEquip;
	OnRep_EquippedKnife();
	OnRep_EquippedGun();
#pragma endregion Server
}

void ACompetitivePlayerCharacter::Attack()
{
	FAIL_IF_NOT_SERVER();
	
#pragma region Server
	AnimInstance->IsAttack = true;
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const bool bCanKnifeAttack = CurrentTime > LastKnifeAttackTime + KnifeCoolTime;

	if (IsValid(EquippedGun))
	{
		PullTrigger();
	}
	else
	{
		if (!bCanKnifeAttack)
		{
			UE_LOG(LogTemp, Warning, TEXT("Knife attack is on cooldown!"));
			return; // 쿨타임이 끝나지 않으면 공격하지 않음
		}
		LastKnifeAttackTime = CurrentTime;

		if (!IsValid(EquippedKnife))
		{
			EquipPickAxe();
		}
		
		OnRep_KnifeAttackCount();
		KnifeAttackCount += 1;
	}
#pragma endregion Server
}

void ACompetitivePlayerCharacter::EquipPickAxe_Implementation()
{
	if (SpawnedPickAxe)
	{
		if (!HasAuthority())
		{
			SpawnedPickAxe->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			SpawnedPickAxe->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
								  TEXT("Weapon_R_Socket"));
		}
		FTransform RelativeTransform;
		RelativeTransform.SetRotation(FQuat(FVector(-1, 0, 0), FMath::DegreesToRadians(90.f)));

		SpawnedPickAxe->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
										  TEXT("Weapon_R_Socket"));
		SpawnedPickAxe->SetActorRelativeTransform(RelativeTransform);
	}
}

void ACompetitivePlayerCharacter::UnEquipPickAxe()
{
	if (SpawnedPickAxe)
	{
		SpawnedPickAxe->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		                                  TEXT("Backpack_Socket"));
	}
}
void ACompetitivePlayerCharacter::PlayMiningAnim()
{
	FAIL_IF_NOT_SERVER();
	
#pragma region Server
	EquipPickAxe();

	MiningCount += 1;
	OnRep_MiningCount();
#pragma endregion Server
}

void ACompetitivePlayerCharacter::PullTrigger()
{
	FAIL_IF_NOT_SERVER();

#pragma region Server
	if (EquippedGun)
	{
		// 총에서 소켓 위치와 회전 가져오기
		FVector FireLoc = EquippedGun->BodyMesh->GetSocketLocation("Muzzle");
		FRotator FireRot = GetActorRotation();
		FRotator BulletFireRot = GetActorRotation();

		EquippedGun->ProjectileFire(FireLoc, FireRot, BulletFireRot);

		FireCount += 1;
		OnRep_FireCount();
	}
#pragma endregion Server
}

float ACompetitivePlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                              class AController* EventInstigator, AActor* DamageCauser)
{
	FAIL_IF_NOT_SERVER_V(0.0f);

#pragma region Server
	if (!IsValid(EventInstigator))
	{
		UE_LOG(LogShootingStar, Error, TEXT("EventInstigator is invalid!"));
		return 0.0f;
	}
	if (!IsValid(DamageCauser))
	{
		UE_LOG(LogShootingStar, Error, TEXT("DamageCauser is invalid!"));
		return 0.0f;
	}

	ACompetitivePlayerCharacter* InstigatorCharacter = Cast<ACompetitivePlayerCharacter>(EventInstigator->GetPawn());
	if (IsValid(InstigatorCharacter) && InstigatorCharacter->GetTeamComponent()->GetTeam() == this->GetTeamComponent()->GetTeam())
		return 0.0f;

	float DamageToApply{0.0f};

	const bool bWasAlreadyDead = IsDead();

	DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToApply = FMath::Min(Health, DamageToApply);

	if (DamageEvent.DamageTypeClass && DamageEvent.DamageTypeClass->IsChildOf(UDoT_DamageType::StaticClass()))
	{
		Health -= DamageToApply * 100 / Armor;
		OnRep_Health();
		ApplyDoTDamage(EventInstigator, DamageCauser);
	}
	else
	{
		// 데미지 타입이 명시되지 않았을 경우 기본 처리
		Health -= DamageToApply * 100 / Armor;
		OnRep_Health();
	}
	HitCount += 1;
	OnRep_HitCount();

	if (!bWasAlreadyDead && IsDead())
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

		UE_LOG(LogTemp, Warning, TEXT("Character is dead!"));
		GetWorldTimerManager().SetTimer(Timer, this, &ACompetitivePlayerCharacter::DestroyCharacter, DeadTime, false);
	
		OnKilled.Broadcast(EventInstigator, GetController());
	}

	return DamageToApply;
#pragma endregion Server
}

void ACompetitivePlayerCharacter::ApplyDoTDamage(AController* InInstigator, AActor* InCauser)
{
	FAIL_IF_NOT_SERVER();

#pragma region Server
	if (GetWorldTimerManager().IsTimerActive(DoTTimerHandle))
		return; // 이미 적용 중이면 무시하

	DoTInstigator = InInstigator;
	DoTCauser = InCauser;
	GetWorldTimerManager().SetTimer(DoTTimerHandle, this, &ACompetitivePlayerCharacter::ApplyDoTTick, 1.0f, true, 0.0f);
#pragma endregion Server
}

void ACompetitivePlayerCharacter::ApplyDoTTick()
{
	FAIL_IF_NOT_SERVER();
	
#pragma region Server
	const float TickDamage = 5.0f;
	const float TotalDuration = 5.0f;
	const float TickInterval = 1.0f;

	if (IsDead())
	{
		GetWorldTimerManager().ClearTimer(DoTTimerHandle);
		return;
	}

	UGameplayStatics::ApplyDamage(this, TickDamage, DoTInstigator, DoTCauser, UDoT_DamageType::StaticClass());
	CurrentDoTTime += TickInterval;
	if (CurrentDoTTime >= TotalDuration)
	{
		GetWorldTimerManager().ClearTimer(DoTTimerHandle);
		CurrentDoTTime = 0.0f;
	}
#pragma endregion Server
}

void ACompetitivePlayerCharacter::DestroyCharacter()
{
	FAIL_IF_NOT_SERVER();

#pragma region Server
	Destroy();
#pragma endregion Server
}

void ACompetitivePlayerCharacter::DashStart()
{
	FAIL_IF_NOT_SERVER();

#pragma region Server
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
#pragma endregion Server
}

void ACompetitivePlayerCharacter::DashEnd()
{
	FAIL_IF_NOT_SERVER();
	
#pragma region Server
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->BrakingFrictionFactor = 2.f;
#pragma endregion Server
}

void ACompetitivePlayerCharacter::SetWeaponData(const FWeaponData& NewWeaponData)
{
	FAIL_IF_NOT_SERVER();

#pragma region Server
	CurrentWeapon = NewWeaponData;
	OnRep_CurrentWeapon();
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	MaxHealth = 100;
	Health = FMath::Min(Health, MaxHealth);
	IncreasedDamage = 1;
	DamageTypeClass = UBullet_DamageType::StaticClass();
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
					GetCharacterMovement()->MaxWalkSpeed *= FMath::Pow(1.1f, UsedCount);
					UE_LOG(LogTemp, Log, TEXT("Wood used, speed increased by 10%% per count. New MaxWalkSpeed: %f"),
					       GetCharacterMovement()->MaxWalkSpeed);
					break;
				case EResourceType::Stone:
					MaxHealth *= FMath::Pow(1.1f, UsedCount);
					Health *= FMath::Pow(1.1f, UsedCount);
					UE_LOG(LogTemp, Log, TEXT("Stone used, defense increased by 10%% per count. New defense: %f"),
					       Health);
					break;
				case EResourceType::Iron:
					IncreasedDamage *= FMath::Pow(1.2f, UsedCount);
					break;
				case EResourceType::Uranium:
					DamageTypeClass = UDoT_DamageType::StaticClass();
					break;
				default:
					break;
				}
			}
		}
	}
	if (WeaponNameStr == TEXT("AK"))
	{
		WeaponChange();
	}
	else if (WeaponNameStr == TEXT("Knife"))
	{
		// 칼 장착 로직
		WeaponKnifeChange();
	}
	else if (WeaponNameStr == TEXT("Shotgun"))
	{
		WeaponShotgunChange();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("알 수 없는 무기입니다: %s"), *WeaponNameStr);
	}
#pragma endregion Server
}

void ACompetitivePlayerCharacter::CraftWeapon_Implementation(const FWeaponData& SelectWeapon,
	const TArray<int32>& ClickedResources)
{
#pragma region Server
	// 실제로 자원을 가졌는지 검증
	const TArray<FResourceInventoryData>& ResourcesHave = InventoryComponent->GetAllResources();
	for (int i = 0; i < static_cast<int>(EResourceType::End); ++i)
	{
		if (ClickedResources[i] <= 0)
		{
			continue;
		}

		if (!ResourcesHave[i].Resource)
		{
			return;
		}

		if (ResourcesHave[i].Count < ClickedResources[i])
		{
			return;
		}
	}

	const FWeaponData CraftedWeapon = InventoryComponent->Craft_Weapon(SelectWeapon, ClickedResources);
	SetWeaponData(CraftedWeapon);
#pragma endregion Server
}

void ACompetitivePlayerCharacter::SetInBush(bool bIsInBush)
{
	FAIL_IF_NOT_SERVER();
}

FWeaponData ACompetitivePlayerCharacter::GetWeaponData()
{
	return CurrentWeapon;
}

void ACompetitivePlayerCharacter::InteractResource()
{
	FAIL_IF_NOT_SERVER();
	
#pragma region Server
	ACompetitiveGameMode* const GameMode = Cast<ACompetitiveGameMode>(GetWorld()->GetAuthGameMode());
	UCompetitiveSystemComponent* const CompetitiveSystemComponent = GameMode->GetCompetitiveSystemComponent();
	if (CompetitiveSystemComponent->GetCurrentPhase() != ECompetitiveGamePhase::Game)
	{
		return;
	}
	
	FVector Start = GetActorLocation();
	Start.Z = 0.f;
	FRotator Rotation = GetActorRotation();
	FVector End = Start + Rotation.Vector() * 130.f;
	FHitResult Hit;
	if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, CollisionChannels::ResourceActor))
	{
		return;
	}

	// Supply 태그 확인
	if (ASupplyActor* SupplyActor = Cast<ASupplyActor>(Hit.GetActor()); Hit.GetActor()->ActorHasTag("Supply") && IsValid(SupplyActor))
	{
		// 보급품 상자가 이미 열려있는지 확인
		if (!SupplyActor->IsOpened())
		{
			// 무기 데이터 설정 및 장착
			SetWeaponData(SupplyActor->GetStoredWeapon());
			EquipRocketLauncher();
			SupplyActor->PlayOpeningAnimation();
		}
		return;
	}
	
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime < LastInteractTime + InteractTimeRequired)
	{
		return;
	}
	LastInteractTime = CurrentTime;
	PlayMiningAnim();
#pragma endregion Server;
}

void ACompetitivePlayerCharacter::SetPlayerName(const FString& Name)
{
	FAIL_IF_NOT_SERVER();

#pragma region Server
	PlayerName = Name;
	OnRep_PlayerName();
#pragma endregion Server
}

void ACompetitivePlayerCharacter::OnRep_Health()
{
	if (Health <= 0.0f)
	{
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false;
		
		AnimInstance->PlayDeadMontage(DeadTime);
	}
}

void ACompetitivePlayerCharacter::OnRep_EquippedGun()
{
	if (IsValid(EquippedGun))
	{
		if (!HasAuthority())
		{
			EquippedGun->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		}
		EquippedGun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
							TEXT("Weapon_R_Socket"));
		EquippedGun->SetDamageType(DamageTypeClass);
	}
	RefreshAnimInstance();
}

void ACompetitivePlayerCharacter::OnRep_EquippedKnife()
{
	if (IsValid(EquippedKnife))
	{
		if (!HasAuthority())
		{
			EquippedKnife->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		}
		EquippedKnife->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
					TEXT("Weapon_R_Socket"));
	}

	RefreshAnimInstance();
}

void ACompetitivePlayerCharacter::OnRep_FireCount()
{
	if (!IsValid(EquippedGun))
	{
		return;
	}
	
	if (EquippedGun->IsA(RocketLauncherClass))
	{
		AnimInstance->PlayRocketFireMontage();
	}
	else 
	{
		AnimInstance->PlayFireMontage();
	}
}

void ACompetitivePlayerCharacter::OnRep_KnifeAttackCount()
{
	AnimInstance->PlayKnifeAttackMontage();
}

void ACompetitivePlayerCharacter::OnRep_HitCount()
{
	AnimInstance->PlayHitMontage();
}

void ACompetitivePlayerCharacter::OnRep_CurrentWeapon()
{
	OnWeaponChanged.Broadcast(CurrentWeapon);
}

void ACompetitivePlayerCharacter::OnRep_PlayerName()
{
	OnPlayerNameChanged.Broadcast(PlayerName);
}

void ACompetitivePlayerCharacter::OnRep_MiningCount()
{
	AnimInstance->PlayMiningMontage(InteractTimeRequired, 4);
}

void ACompetitivePlayerCharacter::OnTeamChanged(const ETeam Team)
{
	SetTeamMaterial(Team);
}

void ACompetitivePlayerCharacter::RefreshAnimInstance()
{
	AnimInstance->IsKnifeEquipped = IsValid(EquippedKnife);
	AnimInstance->IsGunEquipped = IsValid(EquippedGun);
	AnimInstance->IsRockLauncherEquipped = IsValid(EquippedGun) && EquippedGun->IsA(RocketLauncherClass);
}

void ACompetitivePlayerCharacter::Tick_HandleResourceInteraction(const float DeltaSeconds)
{
	if (IsDead() || LastInteractTime == 0.0f)
	{
		LastInteractTime = 0.0f;
		return;
	}
	
	ACompetitiveGameMode* const GameMode = Cast<ACompetitiveGameMode>(GetWorld()->GetAuthGameMode());
	UCompetitiveSystemComponent* const CompetitiveSystemComponent = GameMode->GetCompetitiveSystemComponent();
	if (CompetitiveSystemComponent->GetCurrentPhase() != ECompetitiveGamePhase::Game)
	{
		return;
	}
	
	FVector Start = GetActorLocation();
	Start.Z = 0.f;
	FRotator Rotation = GetActorRotation();

	FVector End = Start + Rotation.Vector() * 130.f;
	
	FHitResult Hit;
	if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, CollisionChannels::ResourceActor))
	{
		// 캐고 있는데 자원 사라짐
		LastInteractTime = 0.0f;
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime < LastInteractTime + InteractTimeRequired) // 아직 자원 채집 완료되지 않음
	{
		return;
	}
	LastInteractTime = 0.0f;

	AResourceActor* const Resource = Cast<AResourceActor>(Hit.GetActor());
	if (Resource)
	{
		InventoryComponent->AddResource(Resource->ResourceData);
		Resource->UpdateMesh_AfterHarvest();
	}
}

void ACompetitivePlayerCharacter::Tick_HandleKnifeAttack(const float DeltaSeconds)
{
	if (IsDead())
	{
		LastKnifeAttackTime = 0.0f;
		return;
	}
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const bool bIsKnifeAttacking = CurrentTime < LastKnifeAttackTime + KnifeCoolTime;

	if (bIsKnifeAttacking)
	{
		if (IsValid(EquippedKnife))
		{
			EquippedKnife->AttackHitBox->SetGenerateOverlapEvents(true);
		}
		if (IsValid(SpawnedPickAxe))
		{
			SpawnedPickAxe->SetOwner(GetController());
			SpawnedPickAxe->AttackHitBox->SetGenerateOverlapEvents(true);
		}
	}
	else
	{
		if (IsValid(EquippedKnife))
		{
			EquippedKnife->AttackHitBox->SetGenerateOverlapEvents(false);
			EquippedKnife->ResetDamageableFlag();
		}
		if (IsValid(SpawnedPickAxe))
		{
			SpawnedPickAxe->AttackHitBox->SetGenerateOverlapEvents(false);
			SpawnedPickAxe->ResetDamageableFlag();
		}
	}
}
