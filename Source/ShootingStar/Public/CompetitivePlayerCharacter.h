// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponData.h"
#include "ShootingStar/ShootingStar.h"
#include "CompetitivePlayerCharacter.generated.h"

class UInventoryComponent;
class UCharacter_AnimInstance;
class ACompetitivePlayerController;
class AGun;
class AKnife;
class APickAxe;
class UWidgetComponent;
class UTeamComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponChanged, FWeaponData, WeaponData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerNameChanged, const FString&, PlayerName);

UCLASS(Blueprintable)
class SHOOTINGSTAR_API ACompetitivePlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	//
	// 캐릭터 상수들
	//

	// 자원 캐는 시간.
	constexpr static float InteractTimeRequired = 3.0f;

	// 리스폰 시간.
	constexpr static float DeadTime = 3.0f;

	// 근접 무기 공격 쿨타임.
	constexpr static float KnifeCoolTime = 1.0f;

	// 자원 캡슐 트레이스 상수들.
	constexpr static float RadiusCapsule = 60.0f;
	constexpr static float HalfHeightCapsule = 20.0f;
	const ETraceTypeQuery TraceTypeResource = UEngineTypes::ConvertToTraceType(CollisionChannels::ResourceActor);
	
	ACompetitivePlayerCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	virtual void Destroyed() override;

	virtual float TakeDamage(float DamageAmount,
	                         FDamageEvent const& DamageEvent,
	                         AController* EventInstigator,
	                         AActor* DamageCauser) override;
	
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UPROPERTY(BlueprintAssignable)
	FPlayerNameChanged OnPlayerNameChanged;

	UPROPERTY(BlueprintAssignable)
	FWeaponChanged OnWeaponChanged;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FWeaponData GetWeaponData();
	
	FORCEINLINE bool IsDead() const { return Health == 0; }

	void DashStart();
	void EquipGun(AGun* Equip);
	void EquipKnife(AKnife* Equip);
	void EquipRocketLauncher();
	void Attack();
	void WeaponChange();
	void WeaponShotgunChange();
	void WeaponKnifeChange();

	UFUNCTION(BlueprintCallable, Reliable, Server)
	void CraftWeapon(const FWeaponData& SelectWeapon, const TArray<int32>& ClickedResources);

	void InteractResource();

	void IncreaseBushCount()
	{
		FAIL_IF_NOT_SERVER();

#pragma region Server
		BushCount += 1;
		SetActorHiddenInGame(true);
#pragma endregion Server
	}

	void DecreaseBushCount()
	{
		FAIL_IF_NOT_SERVER();

#pragma region Server
		BushCount -= 1;
		if (BushCount < 0)
		{
			BushCount = 0;
		}

		if (BushCount == 0)
		{
			SetActorHiddenInGame(false);
		}
#pragma endregion Server
	}

	//
	// Getter, Setter
	//

	/**
	 * 이동 및 회전이 가능한지 반환.
	 * @return
	 */
	UFUNCTION(BlueprintCallable)
	bool IsMovable() const
	{
		if (IsDead())
		{
			return false;
		}

		const float CurrentTime = GetWorld()->GetTimeSeconds();
		return LastInteractTime == 0.0f || CurrentTime > LastInteractTime + InteractTimeRequired;
	}

	UFUNCTION(BlueprintCallable)
	void SetPlayerName(const FString& Name);

	UCharacter_AnimInstance* GetAnimInstance() const
	{
		return AnimInstance;
	}

	UInventoryComponent* GetInventoryComponent() const
	{
		return InventoryComponent;
	}

	UTeamComponent* GetTeamComponent() const
	{
		return TeamComponent;
	}

	float GetIncreasedDamage() const
	{
		return IncreasedDamage;
	}

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	FTimerHandle Timer;

	UPROPERTY(BlueprintReadOnly)
	int32 BushCount; // 들어가있는 부쉬의 개수. 겹쳐있는 경우가 있어서 bool 대신 int 사용.
	
	UPROPERTY(Replicated)
	float MaxHealth = 100;

	UPROPERTY(ReplicatedUsing=OnRep_Health, VisibleAnywhere)
	float Health;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<AGun> GunClass;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<AGun> RifleClass;
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<AGun> ShotgunClass;
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<AGun> RocketLauncherClass;
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<AKnife> KnifeClass;
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<APickAxe> PickAxeClass;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_SpawnedPickAxe)
	TObjectPtr<APickAxe> SpawnedPickAxe;
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing=OnRep_EquippedGun)
	TObjectPtr<AGun> EquippedGun;
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing=OnRep_EquippedKnife)
	TObjectPtr<AKnife> EquippedKnife;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<AGun> Gun;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<AKnife> Knife;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Direction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UArrowComponent> FacingArrow;

	UPROPERTY()
	TObjectPtr<UCharacter_AnimInstance> AnimInstance;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UTeamComponent> TeamComponent;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UChildActorComponent> NameTagActorComponent;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_PlayerName)
	FString PlayerName;

	//
	// 팀 관련
	//

	UPROPERTY(EditDefaultsOnly, Category = "Team")
	UMaterialInterface* RedTeamMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Team")
	UMaterialInterface* BlueTeamMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Team")
	UMaterialInterface* DefaultMaterial;

	//
	// 자원 관련
	//

	UPROPERTY(ReplicatedUsing=OnRep_LastInteractTime, BlueprintReadOnly)
	float LastInteractTime = 0.0f;

	//
	// 대쉬 관련
	//

	FTimerHandle DashTimer;
	void DashEnd();

	//
	// 무기 관련
	//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_CurrentWeapon, Category = "Weapon")
	FWeaponData CurrentWeapon{};

	UPROPERTY(EditDefaultsOnly)
	float Armor = 100;

	UPROPERTY(EditDefaultsOnly)
	float IncreasedDamage = 1;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetWeaponData(const FWeaponData& NewWeaponData);

	//
	// 전투 관련
	//

	UPROPERTY()
	TSubclassOf<UDamageType> DamageTypeClass;
	
	FTimerHandle DoTTimerHandle;

	UPROPERTY()
	TObjectPtr<AController> DoTInstigator = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> DoTCauser = nullptr;

	float CurrentDoTTime = 0.0f;

	float LastKnifeAttackTime = 0.0f;
	
