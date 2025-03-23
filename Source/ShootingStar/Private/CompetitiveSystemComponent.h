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
* 5판3선승 게임인 경쟁 모드의 동작과 상태를 정의하는 컴포넌트입니다.
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
	/**
	 * 언리얼 내장 Tick(), TickComponent() 대신 유연한 구현을 위한 별도 틱 함수입니다.
	 * @param DeltaTime 
	 */

	UFUNCTION(BlueprintCallable)
	ETeam GetTeamOf(const APlayerController* Player) const;
	
	UFUNCTION(BlueprintCallable)
	void Update(float DeltaTime);

	/**
	 * WaitingForStart 페이즈에서 최초 한 번만 호출되는, 게임을 시작하는 함수입니다.
	 * 이 함수 호출 이후에는 더 이상 플레이어를 추가로 등록할 수 없습니다.
	 */
	UFUNCTION(BlueprintCallable)
	void StartGame();

	UFUNCTION(BlueprintCallable)
	void GiveRoundScoreForTeam(ETeam Team, int Score);

	UFUNCTION(BlueprintCallable)
	void GiveRoundScoreForPlayer(const APlayerController* Player, int Score);

	UFUNCTION(BlueprintCallable)
	const TArray<APlayerController*>& GetRedTeamPlayers() const
	{
		return RedTeamPlayers;
	}

	UFUNCTION(BlueprintCallable)
	const TArray<APlayerController*>& GetBlueTeamPlayers() const
	{
		return BlueTeamPlayers;
	}

	UFUNCTION(BlueprintCallable)
	int GetMaxPlayersPerTeam() const
	{
		return MaxPlayersPerTeam;
	}

	UFUNCTION(BlueprintCallable)
	int GetRoundWinningScore() const
	{
		return RoundWinningScore;
	}

	UFUNCTION(BlueprintCallable)
	int GetGameWinningScore() const
	{
		return GameWinningScore;
	}

	UFUNCTION(BlueprintCallable)
	float GetRoundTime() const
	{
		return RoundTime;
	}

	/**
	 * 라운드가 종료된 후부터 다음 라운드 시작까지 대기하는 시간입니다.
	 * @return 
	 */
	UFUNCTION(BlueprintCallable)
	float GetRoundEndTime() const
	{
		return RoundEndTime;
	}

	/**
	 * 모든 게임이 종료되어 퇴장까지 대기하는 시간입니다.
	 * @return 
	 */
	UFUNCTION(BlueprintCallable)
	float GetGameEndTime() const
	{
		return RoundTime;
	}

	/**
	 * 현재 페이즈의 시간입니다. 라운드 페이즈인 경우 라운드 시간이 될 것이고, 게임 종료 페이즈인 경우 게임 종료 후 지난 시간이 될 것입니다.
	 * @return 
	 */
	UFUNCTION(BlueprintCallable)
	float GetCurrentPhaseTime() const
	{
		return CurrentPhaseTime;
	}

	UFUNCTION(BlueprintCallable)
	int GetBlueTeamRoundScore() const
	{
		return BlueTeamRoundScore;
	}

	UFUNCTION(BlueprintCallable)
	int GetRedTeamRoundScore() const
	{
		return RedTeamRoundScore;
	}

	UFUNCTION(BlueprintCallable)
	int GetBlueTeamGameScore() const
	{
		return BlueTeamGameScore;
	}

	UFUNCTION(BlueprintCallable)
	int GetRedTeamGameScore() const
	{
		return RedTeamGameScore;
	}

	UFUNCTION(BlueprintCallable)
	ECompetitiveGamePhase GetCurrentPhase() const
	{
		return CurrentPhase;
	}

private:
	UPROPERTY(meta=(BlueprintGetter="GetMaxPlayersPerTeam"))
	int MaxPlayersPerTeam = 2;

	UPROPERTY(meta=(BlueprintGetter="GetRoundWinningScore"))
	int RoundWinningScore = 100;

	UPROPERTY(meta=(BlueprintGetter="GetGameWinningScore"))
	int GameWinningScore = 5;

	UPROPERTY(meta=(BlueprintGetter="GetRoundTime"))
	float RoundTime = 180.0f;

	UPROPERTY(meta=(BlueprintGetter="GetRoundEndTime"))
	float RoundEndTime = 5.0f;

	UPROPERTY(meta=(BlueprintGetter="GetGameEndTime"))
	float GameEndTime = 10.0f;

	UPROPERTY(meta=(BlueprintGetter="GetRedTeamPlayers"))
	TArray<TObjectPtr<APlayerController>> RedTeamPlayers;

	UPROPERTY(meta=(BlueprintGetter="GetBlueTeamPlayers"))
	TArray<TObjectPtr<APlayerController>> BlueTeamPlayers;

	UPROPERTY(meta=(BlueprintGetter="GetBlueTeamRoundScore"))
	int BlueTeamRoundScore;

	UPROPERTY(meta=(BlueprintGetter="GetRedTeamRoundScore"))
	int RedTeamRoundScore;

	UPROPERTY(meta=(BlueprintGetter="GetBlueTeamGameScore"))
	int BlueTeamGameScore;

	UPROPERTY(meta=(BlueprintGetter="GetRedTeamGameScore"))
	int RedTeamGameScore;

	UPROPERTY(meta=(BlueprintGetter="GetCurrentPhaseTime"))
	float CurrentPhaseTime;

	UPROPERTY(meta=(BlueprintGetter="GetCurrentPhase"))
	ECompetitiveGamePhase CurrentPhase;

	void Update_WaitingForStart();

	void Update_Game();

	void Update_RoundEnd();

	void Update_GameEnd();
	
	void Update_GameDestroyed();
};
