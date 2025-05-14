// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogShootingStar, Log, All);

// Channels
namespace CollisionChannels
{
	constexpr ECollisionChannel ResourceActor = ECC_GameTraceChannel1;
}

#define FAIL_IF_NOT_SERVER()																															\
	if (!HasAuthority())																																\
{																																					    \
	UE_LOG(LogShootingStar,																															    \
		Error,																																	        \
		TEXT("서버에서만 호출되어야 하는 함수가 클라이언트에서 호출되었습니다. 파일: %hs, 함수: %hs, 라인: %d."), __FILE__, __FUNCTION__, __LINE__)     \
	return;																																				\
}																																					    \
else																																					\
	((void*)0)