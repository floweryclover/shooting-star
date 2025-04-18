// Copyright 2025 ShootingStar. All Rights Reserved.


#include "WifiDirectInterface.h"

#if PLATFORM_ANDROID && USE_ANDROID_JNI
#include <Android/AndroidApplication.h>
#include <Android/AndroidJNI.h>
#endif

UWifiDirectInterface* UWifiDirectInterface::GetWifiDirectInterface()
{
	static UWifiDirectInterface* Instance;
	if (!Instance)
	{
		Instance = NewObject<UWifiDirectInterface>();
		Instance->AddToRoot();
	}
	return Instance;
}

UWifiDirectInterface::UWifiDirectInterface()
	: bIsP2pGroupFormed{false},
	  bIsP2pGroupOwner{false},
	  bIsConnecting{false},
	  DiscoveryElapsed{DiscoveryInterval * 0.8f},
	  GroupUpdateElapsed{GroupUpdateInterval * 0.8f}
{
}

void UWifiDirectInterface::Connect(const FString& DeviceAddress)
{
	bIsConnecting = true;

	if (bIsP2pGroupFormed)
	{
		OnConnectionFailed.Broadcast(DeviceAddress);
		return;
	}
	if (DeviceAddress.IsEmpty())
	{
		OnConnectionFailed.Broadcast(DeviceAddress);
		return;
	}
#if PLATFORM_ANDROID && USE_ANDROID_JNI
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();

    jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
    jmethodID ConnectToDeviceMethod = Env->GetMethodID(ActivityClass, "connect", "(Ljava/lang/String;)V");

    jstring JavaDeviceAddress = Env->NewStringUTF(TCHAR_TO_ANSI(*DeviceAddress));

    Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), ConnectToDeviceMethod, JavaDeviceAddress);

    Env->DeleteLocalRef(ActivityClass);
    Env->DeleteLocalRef(JavaDeviceAddress);
#endif
}

void UWifiDirectInterface::Clear()
{
	ShootingStarPeers.Empty();
	bIsP2pGroupFormed = false;
	bIsP2pGroupOwner = false;
	GroupOwnerIpAddress.Empty();
	DiscoveryElapsed = 0.0f;
	GroupUpdateElapsed = 0.0f;
	bIsConnecting = false;
#if PLATFORM_ANDROID && USE_ANDROID_JNI
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();

    jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
    jmethodID ClearMethod = Env->GetMethodID(ActivityClass, "clear", "()V");

    Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), ClearMethod);

    Env->DeleteLocalRef(ActivityClass);
#endif
}

void UWifiDirectInterface::StopBroadcastAndDiscovery()
{
#if PLATFORM_ANDROID && USE_ANDROID_JNI
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
	jmethodID StopBroadcastAndDiscoveryMethod = Env->GetMethodID(ActivityClass, "stopBroadcastAndDiscovery", "()V");

	Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), StopBroadcastAndDiscoveryMethod);

	Env->DeleteLocalRef(ActivityClass);
#endif
}

void UWifiDirectInterface::RegisterService()
{
	DiscoveryElapsed = DiscoveryInterval * 0.8f;
#if PLATFORM_ANDROID && USE_ANDROID_JNI
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
	jmethodID RegisterServiceMethod = Env->GetMethodID(ActivityClass, "registerService", "()V");

	Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), RegisterServiceMethod);

	Env->DeleteLocalRef(ActivityClass);
#endif
}

void UWifiDirectInterface::CheckAndRequestPermissions()
{
#if PLATFORM_ANDROID && USE_ANDROID_JNI
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
	jmethodID CheckAndRequestPermissionsMethod = Env->GetMethodID(ActivityClass, "checkAndRequestPermissions", "()V");

	Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), CheckAndRequestPermissionsMethod);

	Env->DeleteLocalRef(ActivityClass);
#endif
}

