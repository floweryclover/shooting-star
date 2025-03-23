// Copyright 2025 ShootingStar. All Rights Reserved.


#include "CompetitiveSystemComponent.h"

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


bool UCompetitiveSystemComponent::RegisterPlayer(APlayerController* const InPlayer, ETeam InTeam, FText& OutFailReason)
{
	ensure(CurrentPhase == ECompetitiveGamePhase::WaitingForStart);
	if (CurrentPhase != ECompetitiveGamePhase::WaitingForStart)
	{
		return false;
	}

	ensure(InPlayer);
	if (!IsValid(InPlayer))
	{
		OutFailReason = FText::FromString(TEXT("InPlayer가 유효하지 않습니다."));
		return false;
	}

	if (RedTeamPlayers.Num() >= MaxPlayersPerTeam && BlueTeamPlayers.Num() >= MaxPlayersPerTeam)
	{
		OutFailReason = FText::FromString(TEXT("참여 가능한 팀이 없습니다."));
		return false;
	}

	if (InTeam == ETeam::None)
	{
		if (RedTeamPlayers.Num() <= BlueTeamPlayers.Num())
		{
			InTeam = ETeam::Red;
		}
		else
		{
			InTeam = ETeam::Blue;
		}
	}

	TArray<TObjectPtr<APlayerController>>& SelectedTeamPlayers =
		InTeam == ETeam::Red ? RedTeamPlayers : BlueTeamPlayers;
	if (SelectedTeamPlayers.Num() >= MaxPlayersPerTeam)
	{
		FString Reason = TEXT("플레이어를 ");
		Reason += InTeam == ETeam::Red ? TEXT("레드팀") : TEXT("블루팀");
		Reason += TEXT("에 등록하려고 시도했으나, 해당 팀에 더 이상 자리가 없습니다.");
		OutFailReason = FText::FromString(Reason);
		return false;
	}

	SelectedTeamPlayers.Push(InPlayer);
	return true;
}

bool UCompetitiveSystemComponent::IsPlayerRegistered(const APlayerController* const Player) const
{
	return GetTeamOf(Player) != ETeam::None;
}

ETeam UCompetitiveSystemComponent::GetTeamOf(const APlayerController* const Player) const
{
	for (const auto& RedTeamPlayer : RedTeamPlayers)
	{
		if (Player == RedTeamPlayer)
		{
			return ETeam::Red;
		}
	}
	for (const auto& BlueTeamPlayer : BlueTeamPlayers)
	{
		if (Player == BlueTeamPlayer)
		{
			return ETeam::Blue;
		}
	}

	return ETeam::None;
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
	ensure(CurrentPhase == ECompetitiveGamePhase::WaitingForStart);
	if (CurrentPhase != ECompetitiveGamePhase::WaitingForStart)
	{
		return;
	}

	CurrentPhase = ECompetitiveGamePhase::Game;
	CurrentPhaseTime = 0.0f;
}

void UCompetitiveSystemComponent::GiveRoundScoreForTeam(const ETeam Team, const int Score)
{
	ensure(Team != ETeam::None && CurrentPhase == ECompetitiveGamePhase::Game);
	if (Team == ETeam::None || CurrentPhase != ECompetitiveGamePhase::Game)
	{
		return;
	}

	int& TeamScore = Team == ETeam::Blue ? BlueTeamRoundScore : RedTeamRoundScore;
	TeamScore += Score;
}

void UCompetitiveSystemComponent::GiveRoundScoreForPlayer(const APlayerController* const Player, const int Score)
{
	for (const auto& RedTeamPlayer : RedTeamPlayers)
	{
		if (Player == RedTeamPlayer)
		{
			GiveRoundScoreForTeam(ETeam::Red, Score);
			return;
		}
	}

	for (const auto& BlueTeamPlayer : BlueTeamPlayers)
	{
		if (Player == BlueTeamPlayer)
		{
			GiveRoundScoreForTeam(ETeam::Blue, Score);
			return;
		}
	}

	ensure(false);
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
