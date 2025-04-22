// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ServerComponent.generated.h"

struct FWeaponData;
/**
 * 서버로서 원본 컴퓨터가 처리해야 하는 확정된 RPC 함수들이 작성된 Actor Component입니다.
 * @details
 * 예상 시나리오는,
 * 클라이언트의 다른 로직(UI, Character 등)이 멀티플레이어 동작을 위해
 * 자신이 소유한 Server Component의 RPC 함수를 호출하는 상황입니다.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTINGSTAR_API UServerComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Reliable, Server)
	void RequestInteractResource();

	UFUNCTION(BlueprintCallable, Reliable, Server)
	void RequestCraftWeapon(const FWeaponData& Weapon, const TArray<int32>& Resources);
};
