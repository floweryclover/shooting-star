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
class UCompetitiveSystemComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	UCompetitiveSystemComponent();
	/**
	 * 해당 플레이어를 해당 팀에 배정하고 등록합니다.
	 * WaitingForStart 페이즈에서만 가능합니다.
	 * 플레이어 등록에 실패한 경우 false를 반환하며 내부적으로 등록되지 않습니다(이 경우 접속을 종료해야 할 것).
	 * @param InPlayer
	 * @param InTeam None인 경우 임의로 배정합니다.
	 * @param OutFailReason 반환값이 false인 경우 실패 이유가 저장됩니다.
	 * @return 유효하지 않은 플레이어인 경우 false
	 */
	UFUNCTION(BlueprintCallable)
	bool RegisterPlayer(APlayerController* InPlayer, ETeam InTeam, FText& OutFailReason);
	
	UFUNCTION(BlueprintCallable)
	bool IsPlayerRegistered(const APlayerController* Player) const;

	UFUNCTION(BlueprintCallable)
	ETeam GetTeamOf(const APlayerController* Player) const;
	
	/**
	 * 언리얼 내장 Tick(), TickComponent() 대신 유연한 구현을 위한 별도 틱 함수입니다.
	 * @param DeltaTime 
	 */
	UFUNCTION(BlueprintCallable)
	void Update(float DeltaTime);

	/**
	 * WaitingForStart 페이즈에서 최초 한 번만 호출되는, 게임을 시작하는 함수입니다.
	 * 이 함수 호출 이후에는 더 이상 플레이어를 추가로 등록할 수 없습니다.
	 */
	UFUNCTION(BlueprintCallable)
	void StartGame();

	/**
	 * 중간에 플레이어가 퇴장한 경우 등 비정상적인 상황에 호출되는, 게임을 종료하는 함수입니다.
	 */
	UFUNCTION(BlueprintCallable)
	void EndGame();

	UFUNCTION(BlueprintCallable)
	void GiveRoundScoreForTeam(ETeam Team, int Score);

	UFUNCTION(BlueprintCallable)
	void GiveRoundScoreForPlayer(const APlayerController* Player, int Score);

	const TArray<APlayerController*>& GetRedTeamPlayers() const
	{
		return RedTeamPlayers;
	}

	const TArray<APlayerController*>& GetBlueTeamPlayers() const
	{
		return BlueTeamPlayers;
	}

	int GetMaxPlayersPerTeam() const
	{
		return MaxPlayersPerTeam;
	}

	int GetRoundWinningScore() const
	{
		return RoundWinningScore;
	}

	int GetGameWinningScore() const
	{
		return GameWinningScore;
	}

	float GetRoundTime() const
	{
		return RoundTime;
	}

	float GetRoundEndTime() const
	{
		return RoundEndTime;
	}

	float GetCurrentPhaseTime() const
	{
		return CurrentPhaseTime;
	}

	int GetBlueTeamRoundScore() const
	{
		return BlueTeamRoundScore;
	}

	int GetRedTeamRoundScore() const
	{
		return RedTeamRoundScore;
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

protected:
	UPROPERTY(BlueprintReadOnly)
	int MaxPlayersPerTeam = 2;

	UPROPERTY(BlueprintReadOnly)
	int RoundWinningScore = 100;

	UPROPERTY(BlueprintReadOnly)
	int GameWinningScore = 3;

	UPROPERTY(BlueprintReadOnly)
	float RoundTime = 180.0f;

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
	TArray<TObjectPtr<APlayerController>> RedTeamPlayers;

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<APlayerController>> BlueTeamPlayers;

	UPROPERTY(BlueprintReadOnly)
	int BlueTeamRoundScore;

	UPROPERTY(BlueprintReadOnly)
	int RedTeamRoundScore;

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
};
