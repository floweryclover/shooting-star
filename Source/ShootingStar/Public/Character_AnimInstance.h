// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character_AnimInstance.generated.h"

/**
 * 
 */

UCLASS()
class SHOOTINGSTAR_API UCharacter_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UCharacter_AnimInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool IsAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool IsGunEquipped;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool IsKnifeEquipped;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool IsRockLauncherEquipped;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
	void AnimNotify_AttackStart();
	UFUNCTION()
	void AnimNotify_AttackEnd();

	void PlayMiningMontage(float DesiredPlayTime = 0.0f);
	void PlayHitMontage(float DesiredPlayTime = 0.0f);
	void PlayDeadMontage(float DesiredPlayTime = 0.0f);
	void PlayFireMontage(float DesiredPlayTime = 0.0f);
	void PlayRocketFireMontage(float DesiredPlayTime = 0.0f);
	void PlayKnifeAttackMontage(float DesiredPlayTime = 0.0f);
	// void PlayAKIdleMontage();
	// void StopAKIdleMontage();

	// UAnimMontage* GetMontageRocketLauncherShoot() const { return RocketLauncher_Shoot_Montage; }
	// UAnimMontage* GetMontageRocketLauncherShoot() const { return MiningLoopMontage; }
	// UAnimMontage* GetMontageRocketLauncherShoot() const { return AK_Idle_Montage; }
	// UAnimMontage* GetMontageRocketLauncherShoot() const { return AK_Fire_Montage; }
	// UAnimMontage* GetMontageRocketLauncherShoot() const { return KnifeAttackMontage; }
	// UAnimMontage* GetMontageRocketLauncherShoot() const { return HitMontage; }
	// UAnimMontage* GetMontageRocketLauncherShoot() const { return DeadMontage; }

private:
	UPROPERTY()
	TObjectPtr<UAnimMontage> RocketLauncher_Shoot_Montage;
	UPROPERTY()
	TObjectPtr<UAnimMontage> MiningLoopMontage;
	UPROPERTY()
	TObjectPtr<UAnimMontage> AK_Idle_Montage;
	UPROPERTY()
	TObjectPtr<UAnimMontage> AK_Fire_Montage;
	UPROPERTY()
	TObjectPtr<UAnimMontage> KnifeAttackMontage;
	UPROPERTY()
	TObjectPtr<UAnimMontage> HitMontage;
	UPROPERTY()
	TObjectPtr<UAnimMontage> DeadMontage;
};
