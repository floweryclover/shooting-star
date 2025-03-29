// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

WIFIDIRECT_API DECLARE_LOG_CATEGORY_EXTERN(LogWifiDirect, Log, All);

class WIFIDIRECT_API FWifiDirectModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};