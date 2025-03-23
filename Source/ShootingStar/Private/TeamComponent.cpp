// Copyright 2025 ShootingStar. All Rights Reserved.


#include "TeamComponent.h"

#include "Net/UnrealNetwork.h"

UTeamComponent::UTeamComponent()
{
	SetIsReplicatedByDefault(true);
}


void UTeamComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTeamComponent, Team);
}