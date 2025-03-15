// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PendingMultiplayerComponent.generated.h"

/**
 * 멀티플레이어 동기화를 적용시킬 대상 함수들을 작성해 놓는 클래스입니다.
 * 여기 작성해 놓고, 주석 등의 방법으로 자세한 함수 흐름 등을 설명해 놓으면,
 * ServerComponent와 ClientComponent로 담당 파트가 RPC 함수로 작성할 것입니다.
 *
 * 절차적 생성, 아이템 생성등의 핵심 코드는 GameMode나 각자의 ActorComponent 등의 형태로 작성하시고,
 * 여기에는 명확한 시작 함수의 형태가 되도록 작성해 주시면 좋을 것 같습니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UPendingMultiplayerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPendingMultiplayerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	/*
	 * 예시
	 *
	 * 
	 *  
	 * 이 함수는 자원 채집 시작시 호출되는 함수입니다. MyCharacter의 SomeVariable을 변경시킵니다.
	 * void BeginResourceGather();
	 *
	 * 이 함수는 자원 채집 종료시 호출되는 함수입니다.
	 * void EndResourceGather(); 
	 *
	 * 이 함수는 광석의 절차적 생성을 시작하는 함수입니다.
	 * 실제 로직은 MyGameMode::OreGeneration()에 작성해 두었습니다.
	 * void OreGeneration_Pending();
	 *
	 * 이 함수는 인벤토리에서 특정 아이템을 제작 시작하는 함수입니다.
	 * 실제 로직은 이 캐릭터의 부착해 놓은 SomeOtherActorComponent::MakeItem()에 작성해 두었습니다.
	 * void MakeItem_Pending();
	 *
	 */
};
