// Copyright 2025 ShootingStar. All Rights Reserved.


#include "Character_AnimInstance.h"

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

void UCharacter_AnimInstance::NativeUpdateAnimation(const float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!IsValid(CurrentMontage) || TimeCurrentMontageElapsed > TimeCurrentMontageRequested)
	{
		return;
	}

	// LoopCount가 여러번 지정되어 여러번 재생해야 하면 재생
	const float RemainingTime = TimeCurrentMontageRequested - TimeCurrentMontageElapsed;
	const float CurrentMontageLength = CurrentMontage->GetPlayLength();
	if (!Montage_IsPlaying(CurrentMontage) && TimeCurrentMontageRequested / CountCurrentMontageRequested <= RemainingTime)
	{
		const float PlayRate = TimeCurrentMontageRequested <= 0.0f ? 1.0f * CountCurrentMontageRequested : CurrentMontageLength / TimeCurrentMontageRequested * CountCurrentMontageRequested;
		Montage_Play(CurrentMontage, PlayRate, EMontagePlayReturnType::Duration, 0.0f, false);
	}
	
	TimeCurrentMontageElapsed += DeltaSeconds;
}
void UCharacter_AnimInstance::PlayMiningMontage(const float DesiredPlayTime, const int32 LoopCount)
{
	// const float PlayRate = DesiredPlayTime <= 0.0f ? 1.0f : MiningLoopMontage->GetPlayLength() / DesiredPlayTime;
	// Montage_Play(MiningLoopMontage, PlayRate);
	PlayMontage(MiningLoopMontage, DesiredPlayTime, LoopCount, true);
}
void UCharacter_AnimInstance::PlayHitMontage(const float DesiredPlayTime, const int32 LoopCount)
{
	// const float PlayRate = DesiredPlayTime <= 0.0f ? 1.0f : HitMontage->GetPlayLength() / DesiredPlayTime;
	// //Montage_Play(HitMontage, PlayRate, EMontagePlayReturnType::MontageLength, 0.0f, false);
	// Montage_Play(HitMontage, PlayRate);
	PlayMontage(HitMontage, DesiredPlayTime, LoopCount, false);
}

void UCharacter_AnimInstance::PlayDeadMontage(const float DesiredPlayTime, const int32 LoopCount)
{
	// const float PlayRate = DesiredPlayTime <= 0.0f ? 1.0f : DeadMontage->GetPlayLength() / DesiredPlayTime;
	// Montage_Play(DeadMontage, PlayRate);
	// //DeadMontage->BlendOut.SetBlendTime(0.f);
	PlayMontage(DeadMontage, DesiredPlayTime, LoopCount, true);
}

void UCharacter_AnimInstance::PlayFireMontage(const float DesiredPlayTime, const int32 LoopCount)
{
	// //const float MontageLength = AK_Fire_Montage->GetPlayLength();
	// const float PlayRate = DesiredPlayTime <= 0.0f ? 1.0f : AK_Fire_Montage->GetPlayLength() / DesiredPlayTime;
	// //Montage_Play(AK_Fire_Montage, PlayRate, EMontagePlayReturnType::MontageLength, 0.0f, false);
	// Montage_Play(AK_Fire_Montage, PlayRate);
	PlayMontage(AK_Fire_Montage, DesiredPlayTime, LoopCount, true);
}

void UCharacter_AnimInstance::PlayRocketFireMontage(const float DesiredPlayTime, const int32 LoopCount)
{
	// //float MontageLength = RocketLauncher_Shoot_Montage->GetPlayLength();
	// const float PlayRate = DesiredPlayTime <= 0.0f ? 1.0f : RocketLauncher_Shoot_Montage->GetPlayLength() / DesiredPlayTime;
	// //Montage_Play(RocketLauncher_Shoot_Montage, PlayRate, EMontagePlayReturnType::MontageLength, 0.0f, false);
	// Montage_Play(RocketLauncher_Shoot_Montage);
	PlayMontage(RocketLauncher_Shoot_Montage, DesiredPlayTime, LoopCount, true);
}

void UCharacter_AnimInstance::PlayKnifeAttackMontage(float DesiredPlayTime, const int32 LoopCount)
{
	//float MontageLength = KnifeAttackMontage->GetPlayLength();
	// const float PlayRate = DesiredPlayTime <= 0.0f ? 1.0f : RocketLauncher_Shoot_Montage->GetPlayLength() / DesiredPlayTime;
	// Montage_Play(KnifeAttackMontage);
	PlayMontage(KnifeAttackMontage, DesiredPlayTime, LoopCount, true);
}

void UCharacter_AnimInstance::StopMiningMontage()
{
	if (CurrentMontage == MiningLoopMontage)
	{
		CurrentMontage = nullptr;
	}
	Montage_Stop(0.2f, MiningLoopMontage);
}

void UCharacter_AnimInstance::AnimNotify_MiningHit(UAnimNotify* Notify)
{
	OnMiningAnimationHit.Broadcast();
}

void UCharacter_AnimInstance::PlayMontage(UAnimMontage* Montage, float DesiredPlayTime, const int32 LoopCount, const bool bShouldStopOthers)
{
	if (Montage_IsPlaying(DeadMontage))
	{
		return;
	}

	if (bShouldStopOthers)
	{
		Montage_Stop(0.0f);
	}

	if (LoopCount > 1)
	{
		CurrentMontage = Montage;
		TimeCurrentMontageElapsed = 0.0f;
		TimeCurrentMontageRequested = DesiredPlayTime == 0.0f ? Montage->GetPlayLength() : DesiredPlayTime;
		CountCurrentMontageRequested = LoopCount;
	}
	else
	{
		const float PlayRate = DesiredPlayTime == 0.0f ? 1.0f : Montage->GetPlayLength() / DesiredPlayTime;
		Montage_Play(Montage, PlayRate, EMontagePlayReturnType::Duration, 0.0f, bShouldStopOthers);
	}
}

// void UCharacter_AnimInstance::PlayAKIdleMontage()
// {
// 	if (AK_Idle_Montage && !Montage_IsPlaying(AK_Idle_Montage))
// 	{
// 		Montage_Play(AK_Idle_Montage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);
// 	}
// }
//
// void UCharacter_AnimInstance::StopAKIdleMontage()
// {
// 	if (AK_Idle_Montage && Montage_IsPlaying(AK_Idle_Montage))
// 	{
// 		Montage_Stop(0.2f, AK_Idle_Montage);
// 	}
// }
void UCharacter_AnimInstance::AnimNotify_AttackStart()
{
	// ACompetitivePlayerCharacter* Character = Cast<ACompetitivePlayerCharacter>(TryGetPawnOwner());
	// if (Character)
	// {
	// 	if (!IsKnifeEquipped)
	// 	{
	// 		Character->PickAxeAttackStart();
	// 	}
	// 	else
	// 	{
	// 		Character->KnifeAttackStart();
	// 	}
	// }
}
void UCharacter_AnimInstance::AnimNotify_AttackEnd()
{
	// ACompetitivePlayerCharacter* Character = Cast<ACompetitivePlayerCharacter>(TryGetPawnOwner());
	// if (Character)
	// {
	// 	if (!IsKnifeEquipped)
	// 	{
	// 		Character->PickAxeAttackEnd();
	// 	}
	// 	else
	// 	{
	// 		Character->KnifeAttackEnd();
	// 	}
	// }
}