// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClientComponent.generated.h"

struct FWeaponData;
class UResourceDataAsset;

// 서버->클라이언트 무기 제작 완료 통지용 임시 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponGained, const FWeaponData&, Weapon);


/**
 * 클라이언트로서 각 원격 컴퓨터가 처리해야 하는 확정된 RPC 함수들이 작성된 Actor Component입니다.
 * @details
 * 예상 시나리오는,
 * 서버의 GameMode 로직이 특정 플레이어의 RPC 호출을 위해
 * 해당 플레이어의 ClientComponent의 함수를 호출하는 상황입니다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHOOTINGSTAR_API UClientComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	// 서버->클라이언트 무기 제작 통지용 임시 델리게이트
	UPROPERTY(BlueprintAssignable)
	FWeaponGained OnWeaponGained;
	
	UFUNCTION(BlueprintCallable, Reliable, Client)
	void GainResource(UResourceDataAsset* Resource);

	UFUNCTION(BlueprintCallable, Reliable, Client)
	void GainWeapon(const FWeaponData& Weapon);
};
