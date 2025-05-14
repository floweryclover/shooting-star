// Copyright 2025 ShootingStar. All Rights Reserved.


#include "Character_AnimInstance.h"
#include "CompetitivePlayerCharacter.h"

UCharacter_AnimInstance::UCharacter_AnimInstance()
{

	static ConstructorHelpers::FObjectFinder<UAnimMontage>GetHitMontage(TEXT("AnimMontage'/Game/Toon_Soldiers_UE5/Animations/Akimbo/Character_Hit.Character_Hit'"));
	if (GetHitMontage.Succeeded()) HitMontage = GetHitMontage.Object;
	static ConstructorHelpers::FObjectFinder<UAnimMontage>GetDeadMontage(TEXT("AnimMontage'/Game/Toon_Soldiers_UE5/Animations/Akimbo/Character_Dead.Character_Dead'"));
	if (GetDeadMontage.Succeeded()) DeadMontage = GetDeadMontage.Object;
	static ConstructorHelpers::FObjectFinder<UAnimMontage>GetAK_FireMontage(TEXT("AnimMontage'/Game/Toon_Soldiers_UE5/Animations/Akimbo/Character_Fire.Character_Fire'"));
	if (GetAK_FireMontage.Succeeded()) AK_Fire_Montage = GetAK_FireMontage.Object;
	static ConstructorHelpers::FObjectFinder<UAnimMontage> GetAK_IdleMontage(TEXT("AnimMontage'/Game/Toon_Soldiers_UE5/Animations/Akimbo/Character_GunIdle.Character_GunIdle'"));
	if (GetAK_IdleMontage.Succeeded()) AK_Idle_Montage = GetAK_IdleMontage.Object;
	static ConstructorHelpers::FObjectFinder<UAnimMontage> GetKnifeAttackMontage(TEXT("AnimMontage'/Game/Toon_Soldiers_UE5/Animations/Akimbo/Character_KnifeAttack.Character_KnifeAttack'"));
	if (GetKnifeAttackMontage.Succeeded()) KnifeAttackMontage = GetKnifeAttackMontage.Object;
	static ConstructorHelpers::FObjectFinder<UAnimMontage> GetMiningLoop(TEXT("AnimMontage'/Game/Toon_Soldiers_UE5/Animations/PickAxe/MiningLoopMontage.MiningLoopMontage'"));
	if (GetMiningLoop.Succeeded()) MiningLoopMontage = GetMiningLoop.Object;
	static ConstructorHelpers::FObjectFinder<UAnimMontage> GetRocketShoot(TEXT("AnimMontage'/Game/Toon_Soldiers_UE5/Animations/RocketLauncher/rocketlauncher_shoot.rocketlauncher_shoot'"));
	if (GetRocketShoot.Succeeded()) RocketLauncher_Shoot_Montage = GetRocketShoot.Object;
}

void UCharacter_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

}
void UCharacter_AnimInstance::PlayMiningMontage()
{
	Montage_Play(MiningLoopMontage);
}
void UCharacter_AnimInstance::PlayHitMontage()
{
	Montage_Play(HitMontage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, false);
}

void UCharacter_AnimInstance::PlayDeadMontage()
{
	Montage_Play(DeadMontage);
	DeadMontage->BlendOut.SetBlendTime(0.f);
}
void UCharacter_AnimInstance::PlayFireMontage()
{
	float MontageLength = AK_Fire_Montage->GetPlayLength();
	Montage_Play(AK_Fire_Montage, MontageLength, EMontagePlayReturnType::MontageLength, 0.0f, false);
}
void UCharacter_AnimInstance::PlayRocketFireMontage()
{
	float MontageLength = RocketLauncher_Shoot_Montage->GetPlayLength();
	Montage_Play(RocketLauncher_Shoot_Montage, MontageLength, EMontagePlayReturnType::MontageLength, 0.0f, false);
}
void UCharacter_AnimInstance::PlayKnifeAttackMontage()
{
	float MontageLength = KnifeAttackMontage->GetPlayLength();
	Montage_Play(KnifeAttackMontage, MontageLength, EMontagePlayReturnType::MontageLength, 0.0f, false);
}

void UCharacter_AnimInstance::PlayAKIdleMontage()
{
	if (AK_Idle_Montage && !Montage_IsPlaying(AK_Idle_Montage))
	{
		Montage_Play(AK_Idle_Montage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);
	}
}

void UCharacter_AnimInstance::StopAKIdleMontage()
{
	if (AK_Idle_Montage && Montage_IsPlaying(AK_Idle_Montage))
	{
		Montage_Stop(0.2f, AK_Idle_Montage);
	}
}
void UCharacter_AnimInstance::AnimNotify_AttackStart()
{
	ACompetitivePlayerCharacter* Character = Cast<ACompetitivePlayerCharacter>(TryGetPawnOwner());
	if (Character)
	{
		if (!IsKnifeEquipped)
		{
			Character->PickAxeAttackStart();
		}
		else
		{
			Character->KnifeAttackStart();
		}
	}
}
void UCharacter_AnimInstance::AnimNotify_AttackEnd()
{
	ACompetitivePlayerCharacter* Character = Cast<ACompetitivePlayerCharacter>(TryGetPawnOwner());
	if (Character)
	{
		if (!IsKnifeEquipped)
		{
			Character->PickAxeAttackEnd();
		}
		else
		{
			Character->KnifeAttackEnd();
		}
	}
}