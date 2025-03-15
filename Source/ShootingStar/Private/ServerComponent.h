// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ServerComponent.generated.h"

/**
 * 서버로서 원본 컴퓨터가 처리해야 하는 확정된 RPC 함수들이 작성된 Actor Component입니다.
 * @details
 * 예상 시나리오는,
 * 클라이언트의 다른 로직(UI, Character 등)이 멀티플레이어 동작을 위해
 * 자신이 소유한 Server Component의 RPC 함수를 호출하는 상황입니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UServerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UServerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * 클라이언트의 원격 컴퓨터에서 자신이 소유한 Server Component를 통해 이 함수를 호출하면,
	 * 서버 측의 원본 컴퓨터의 해당 Server Component에서 실제 로직이 호출됩니다.
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerPleaseMakeMeJump();

	/**
	 * 클라이언트가 서버를 거치지 않고 SpawnActor하는 것을 시뮬레이션한 함수입니다.
	 */
	UFUNCTION(BlueprintCallable)
	void BadSpawn(UClass* ActorToSpawn);
	
	/**
	 * 클라이언트가 서버에게 SpawnActor를 정상적으로 요청하는 것을 시뮬레이션한 함수입니다.
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void GoodSpawn(UClass* ActorToSpawn);
};
