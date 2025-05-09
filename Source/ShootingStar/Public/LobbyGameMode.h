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
class SHOOTINGSTAR_API ALobbyGameMode final : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALobbyGameMode();

	virtual int32 GetNumPlayers() override;

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual void Logout(AController* Exiting) override;

	virtual void SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC) override;
	
protected:
	UPROPERTY(BlueprintReadOnly)
	int32 NumPlayers;
	
	bool bIsDestroying;
};
