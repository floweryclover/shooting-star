// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CompetitiveSystemComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CompetitiveGameState.generated.h"

class ASupplyActor;

UENUM(BlueprintType)
enum class EPlayerDeadReason : uint8
{
	Unknown,
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPlayerDead, const FString&, Killee, const FString&, Killer, UClass*, Cause);

UCLASS()
class SHOOTINGSTAR_API ACompetitiveGameState final : public AGameStateBase
{
	GENERATED_BODY()

public:
	ACompetitiveGameState();

	/**
	 * 마지막 인자는 ARockerLauncher, ASafeZoneActor의 StaticClass를 전달합니다.
	 * nullptr일 수 있습니다.
	 */
	UPROPERTY(BlueprintAssignable)
	FPlayerDead OnPlayerDead;
	
	virtual void Tick(float DeltaSeconds) override;

	int GetBlueTeamKills() const
	{
		return BlueTeamKills;
	}

	int GetRedTeamKills() const
	{
		return RedTeamKills;
	}

	int GetBlueTeamWinRounds() const
	{
		return BlueTeamWinRounds;
	}

	int GetRedTeamWinRounds() const
	{
		return RedTeamWinRounds;
	}

	bool IsGoldenKillTime() const
	{
		return bIsGoldenKillTime;
	}

	float GetPhaseTime() const
	{
		return PhaseTime;
	}

	ECompetitiveGamePhase GetPhase() const
	{
		return Phase;
	}
	
	ETeam GetLastRoundWinTeam() const
	{
		return LastRoundWinTeam;
	}

	const TArray<ASupplyActor*>& GetSupplyActors() const
	{
		return SupplyActors;
	}
	
	UFUNCTION(Reliable, NetMulticast)
	void MulticastPlayerDead(const FString& Killee, const FString& Killer, UClass* Cause);

protected:
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	float WaitingForGameStartTime;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	int BlueTeamKills;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int RedTeamKills;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int BlueTeamWinRounds;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int RedTeamWinRounds;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float PhaseTime;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float RemainingGameTime;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsGoldenKillTime;

	UPROPERTY(Replicated, BlueprintReadOnly)
	ETeam LastRoundWinTeam;

	UPROPERTY(Replicated, BlueprintReadOnly)
	ECompetitiveGamePhase Phase;

	UPROPERTY(Replicated, BlueprintReadOnly)
	TArray<ASupplyActor*> SupplyActors;

private:
	UFUNCTION()
	void OnSupplyDroppedHandler(FVector Location);
};
