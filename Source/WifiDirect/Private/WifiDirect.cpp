// Copyright 2025 ShootingStar. All Rights Reserved.

#include "WifiDirect.h"
#include "WifiDirectInterface.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FWifiDirectModule"

void FWifiDirectModule::StartupModule()
{
	UWifiDirectInterface::GetWifiDirectInterface();
}

void FWifiDirectModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FWifiDirectModule, WifiDirect);

DEFINE_LOG_CATEGORY(LogWifiDirect)
 