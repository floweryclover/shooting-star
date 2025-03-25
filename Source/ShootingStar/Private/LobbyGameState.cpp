// Copyright 2025 ShootingStar. All Rights Reserved.


#include "LobbyGameState.h"
#include "TeamComponent.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "ShootingStar/ShootingStar.h"

ALobbyGameState::ALobbyGameState()
	: UpdateElapsed{0.0f}
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALobbyGameState::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	UpdateElapsed += DeltaSeconds;
	if (UpdateElapsed >= UpdateInterval)
	{
		UpdateElapsed = 0.0f;

		BlueTeamPlayerNames.Empty();
		RedTeamPlayerNames.Empty();

		for (const auto PlayerState : PlayerArray)
		{
			AController* const Controller = PlayerState->GetOwningController();

			UTeamComponent* const TeamComponent = Cast<UTeamComponent>(
				Controller->GetComponentByClass(UTeamComponent::StaticClass()));
			check(TeamComponent != nullptr);

			const FString PlayerName = PlayerState->GetPlayerName();
			const ETeam Team = TeamComponent->GetTeam();
			if (Team == ETeam::Blue)
			{
				BlueTeamPlayerNames.Add(PlayerName);
			}
			else if (Team == ETeam::Red)
			{
				RedTeamPlayerNames.Add(PlayerName);
			}
			else
			{
				UE_LOG(LogShootingStar, Error, TEXT("ETeam::None인 플레이어가 존재합니다."));
			}
		}

		OnRep_PlayerNames();
	}
}


void ALobbyGameState::BeginPlay()
{
	Super::BeginPlay();
}

void ALobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyGameState, BlueTeamPlayerNames);
	DOREPLIFETIME(ALobbyGameState, RedTeamPlayerNames);
}

void ALobbyGameState::OnRep_PlayerNames()
{
	OnPlayerListChanged.Broadcast();
}
