// Copyright 2025 ShootingStar. All Rights Reserved.


#include "CompetitiveGameState.h"
#include "CompetitiveGameMode.h"
#include "Net/UnrealNetwork.h"

ACompetitiveGameState::ACompetitiveGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACompetitiveGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!HasAuthority())
	{
		return;
	}
	ensure(GetWorld());

	ACompetitiveGameMode* const CompetitiveGameMode = Cast<ACompetitiveGameMode>(GetWorld()->GetAuthGameMode());
	ensure(CompetitiveGameMode);
	if (!CompetitiveGameMode)
	{
		return;
	}

	UCompetitiveSystemComponent* const CompetitiveSystem = CompetitiveGameMode->GetCompetitiveSystemComponent();
	ensure(CompetitiveSystem);
	if (!CompetitiveSystem)
	{
		return;
	}

	if (AssignIfDifferent(BlueTeamRoundScore, CompetitiveSystem->GetBlueTeamRoundScore()))
	{
		OnRep_BlueTeamRoundScore();
	}
	if (AssignIfDifferent(RedTeamRoundScore, CompetitiveSystem->GetRedTeamRoundScore()))
	{
		OnRep_RedTeamRoundScore();
	}
	if (AssignIfDifferent(BlueTeamGameScore, CompetitiveSystem->GetBlueTeamGameScore()))
	{
		OnRep_BlueTeamGameScore();
	}
	if (AssignIfDifferent(RedTeamGameScore, CompetitiveSystem->GetRedTeamGameScore()))
	{
		OnRep_RedTeamGameScore();
	}
	if (AssignIfDifferent(Phase, CompetitiveSystem->GetCurrentPhase()))
	{
		OnRep_Phase();
	}
	if (AssignIfDifferent(PhaseTime, CompetitiveSystem->GetCurrentPhaseTime()))
	{
		OnRep_PhaseTime();
	}
}

void ACompetitiveGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACompetitiveGameState, BlueTeamRoundScore);
	DOREPLIFETIME(ACompetitiveGameState, RedTeamRoundScore);
	DOREPLIFETIME(ACompetitiveGameState, BlueTeamGameScore);
	DOREPLIFETIME(ACompetitiveGameState, RedTeamGameScore);
	DOREPLIFETIME(ACompetitiveGameState, Phase);
	DOREPLIFETIME(ACompetitiveGameState, PhaseTime);
}

void ACompetitiveGameState::OnRep_BlueTeamRoundScore()
{
	OnBlueTeamRoundScoreChanged.Broadcast(BlueTeamRoundScore);
}

void ACompetitiveGameState::OnRep_RedTeamRoundScore()
{
	OnRedTeamRoundScoreChanged.Broadcast(RedTeamRoundScore);
}

void ACompetitiveGameState::OnRep_BlueTeamGameScore()
{
	OnBlueTeamGameScoreChanged.Broadcast(BlueTeamGameScore);
}

void ACompetitiveGameState::OnRep_RedTeamGameScore()
{
	OnRedTeamGameScoreChanged.Broadcast(RedTeamGameScore);
}

void ACompetitiveGameState::OnRep_PhaseTime()
{
	OnPhaseTimeChanged.Broadcast(PhaseTime);
}

void ACompetitiveGameState::OnRep_Phase()
{
	OnPhaseChanged.Broadcast(Phase);
}
