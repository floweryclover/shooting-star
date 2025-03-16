// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LobbyNetworkComponent.generated.h"

/**
 * 게임 시작, 나가기 등 로비에서의 멀티플레이어 관련 동작을 담당하는 컴포넌트입니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ULobbyNetworkComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/**
	 * 클라이언트에서 호출하는 경우 아무것도 하지 않습니다.
	 * 서버인 경우 즉시 게임을 시작합니다(경쟁 모드).
	 */
	UFUNCTION(BlueprintCallable)
	void StartGame();

	/**
	 * 클라이언트에서 호출하는 경우 자신만 나갑니다.
	 * 서버인 경우 방이 폭파되어 모두 메인화면으로 나가집니다.
	 */
	UFUNCTION(BlueprintCallable)
	void LeaveGame();
};
