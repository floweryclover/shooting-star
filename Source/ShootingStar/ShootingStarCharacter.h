// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShootingStarCharacter.generated.h"

class AGun;
class AKnife;
class UInventoryComponent;

UCLASS(Blueprintable)
class AShootingStarCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AShootingStarCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AloowPrivateAccess = "true"))
	USkeletalMeshComponent* PickAxeMesh;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UFUNCTION(BlueprintCallable)
	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	UPROPERTY(EditDefaultsOnly)
	TArray<AGun*> WeaponList;

	UFUNCTION()
	FORCEINLINE void AddWeaponToList(AGun* Weapon) { WeaponList.Add(Weapon); }

	virtual float TakeDamage(float DamageAmount, 
		struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, 
		AActor* DamageCauser) override;

	FORCEINLINE bool IsDead() { return Health == 0; }
	void Attack();
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

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	UPROPERTY()
	class UCharacter_AnimInstance* AnimInstance = nullptr;

private:

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	// 

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

	UPROPERTY(EditDefaultsOnly)
	AGun* EquippedGun;
	UPROPERTY(EditDefaultsOnly)
	AKnife* EquippedKnife;

	UPROPERTY(EditDefaultsOnly)
	AGun* Gun = nullptr;
	UPROPERTY(EditDefaultsOnly)
	AKnife* Knife = nullptr;


	UPROPERTY(VisibleAnywhere, BlueprintGetter = GetInventoryComponent)
	UInventoryComponent* InventoryComponent;
	
};

