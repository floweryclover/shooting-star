// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

class UCompetitiveSystemComponent;
/**
 * 
 */
UCLASS()
class ALobbyGameMode final : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALobbyGameMode();

	virtual int32 GetNumPlayers() override;

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual void Logout(AController* Exiting) override;

	virtual void SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC) override;

	UCompetitiveSystemComponent* GetCompetitiveSystemComponent() const
	{
		return CompetitiveSystemComponent;
	}
	
protected:
	UPROPERTY(BlueprintReadOnly)
	int32 NumPlayers;

	// 최대 인원수 등 경쟁모드의 여러 설정값들을 가져오기 위한 컴포넌트입니다.
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCompetitiveSystemComponent> CompetitiveSystemComponent;
};
