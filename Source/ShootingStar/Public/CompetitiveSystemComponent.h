// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "TeamComponent.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CompetitiveSystemComponent.generated.h"

UENUM(BlueprintType)
enum class ECompetitiveGamePhase : uint8
{
	WaitingForStart, // 기본 상태입니다. 외부에 의해 시작이 호출되어야 합니다.
	Game, // 라운드가 진행 중입니다.
	RoundEnd, // 한 라운드가 끝나 다음 라운드까지 잠시 대기 중입니다.
	GameEnd, // 모든 라운드가 끝났습니다.
	GameDestroyed, // 완전히 종료되어 더 이상 유효한 게임 상태가 아닙니다.
};

/**
* 3선승 게임인 경쟁 모드의 동작과 상태를 정의하는 컴포넌트입니다.
* @details RoundScore는 한 라운드에서 각 팀이 얻은 점수를, GameScore는 승리한 라운드 수를 의미합니다.
*/
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHOOTINGSTAR_API UCompetitiveSystemComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	UCompetitiveSystemComponent();
	
	/**
	 * 언리얼 내장 Tick(), TickComponent() 대신 유연한 구현을 위한 별도 틱 함수입니다.
	 * @param DeltaTime 
	 */
	UFUNCTION(BlueprintCallable)
	void Update(float DeltaTime);

	/**
	 * WaitingForStart 페이즈에서 최초 한 번만 호출되는, 게임을 시작하는 함수입니다.
	 */
	UFUNCTION(BlueprintCallable)
	void StartGame();

	/**
	 * 중간에 플레이어가 퇴장한 경우 등 비정상적인 상황에 호출되는, 게임을 종료하는 함수입니다.
	 */
	UFUNCTION(BlueprintCallable)
	void EndGame();

	UFUNCTION(BlueprintCallable)
	void GiveKillScoreForTeam(ETeam Team);

	/**
	 * 플레이어에게 할당해 줄 수 있는 팀을 계산합니다.
	 * @remarks 양 팀의 정원이 모두 가득 찬 경우 None이 반환됩니다.
	 * @param PlayerArray 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable)
	ETeam GetTeamForNextPlayer(const TArray<APlayerState*>& PlayerArray) const;

	int GetMaxPlayersPerTeam() const
	{
		return MaxPlayersPerTeam;
	}
	
	int GetRoundWinningKillScore() const
	{
		return RoundWinningKillScore;
	}

	int GetGameWinningScore() const
	{
		return GameWinningScore;
	}

	float GetRoundTime() const
	{
		return GameTime;
	}

	float GetRoundEndTime() const
	{
		return RoundEndTime;
	}

	float GetCurrentPhaseTime() const
	{
		return CurrentPhaseTime;
	}

	int GetBlueTeamKillScore() const
	{
		return BlueTeamKillScore;
	}

	int GetRedTeamKillScore() const
	{
		return RedTeamKillScore;
	}

	int GetBlueTeamGameScore() const
	{
		return BlueTeamGameScore;
	}

	int GetRedTeamGameScore() const
	{
		return RedTeamGameScore;
	}

	ECompetitiveGamePhase GetCurrentPhase() const
	{
		return CurrentPhase;
	}

	bool IsGoldenKillTime() const
	{
		return CurrentPhase == ECompetitiveGamePhase::Game && CurrentPhaseTime > GameTime;
	}

	float GetRemainingGameTime() const
	{
		if (CurrentPhase != ECompetitiveGamePhase::Game)
		{
			return 0.0f;
		}

		return IsGoldenKillTime() ? 0.0f : GameTime - CurrentPhaseTime;
	}

protected:
	UPROPERTY(BlueprintReadOnly)
	int MaxPlayersPerTeam = 2;
	
	UPROPERTY(BlueprintReadOnly)
	int RoundWinningKillScore = 5;

	UPROPERTY(BlueprintReadOnly)
	int GameWinningScore = 3;

	/**
	 * 한 라운드 타임입니다.
	 */
	UPROPERTY(BlueprintReadOnly)
	float GameTime = 180.0f;

	// GameEnd에서 GameDestroyed 상태로 전이까지 필요한 시간
	UPROPERTY(BlueprintReadOnly)
	float GameEndTime = 10.0f;

	/**
	 * 라운드가 종료된 후부터 다음 라운드 시작까지 대기하는 시간입니다.
	 * @return 
	 */
	UPROPERTY(BlueprintReadOnly)
	float RoundEndTime = 5.0f;

	UPROPERTY(BlueprintReadOnly)
	int BlueTeamKillScore;

	UPROPERTY(BlueprintReadOnly)
	int RedTeamKillScore;

	UPROPERTY(BlueprintReadOnly)
	int BlueTeamGameScore;

	UPROPERTY(BlueprintReadOnly)
	int RedTeamGameScore;
	
	UPROPERTY(BlueprintReadOnly)
	float CurrentPhaseTime;

	UPROPERTY(BlueprintReadOnly)
	ECompetitiveGamePhase CurrentPhase;

private:
	void Update_WaitingForStart();

	void Update_Game();

	void Update_RoundEnd();

	void Update_GameEnd();
	
	void Update_GameDestroyed();

	void WinTeam(ETeam Team);
};
