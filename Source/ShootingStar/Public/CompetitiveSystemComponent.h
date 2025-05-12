// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "TeamComponent.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CompetitiveGameMode.h"
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameStarted);

// 보급 이벤트 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSupplyDropEvent, FVector, Location);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSupplyOpenEvent, FVector, Location);

/**
* 3선승 게임인 경쟁 모드의 동작과 상태를 정의하는 컴포넌트입니다.
* @details RoundScore는 한 라운드에서 각 팀이 얻은 점수를, GameScore는 승리한 라운드 수를 의미합니다.
*/
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHOOTINGSTAR_API UCompetitiveSystemComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	//
	// 상수들. 다른 클래스에서 항상 동일한 값 참조를 위해 static constexpr로 정의하였습니다.
	//
	static constexpr int32 MaxPlayersPerTeam = 2;
	
	static constexpr int32 RoundWinningKillScore = 5;
	
	static constexpr int32 GameWinningScore = 3;

	// 한 라운드 타임입니다.
	static constexpr float GameTime = 180.0f;

	// GameEnd에서 GameDestroyed 상태로 전이까지 필요한 시간
	static constexpr float GameEndTime = 10.0f;
	
	// 라운드가 종료된 후부터 다음 라운드 시작까지 대기하는 시간입니다. 
	static constexpr float RoundEndTime = 5.0f;

	static constexpr float SafeZoneShrinkStartTime = 30.f;
	
	static constexpr float SafeZoneShrinkDuration = 120.f;

	// 보급품 드롭 타임
	static constexpr int NumSupplies = 3;
	static constexpr float SupplyDropTimes[] = {3.f, 10.f, 20.f};
	
	/**
	 * Game 상태로 처음 전이될 때 발생하는 이벤트.
	 */
	UPROPERTY(BlueprintAssignable)
	FGameStarted OnGameStarted;

	// 보급품 생성 이벤트
	UPROPERTY(BlueprintAssignable)
	FSupplyDropEvent OnSupplyDropped;

	// 보급품 오픈 이벤트
	UPROPERTY(BlueprintAssignable)
	FSupplyOpenEvent OnSupplyOpened;
	
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
	static ETeam GetTeamForNextPlayer(const TArray<APlayerState*>& PlayerArray);

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

	// 자기장 알파값 getter
	UFUNCTION(BlueprintCallable)
	float GetSafeZoneAlpha() const { return SafeZoneAlpha; }

	ETeam GetLastRoundWinTeam() const
	{
		return LastRoundWinTeam;
	}

protected:
	virtual void BeginPlay() override;

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

	UPROPERTY(BlueprintReadOnly)
	ETeam LastRoundWinTeam{};
	
	// 현재 자기장 알파값 (0: 초기 크기, 1: 최종 크기)
	UPROPERTY()
	float SafeZoneAlpha;

	// 보급품 트리거 배열
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<bool> SupplyDropsTriggered;

	UPROPERTY()
	class ACompetitiveGameMode* GameMode;

private:
	void Update_WaitingForStart();

	void Update_Game();

	void Update_RoundEnd();

	void Update_GameEnd();
	
	void Update_GameDestroyed();

	void UpdateSafeZoneAlpha(float GameTime);

	void CheckAndTriggerSupplyDrop(float GameTime);

	void WinTeam(ETeam Team);
};
