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
 * 플레이어 캐릭터나 기타 액터들의 공격 이벤트와 판정 등을 위해 정의하였습니다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UTeamComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	UTeamComponent();
	
	UPROPERTY(BlueprintReadWrite, Replicated)
	ETeam Team;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
