// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TeamComponent.generated.h"

UENUM(BlueprintType)
enum class ETeam : uint8
{
	None,
	Red,
	Blue
};

/**
 * 액터의 소속 팀을 정의하는 컴포넌트입니다.
 * 플레이어의 경우, 캐릭터와 컨트롤러에 모두 부착하여야 합니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UTeamComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	ETeam Team;
};
