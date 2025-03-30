// Copyright 2025 ShootingStar. All Rights Reserved.


#include "AndroidFunctionLibrary.h"
#include "AndroidPermissionFunctionLibrary.h"

bool UAndroidFunctionLibrary::CheckPermission(const FString& Permission)
{
	return UAndroidPermissionFunctionLibrary::CheckPermission(Permission);
}

UAndroidPermissionCallbackProxy* UAndroidFunctionLibrary::AcquirePermissions(const TArray<FString>& Permissions)
{
	return UAndroidPermissionFunctionLibrary::AcquirePermissions(Permissions);
}
