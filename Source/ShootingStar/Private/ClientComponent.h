// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClientComponent.generated.h"

/**
 * 클라이언트로서 각 원격 컴퓨터가 처리해야 하는 확정된 RPC 함수들이 작성된 Actor Component입니다.
 * @details
 * 예상 시나리오는,
 * 서버의 GameMode 로직이 특정 플레이어의 RPC 호출을 위해
 * 해당 플레이어의 ClientComponent의 함수를 호출하는 상황입니다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UClientComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UClientComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * 서버의 원본 컴퓨터에서 특정 Client Component를 통해 이 함수를 호출하면,
	 * 그것을 소유한 클라이언트 측의 원격 컴퓨터의 해당 Client Component에서 실제 로직이 호출됩니다.
	 */
	UFUNCTION(Client, Reliable)
	void JumpClient();
};
