// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerComponent.h"
#include "CompetitiveGameMode.h"
#include "CompetitivePlayerCharacter.h"

template<void (ACompetitivePlayerCharacter::*Action)()>
void ExecuteOnControlledCharacter(UObject* Owner)
{
	if (AController* Controller = Cast<AController>(Owner))
	{
		if (ACompetitivePlayerCharacter* Char = Cast<ACompetitivePlayerCharacter>(Controller->GetPawn()))
		{
			(Char->*Action)();   // 멤버 함수 포인터 호출
		}
	}
}

void UServerComponent::RequestInteractResource_Implementation()
{
	ACompetitiveGameMode* GameMode = Cast<ACompetitiveGameMode>(GetWorld()->GetAuthGameMode());
	AController* const Controller = Cast<AController>(GetOwner());
	GameMode->InteractResource(Controller);
}

void UServerComponent::RequestCraftWeapon_Implementation(const FWeaponData& Weapon, const TArray<int32>& Resources)
{
	ACompetitiveGameMode* GameMode = Cast<ACompetitiveGameMode>(GetWorld()->GetAuthGameMode());
	AController* const Controller = Cast<AController>(GetOwner());
	GameMode->CraftWeapon(Controller, Weapon, Resources);
}

void UServerComponent::RequestAttack_Implementation()
{
	ExecuteOnControlledCharacter<&ACompetitivePlayerCharacter::Attack>(GetOwner());
}

void UServerComponent::RequestEquipWeapon_Implementation()
{
	ExecuteOnControlledCharacter<&ACompetitivePlayerCharacter::WeaponChange>(GetOwner());
}

void UServerComponent::RequestEquipKnifeWeapon_Implementation()
{
	ExecuteOnControlledCharacter<&ACompetitivePlayerCharacter::WeaponKnifeChange>(GetOwner());
}
