// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AndroidFunctionLibrary.generated.h"

class UAndroidPermissionCallbackProxy;

/**
 * 
 */
UCLASS()
class WIFIDIRECT_API UAndroidFunctionLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static bool CheckPermission(const FString& Permission);

	UFUNCTION(BlueprintCallable)
	static UAndroidPermissionCallbackProxy* AcquirePermissions(const TArray<FString>& Permissions);
};
