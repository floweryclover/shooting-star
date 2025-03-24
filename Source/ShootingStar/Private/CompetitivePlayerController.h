// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CompetitivePlayerController.generated.h"

class UTeamComponent;

/**
 *
 */
UCLASS()
class ACompetitivePlayerController final : public APlayerController
{
	GENERATED_BODY()

public:
	ACompetitivePlayerController();
	
	UTeamComponent* GetTeamComponent() const
	{
		return TeamComponent;
	}
	
	/**
	 * Actor에 부착되어 공격 판정 등에 이용되는 일반적인 TeamComponent와는 달리,
	 * 이 컴포넌트는 게임 동안의 플레이어의 소속 팀을 정의하며, 레벨 이동시 소속 팀 유지를 위해 사용됩니다.
	 */
	UPROPERTY(BlueprintReadOnly)
	UTeamComponent* TeamComponent;
};
