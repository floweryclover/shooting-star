// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CompetitiveGameMode.generated.h"

class UCompetitiveSystemComponent;

/**
 * 2대2 5판3선승 게임 모드입니다.
 */
UCLASS()
class ACompetitiveGameMode final : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACompetitiveGameMode();

	UFUNCTION(BlueprintCallable)
	UCompetitiveSystemComponent* GetCompetitiveSystemComponent()
	{
		return CompetitiveSystemComponent;
	}
	
private:
	UPROPERTY(meta=(BlueprintGetter=GetCompetitiveSystemComponent))
	UCompetitiveSystemComponent* CompetitiveSystemComponent;
};