void UWifiDirectInterface::CancelConnect()
{
	bIsConnecting = false;
#if PLATFORM_ANDROID && USE_ANDROID_JNI
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
	jmethodID CancelConnectMethod = Env->GetMethodID(ActivityClass, "cancelConnect", "()V");

	Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), CancelConnectMethod);

	Env->DeleteLocalRef(ActivityClass);
#endif
}

void UWifiDirectInterface::Update(const float DeltaSeconds)
{
	DiscoveryElapsed += DeltaSeconds;
	if (DiscoveryElapsed >= DiscoveryInterval)
	{
		DiscoveryElapsed = 0.0f;
		if (!bIsP2pGroupFormed)
		{
			RefreshServiceDiscovery();
		}
		else if (!bIsP2pGroupOwner)
		{
			StopBroadcastAndDiscovery();
		}
	}
	
	GroupUpdateElapsed += DeltaSeconds;
	if (GroupUpdateElapsed >= GroupUpdateInterval)
	{
		GroupUpdateElapsed = 0.0f;
		RefreshGroupInfo();
	}
}

void UWifiDirectInterface::RefreshGroupInfo()
{
#if PLATFORM_ANDROID && USE_ANDROID_JNI
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();

    jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
    jmethodID RefreshGroupInfoMethod = Env->GetMethodID(ActivityClass, "refreshGroupInfo", "()V");

    Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), RefreshGroupInfoMethod);

    Env->DeleteLocalRef(ActivityClass);
#endif
}

void UWifiDirectInterface::RefreshServiceDiscovery()
{
	if (bIsConnecting)
	{
		return;
	}
	ShootingStarPeers.Empty();
#if PLATFORM_ANDROID && USE_ANDROID_JNI
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
	jmethodID RefreshServiceDiscoveryMethod = Env->GetMethodID(ActivityClass, "refreshServiceDiscovery", "()V");

	Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), RefreshServiceDiscoveryMethod);

	Env->DeleteLocalRef(ActivityClass);
#endif
}

void UWifiDirectInterface::OnServiceFound(const FString& DeviceName, const FString& DeviceMacAddress)
{
	if (!ShootingStarPeers.ContainsByPredicate([&DeviceMacAddress](const auto& DeviceInfo)
	{
		return DeviceInfo.DeviceMacAddress == DeviceMacAddress;
	}))
	{
		ShootingStarPeers.Push(FWifiDirectPeerDeviceInfo{DeviceName, DeviceMacAddress});
	}
}

void UWifiDirectInterface::OnConnectionFailedCallback(const FString& DeviceName, const FString& DeviceMacAddress)
{
	bIsConnecting = false;
	for (int i = ShootingStarPeers.Num() - 1; i >= 0; --i)
	{
		const FWifiDirectPeerDeviceInfo& DeviceInfo = ShootingStarPeers[i];
		if (DeviceInfo.DeviceMacAddress == DeviceMacAddress)
		{
			ShootingStarPeers.RemoveAt(i);
		}
	}
	OnConnectionFailed.Broadcast(DeviceName);
}

#if PLATFORM_ANDROID && USE_ANDROID_JNI
extern "C" JNIEXPORT void JNICALL Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnErrorFunction(JNIEnv * Env, jclass Clazz, jstring JavaError)
{
	FString Error;
	if (JavaError != nullptr)
	{
		const char* ErrorChars = Env->GetStringUTFChars(JavaError, nullptr);
		Error = FString(UTF8_TO_TCHAR(ErrorChars));
		Env->ReleaseStringUTFChars(JavaError, ErrorChars);
	}
    
	FFunctionGraphTask::CreateAndDispatchWhenReady([Error]()
		{
			UWifiDirectInterface::GetWifiDirectInterface()->OnWifiDirectError.Broadcast(Error);
		}, TStatId(), nullptr, ENamedThreads::GameThread);
}

