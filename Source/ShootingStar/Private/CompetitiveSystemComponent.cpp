// Copyright 2025 ShootingStar. All Rights Reserved.


#include "CompetitiveSystemComponent.h"

#include "GameFramework/PlayerState.h"
#include "ShootingStar/ShootingStar.h"

UCompetitiveSystemComponent::UCompetitiveSystemComponent()
	: BlueTeamRoundScore{0},
	  RedTeamRoundScore{0},
	  BlueTeamGameScore{0},
	  RedTeamGameScore{0},
	  CurrentPhaseTime{0.0f},
	  CurrentPhase{ECompetitiveGamePhase::WaitingForStart}
{
}

void UCompetitiveSystemComponent::Update(const float DeltaTime)
{
	CurrentPhaseTime += DeltaTime;

	if (CurrentPhase == ECompetitiveGamePhase::WaitingForStart)
	{
		Update_WaitingForStart();
	}
	else if (CurrentPhase == ECompetitiveGamePhase::Game)
	{
		Update_Game();
	}
	else if (CurrentPhase == ECompetitiveGamePhase::RoundEnd)
	{
		Update_RoundEnd();
	}
	else if (CurrentPhase == ECompetitiveGamePhase::GameEnd)
	{
		Update_GameEnd();
	}
	else
	{
		Update_GameDestroyed();
	}
}

void UCompetitiveSystemComponent::StartGame()
{
	check(CurrentPhase == ECompetitiveGamePhase::WaitingForStart);

	CurrentPhase = ECompetitiveGamePhase::Game;
	CurrentPhaseTime = 0.0f;
}

void UCompetitiveSystemComponent::EndGame()
{
	check(CurrentPhase != ECompetitiveGamePhase::GameDestroyed);
	check(CurrentPhase != ECompetitiveGamePhase::GameEnd);

	CurrentPhase = ECompetitiveGamePhase::GameEnd;
	CurrentPhaseTime = 0.0f;
}

void UCompetitiveSystemComponent::GiveRoundScoreForTeam(const ETeam Team, const int Score)
{
	if (Team == ETeam::None || CurrentPhase != ECompetitiveGamePhase::Game)
	{
		return;
	}

	int& TeamScore = Team == ETeam::Blue ? BlueTeamRoundScore : RedTeamRoundScore;
	TeamScore += Score;
}

ETeam UCompetitiveSystemComponent::GetTeamForNextPlayer(const TArray<APlayerState*>& PlayerArray) const
{
	int NumBlueTeamPlayers = 0;
	int NumRedTeamPlayers = 0;
	for (const auto PlayerState : PlayerArray)
	{
		UTeamComponent* TeamComponent = Cast<UTeamComponent>(PlayerState->GetPlayerController()->GetComponentByClass(UTeamComponent::StaticClass()));
		check(TeamComponent != nullptr);

		if (TeamComponent->GetTeam() == ETeam::Red)
		{
			NumRedTeamPlayers += 1;
		}
		else if (TeamComponent->GetTeam() == ETeam::Blue)
		{
			NumBlueTeamPlayers += 1;
		}
	}

	if (NumRedTeamPlayers >= MaxPlayersPerTeam && NumBlueTeamPlayers >= MaxPlayersPerTeam)
	{
		return ETeam::None;	
	}
	else if (NumBlueTeamPlayers <= NumRedTeamPlayers)
	{
		return ETeam::Blue;
	}
	else
	{
		return ETeam::Red;
	}
}

void UCompetitiveSystemComponent::Update_WaitingForStart()
{
}

void UCompetitiveSystemComponent::Update_Game()
{
	ensure(BlueTeamGameScore < GameWinningScore && RedTeamGameScore < GameWinningScore);
	if (BlueTeamGameScore >= GameWinningScore || RedTeamGameScore >= GameWinningScore)
	{
		CurrentPhase = ECompetitiveGamePhase::GameEnd;
		CurrentPhaseTime = 0.0f;
		return;
	}

	if (CurrentPhaseTime >= RoundTime
		|| BlueTeamRoundScore >= RoundWinningScore
		|| RedTeamRoundScore >= RoundWinningScore)
	{
		ETeam WinTeam = ETeam::None;
		if (BlueTeamRoundScore > RedTeamRoundScore)
		{
			WinTeam = ETeam::Blue;
		}
		else if (BlueTeamRoundScore < RedTeamRoundScore)
		{
			WinTeam = ETeam::Red;
		}

		if (WinTeam != ETeam::None)
		{
			int& CurrentTeamGameScore = WinTeam == ETeam::Blue ? BlueTeamGameScore : RedTeamGameScore;
			ensure(CurrentTeamGameScore < GameWinningScore);
			if (CurrentTeamGameScore < GameWinningScore)
			{
				CurrentTeamGameScore += 1;
			}
		}

		RedTeamRoundScore = 0;
		BlueTeamRoundScore = 0;
		CurrentPhaseTime = 0.0f;

		if (BlueTeamGameScore >= GameWinningScore
			|| RedTeamGameScore >= GameWinningScore)
		{
			CurrentPhase = ECompetitiveGamePhase::GameEnd;
		}
		else
		{
			CurrentPhase = ECompetitiveGamePhase::RoundEnd;
		}
	}
}

void UCompetitiveSystemComponent::Update_RoundEnd()
{
	if (CurrentPhaseTime >= RoundEndTime)
	{
		CurrentPhase = ECompetitiveGamePhase::Game;
		CurrentPhaseTime = 0.0f;
	}
}

void UCompetitiveSystemComponent::Update_GameEnd()
{
	if (CurrentPhaseTime >= GameEndTime)
	{
		CurrentPhase = ECompetitiveGamePhase::GameDestroyed;
		CurrentPhaseTime = 0.0f;
	}
}

void UCompetitiveSystemComponent::Update_GameDestroyed()
{
}
