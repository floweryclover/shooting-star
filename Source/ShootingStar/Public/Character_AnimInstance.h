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

	void PlayMiningMontage(float DesiredPlayTime = 0.0f, int32 LoopCount = 1);
	void PlayHitMontage(float DesiredPlayTime = 0.0f, int32 LoopCount = 1);
	void PlayDeadMontage(float DesiredPlayTime = 0.0f, int32 LoopCount = 1);
	void PlayFireMontage(float DesiredPlayTime = 0.0f, int32 LoopCount = 1);
	void PlayRocketFireMontage(float DesiredPlayTime = 0.0f, int32 LoopCount = 1);
	void PlayKnifeAttackMontage(float DesiredPlayTime = 0.0f, int32 LoopCount = 1);
	// void PlayAKIdleMontage();
	// void StopAKIdleMontage();

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

	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentMontage;
	float TimeCurrentMontageRequested = 0.0f;
	int32 CountCurrentMontageRequested = 1.0f;
	float TimeCurrentMontageElapsed = 0.0f;

	void PlayMontage(UAnimMontage* Montage, float DesiredPlayTime, int32 LoopCount, bool bShouldStopOthers);
};
