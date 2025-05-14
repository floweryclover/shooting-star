// Copyright 2025 ShootingStar. All Rights Reserved.


#include "CompetitiveSystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "CompetitiveGameMode.h"
#include "SafeZoneActor.h"

UCompetitiveSystemComponent::UCompetitiveSystemComponent()
	: BlueTeamKillScore{0},
	  RedTeamKillScore{0},
	  BlueTeamGameScore{0},
	  RedTeamGameScore{0},
	  CurrentPhaseTime{0.0f},
	  CurrentPhase{ECompetitiveGamePhase::WaitingForStart}
{
}

void UCompetitiveSystemComponent::Init(TFunction<float()> InGetSafeZoneRadius)
{
	GetSafeZoneRadius = MoveTemp(InGetSafeZoneRadius);
}

void UCompetitiveSystemComponent::BeginPlay()
{
    Super::BeginPlay();
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
	SupplyDropsTriggered.Init(false, 3);
	OnGameStarted.Broadcast();
}

void UCompetitiveSystemComponent::EndGame()
{
	check(CurrentPhase != ECompetitiveGamePhase::GameDestroyed);
	check(CurrentPhase != ECompetitiveGamePhase::GameEnd);

	CurrentPhase = ECompetitiveGamePhase::GameEnd;
	CurrentPhaseTime = 0.0f;
}

void UCompetitiveSystemComponent::GiveKillScoreForTeam(const ETeam Team)
{
	if (Team == ETeam::None || CurrentPhase != ECompetitiveGamePhase::Game)
	{
		return;
	}

	int& TeamKillScore = Team == ETeam::Blue ? BlueTeamKillScore : RedTeamKillScore;
	TeamKillScore += 1;
	
	// 골든 킬(점수를 얻는 팀이 즉시 승리)인지?
	if (IsGoldenKillTime())
	{
		WinTeam(Team);
	}
}

ETeam UCompetitiveSystemComponent::GetTeamForNextPlayer(const TArray<APlayerState*>& PlayerArray)
{
	int NumBlueTeamPlayers = 0;
	int NumRedTeamPlayers = 0;
	for (const auto PlayerState : PlayerArray)
	{
		UTeamComponent* const TeamComponent = PlayerState->GetPlayerController()->FindComponentByClass<UTeamComponent>();
		if (!IsValid(TeamComponent))
		{
			continue;
		}

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
	// 3선승 팀이 결정되었다면 게임 즉시 종료
	ensure(BlueTeamGameScore < GameWinningScore && RedTeamGameScore < GameWinningScore);
	if (BlueTeamGameScore >= GameWinningScore || RedTeamGameScore >= GameWinningScore)
	{
		CurrentPhase = ECompetitiveGamePhase::GameEnd;
		CurrentPhaseTime = 0.0f;
		
		return;
	}
	
	// 승리 팀 결정됨
	if (BlueTeamKillScore >= RoundWinningKillScore || RedTeamKillScore >= RoundWinningKillScore // 점수 기준을 넘겼거나
	|| (CurrentPhaseTime >= GameTime && (BlueTeamKillScore > RedTeamKillScore || RedTeamKillScore > BlueTeamKillScore))) // 시간이 다 되었을 때 앞선 팀이 있다면
	{
		const ETeam Team = BlueTeamKillScore > RedTeamKillScore ? ETeam::Blue : ETeam::Red;

		WinTeam(Team);
	}

	// 보급품 업데이트
	CheckAndTriggerSupplyDrop();
}

void UCompetitiveSystemComponent::CheckAndTriggerSupplyDrop()
{
    for (int32 i = 0; i < SupplyDropsTriggered.Num(); ++i)
    {
        if (!SupplyDropsTriggered[i] && CurrentPhaseTime >= SupplyDropTimes[i])
        {
            SupplyDropsTriggered[i] = true;
            
            // 맵 중앙 기준으로 현재 자기장 반경 내 랜덤 위치 선정
            const float RandomAngle = FMath::RandRange(0.f, 360.f);
            const float CurrentRadius = GetSafeZoneRadius();
            const float RandomRadius = FMath::RandRange(0.f, FMath::Min(CurrentRadius * 0.8f, 3000.0f));  // 자기장 80% 이내 위치에 생성하되, 이동 가능한 맵을 벗어나지 않게
            const FVector DropLocation(
                RandomRadius * FMath::Cos(RandomAngle),
                RandomRadius * FMath::Sin(RandomAngle),
                0.f
            );
            
			UE_LOG(LogTemp, Log, TEXT("Supply drop triggered at %s"), *DropLocation.ToString());
            OnSupplyDropped.Broadcast(DropLocation);
        }
    }
}

void UCompetitiveSystemComponent::Update_RoundEnd()
{
	if (CurrentPhaseTime >= RoundEndTime)
	{
		CurrentPhase = ECompetitiveGamePhase::Game;
		BlueTeamKillScore = 0;
		RedTeamKillScore = 0;
		CurrentPhaseTime = 0.0f;
		SupplyDropsTriggered.Init(false, 3);
		OnGameStarted.Broadcast();
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

void UCompetitiveSystemComponent::WinTeam(const ETeam Team)
{
	ensure(CurrentPhase == ECompetitiveGamePhase::Game);
	if (CurrentPhase != ECompetitiveGamePhase::Game)
	{
		return;
	}
	
	LastRoundWinTeam = Team;
	int& CurrentTeamGameScore = Team == ETeam::Blue ? BlueTeamGameScore : RedTeamGameScore;
	ensure(CurrentTeamGameScore < GameWinningScore);
	if (CurrentTeamGameScore < GameWinningScore)
	{
		CurrentTeamGameScore += 1;
	}
		
	if (BlueTeamGameScore >= GameWinningScore
		|| RedTeamGameScore >= GameWinningScore)
	{
		CurrentPhase = ECompetitiveGamePhase::GameEnd;
	}
	else
	{
		CurrentPhase = ECompetitiveGamePhase::RoundEnd;
	}
	CurrentPhaseTime = 0.0f;
}
