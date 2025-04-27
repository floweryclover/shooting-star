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

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
	void AnimNotify_AttackStart();
	UFUNCTION()
	void AnimNotify_AttackEnd();

	void PlayHitMontage();
	void PlayDeadMontage();
	void PlayFireMontage();
	void PlayKnifeAttackMontage();
	void PlayAKIdleMontage();
	void StopAKIdleMontage();
	
	UAnimMontage* AK_Idle_Montage;
	UAnimMontage* AK_Fire_Montage;
	UAnimMontage* KnifeAttackMontage;
	UAnimMontage* HitMontage;
	UAnimMontage* DeadMontage;
};