extern "C" JNIEXPORT void JNICALL Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnConnectionFailedFunction(JNIEnv * Env, jclass Clazz, jstring JavaDeviceName, jstring JavaDeviceMacAddress)
{
	FString DeviceName;
	if (JavaDeviceName != nullptr)
	{
		const char* DeviceNameChars = Env->GetStringUTFChars(JavaDeviceName, nullptr);
		DeviceName = FString(UTF8_TO_TCHAR(DeviceNameChars));
		Env->ReleaseStringUTFChars(JavaDeviceName, DeviceNameChars);
	}
	
	FString DeviceMacAddress;
	if (JavaDeviceMacAddress != nullptr)
	{
		const char* AddressChars = Env->GetStringUTFChars(JavaDeviceMacAddress, nullptr);
		DeviceMacAddress = FString(UTF8_TO_TCHAR(AddressChars));
		Env->ReleaseStringUTFChars(JavaDeviceMacAddress, AddressChars);
	}
    
	FFunctionGraphTask::CreateAndDispatchWhenReady([DeviceName, DeviceMacAddress]()
		{
			UWifiDirectInterface::GetWifiDirectInterface()->OnConnectionFailedCallback(DeviceName, DeviceMacAddress);
		}, TStatId(), nullptr, ENamedThreads::GameThread);
}

extern "C" JNIEXPORT void JNICALL Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnRefreshGroupFunction(JNIEnv * Env, jclass Clazz, jboolean JavaIsGroupFormed, jboolean JavaIsGroupOwner, jstring JavaGroupOwnerIpAddress)
{
	bool bIsGroupFormed = (JavaIsGroupFormed == JNI_TRUE);
	bool bIsGroupOwner = (JavaIsGroupOwner == JNI_TRUE);
    
	FString GroupOwnerIp;
	if (JavaGroupOwnerIpAddress != nullptr)
	{
		const char* IpChars = Env->GetStringUTFChars(JavaGroupOwnerIpAddress, nullptr);
		GroupOwnerIp = FString(UTF8_TO_TCHAR(IpChars));
		Env->ReleaseStringUTFChars(JavaGroupOwnerIpAddress, IpChars);
	}
    
	FFunctionGraphTask::CreateAndDispatchWhenReady([bIsGroupFormed, bIsGroupOwner, GroupOwnerIp]()
	{
		UWifiDirectInterface* const Interface = UWifiDirectInterface::GetWifiDirectInterface();
		Interface->bIsP2pGroupFormed = bIsGroupFormed;
		Interface->bIsP2pGroupOwner = bIsGroupOwner;
		Interface->GroupOwnerIpAddress = GroupOwnerIp;
	}, TStatId(), nullptr, ENamedThreads::GameThread);
}

extern "C" JNIEXPORT void JNICALL Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnServiceFoundFunction(JNIEnv * Env, jclass Clazz, jstring JavaDeviceName, jstring JavaDeviceMacAddress)
{
	FString DeviceName;
	if (JavaDeviceName != nullptr)
	{
		const char* NameChars = Env->GetStringUTFChars(JavaDeviceName, nullptr);
		DeviceName = FString(UTF8_TO_TCHAR(NameChars));
		Env->ReleaseStringUTFChars(JavaDeviceName, NameChars);
	}
	
	FString DeviceMacAddress;
	if (JavaDeviceMacAddress != nullptr)
	{
		const char* AddressChars = Env->GetStringUTFChars(JavaDeviceMacAddress, nullptr);
		DeviceMacAddress = FString(UTF8_TO_TCHAR(AddressChars));
		Env->ReleaseStringUTFChars(JavaDeviceMacAddress, AddressChars);
	}
    
	FFunctionGraphTask::CreateAndDispatchWhenReady([DeviceName, DeviceMacAddress]()
	{
		UWifiDirectInterface* const Interface = UWifiDirectInterface::GetWifiDirectInterface();
		Interface->OnServiceFound(DeviceName, DeviceMacAddress);
	}, TStatId(), nullptr, ENamedThreads::GameThread);
}

#endif
