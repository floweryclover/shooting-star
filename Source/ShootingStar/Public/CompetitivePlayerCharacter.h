// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "WeaponData.h"
#include "ResourceType.h"
#include "CompetitivePlayerCharacter.generated.h"

class UTeamComponent;
class UCharacter_AnimInstance;
class AGun;
class AKnife;
class UInventoryComponent;
class APickAxe;

enum class ETeam :uint8;
struct FWeaponData;
enum class EResourceType :uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponChanged, FWeaponData, WeaponData);

UCLASS(Blueprintable)
class SHOOTINGSTAR_API ACompetitivePlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACompetitivePlayerCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UFUNCTION(BlueprintCallable)
	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	UPROPERTY(EditDefaultsOnly)
	TArray<AGun*> WeaponList;

	// 팀 관련
	ETeam PlayerTeam;
	UFUNCTION()
	void SetTeamMaterial(ETeam Team);
	UMaterialInterface* TeamColor = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Team")
	UMaterialInterface* RedTeamMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Team")
	UMaterialInterface* BlueTeamMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Team")
	UMaterialInterface* DefaultMaterial;
	//대쉬 관련
	FTimerHandle DashTimer;
	void DashStart();
	void DashEnd();

	//무기 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_CurrentWeapon, Category = "Weapon")
	FWeaponData CurrentWeapon{};

	UPROPERTY(EditDefaultsOnly)
	float Armor = 100;

	UPROPERTY(EditDefaultsOnly)
	float IncreasedDamage = 1;

	UPROPERTY(BlueprintAssignable)
	FWeaponChanged OnWeaponChanged;
	
	FTimerHandle KnifeAttackCoolDownTimer;
	float KnifeAttackCooldown = 1.0f;
	bool bCanKnifeAttack = true;
	void ResetKnifeAttackCooldown();

	// 외부에서 무기 데이터를 세팅하는 함수
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetWeaponData(const FWeaponData& NewWeaponData);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FWeaponData GetWeaponData();

	UFUNCTION()
	FORCEINLINE void AddWeaponToList(AGun* Weapon) { WeaponList.Add(Weapon); }

	virtual float TakeDamage(float DamageAmount, 
		struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, 
		AActor* DamageCauser) override;

	FORCEINLINE bool IsDead() { return Health == 0; }
	void Attack();
	void SpawnPickAxe();
	void EquipPickAxe();
	void UnEquipPickAxe();
	void PullTrigger();
	float GetHealthPercent() const;
	void WeaponChange();
	void WeaponKnifeChange();
	void EquipGun(AGun* Equip);
	void EquipKnife(AKnife* Equip);
	void PlayDeadAnim();
	void DestroyCharacter();
	// �ڿ� ��ȣ �ۿ�
	void OnInteract();
	bool bIsKnifeAttacking;
	UFUNCTION()
	void KnifeAttackStart();
	UFUNCTION()
	void KnifeAttackEnd();
	UCharacter_AnimInstance* GetAnimInstance() const
	{
		return AnimInstance;
	}

	UTeamComponent* GetTeamComponent() const
	{
		return TeamComponent;
	}

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY()
	class UCharacter_AnimInstance* AnimInstance = nullptr;

	UPROPERTY(BlueprintReadOnly)
	UTeamComponent* TeamComponent;

private:

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	FTimerHandle timer;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;

	UPROPERTY(VisibleAnywhere)
	float Health;



	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<AGun> GunClass;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<AGun> RifleClass;
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<AKnife> KnifeClass;
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<APickAxe> PickAxeClass;
	UPROPERTY(VisibleAnywhere)
	APickAxe* SpawnedPickAxe;
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing=OnRep_EquippedGun)
	AGun* EquippedGun;
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing=OnRep_EquippedKnife)
	AKnife* EquippedKnife;

	UPROPERTY(EditDefaultsOnly)
	AGun* Gun = nullptr;
	UPROPERTY(EditDefaultsOnly)
	AKnife* Knife = nullptr;


	UPROPERTY(VisibleAnywhere, BlueprintGetter = GetInventoryComponent)
	UInventoryComponent* InventoryComponent;

	// 애니메이션 동기화용 카운트. 값 변경시켜 클라이언트에서 감지하는 것 이외에는 값에 의미 없음
	UPROPERTY(ReplicatedUsing=OnRep_FireCount)
	int32 FireCount;

	UPROPERTY(ReplicatedUsing=OnRep_KnifeAttackCount)
	int32 KnifeAttackCount;
	
	UPROPERTY(ReplicatedUsing=OnRep_HitCount)
	int32 HitCount;
	
	UPROPERTY(ReplicatedUsing=OnRep_bDeadNotify)
	bool bDeadNotify;
	
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
	void OnRep_bDeadNotify();

	UFUNCTION()
	void OnRep_CurrentWeapon();
	
	void RefreshAnimInstance();
};