private:
	//
	// 전투 관련
	//
	
	void ApplyDoTDamage(AController* InInstigator, AActor* InCauser);
	void ApplyDoTTick();
	UFUNCTION(Reliable, NetMulticast)
	void EquipPickAxe();
	UFUNCTION(Reliable, NetMulticast)
	void UnEquipPickAxe();
	void PlayMiningAnim();
	void PullTrigger();
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealthPercent() const;
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealth() const;
	void DestroyCharacter();

	//
	// Replication Notifies
	// 애니메이션 동기화용 카운트. 값에 의미 없음 - 변화를 감지해서 클라이언트에서 애니메이션 출력 등을 위함임
	//

	UPROPERTY(ReplicatedUsing=OnRep_FireCount)
	int32 FireCount;

	UPROPERTY(ReplicatedUsing=OnRep_KnifeAttackCount)
	int32 KnifeAttackCount;

	UPROPERTY(ReplicatedUsing=OnRep_HitCount)
	int32 HitCount;

	UPROPERTY(ReplicatedUsing=OnRep_MiningCount)
	int32 MiningCount;

	UFUNCTION()
	void OnRep_Health();

	UFUNCTION()
	void OnRep_EquippedGun();

	UFUNCTION()
	void OnRep_EquippedKnife();
	
	UFUNCTION()
	void OnRep_FireCount();

	UFUNCTION()
	void OnRep_KnifeAttackCount();

	UFUNCTION()
	void OnRep_HitCount();

	UFUNCTION()
	void OnRep_CurrentWeapon();

	UFUNCTION()
	void OnRep_PlayerName();

	UFUNCTION()
	void OnRep_MiningCount();

	UFUNCTION()
	void OnRep_LastInteractTime();

	UFUNCTION()
	void OnRep_SpawnedPickAxe();
	
	UFUNCTION()
	void OnTeamChanged(ETeam Team);

	UFUNCTION()
	void OnMiningAnimationHit();
	
	void RefreshAnimInstance();

	void SetTeamMaterial(ETeam Team);

	void Tick_HandleResourceInteraction(float DeltaSeconds);

	void Tick_HandleKnifeAttack(float DeltaSeconds);

	void Tick_HandleHidden(float DeltaSeconds);

	bool CapsuleTraceResource(FHitResult& OutHitResult);
};
