// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CompetitiveGameMode.generated.h"

class UCompetitiveSystemComponent;

/**
 * 2대2 3선승 게임 모드입니다.
 */
UCLASS()
class ACompetitiveGameMode final : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACompetitiveGameMode();

	virtual void Tick(float DeltaSeconds) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual void Logout(AController* Exiting) override;

	virtual void SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC) override;

	virtual int32 GetNumPlayers() override
	{
		Super::GetNumPlayers();
		return NumPlayers;
	}
	
	UCompetitiveSystemComponent* GetCompetitiveSystemComponent() const
	{
		return CompetitiveSystemComponent;
	}

protected:
	UPROPERTY(BlueprintReadOnly)
	int32 NumPlayers;
	
	// 게임 입장 후 게임 시작까지 필요한 시간
	UPROPERTY(BlueprintReadOnly)
	float GameStartSeconds = 10.0f;

	// 게임 종료 후 이동할 레벨
	UPROPERTY(BlueprintReadOnly)
	FName ExitLevel = TEXT("/Game/Levels/Lobby");
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCompetitiveSystemComponent> CompetitiveSystemComponent;
};
