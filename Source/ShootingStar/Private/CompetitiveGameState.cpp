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
	UCompetitiveSystemComponent* const CompetitiveSystem = CompetitiveGameMode->GetCompetitiveSystemComponent();

	WaitingForGameStartTime = CompetitiveGameMode->GetWaitingForGameStartSeconds();
	BlueTeamKills = CompetitiveSystem->GetBlueTeamKillScore();
	RedTeamKills = CompetitiveSystem->GetRedTeamKillScore();
	BlueTeamWinRounds = CompetitiveSystem->GetBlueTeamGameScore();
	RedTeamWinRounds = CompetitiveSystem->GetRedTeamGameScore();
	Phase = CompetitiveSystem->GetCurrentPhase();
	PhaseTime = CompetitiveSystem->GetCurrentPhaseTime();
	bIsGoldenKillTime = CompetitiveSystem->IsGoldenKillTime();
	RemainingGameTime = CompetitiveSystem->GetRemainingGameTime();
	LastRoundWinTeam = CompetitiveSystem->GetLastRoundWinTeam();
}

void ACompetitiveGameState::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}
	
	ACompetitiveGameMode* const CompetitiveGameMode = Cast<ACompetitiveGameMode>(GetWorld()->GetAuthGameMode());
	CompetitiveGameMode->GetCompetitiveSystemComponent()->OnSupplyDropped.AddDynamic(this, &ACompetitiveGameState::OnSupplyDroppedHandler);
}

void ACompetitiveGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACompetitiveGameState, WaitingForGameStartTime);
	DOREPLIFETIME(ACompetitiveGameState, BlueTeamKills);
	DOREPLIFETIME(ACompetitiveGameState, RedTeamKills);
	DOREPLIFETIME(ACompetitiveGameState, BlueTeamWinRounds);
	DOREPLIFETIME(ACompetitiveGameState, RedTeamWinRounds);
	DOREPLIFETIME(ACompetitiveGameState, Phase);
	DOREPLIFETIME(ACompetitiveGameState, PhaseTime);
	DOREPLIFETIME(ACompetitiveGameState, bIsGoldenKillTime);
	DOREPLIFETIME(ACompetitiveGameState, RemainingGameTime);
	DOREPLIFETIME(ACompetitiveGameState, LastRoundWinTeam);
	DOREPLIFETIME(ACompetitiveGameState, SupplyActors);
}

void ACompetitiveGameState::OnSupplyDroppedHandler(FVector)
{
	ACompetitiveGameMode* const CompetitiveGameMode = Cast<ACompetitiveGameMode>(GetWorld()->GetAuthGameMode());
	SupplyActors = CompetitiveGameMode->GetSupplyActors();
}
