// Copyright 2025 ShootingStar. All Rights Reserved.


#include "WifiDirectInterface.h"

#include "WifiDirect.h"

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
	: bIsP2pServiceAdded{false},
	  bIsP2pGroupFormed{false},
	  bIsP2pGroupOwner{false},
	  bIsP2pAvailable{false},
	  bIsP2pPeerDiscovering{false}
{
	OnDiscoverPeersFailed.AddDynamic(this, &UWifiDirectInterface::ClearPeerList);
	OnAddLocalServiceFailed.AddDynamic(this, &UWifiDirectInterface::ClearIsP2pServiceAdded);
}

void UWifiDirectInterface::ConnectToNearbyDevice(const FString& DeviceAddress)
{
	if (bIsP2pGroupFormed)
	{
		OnConnectToPeerFailed.Broadcast(-1);
		return;
	}
	if (DeviceAddress.IsEmpty())
	{
		OnConnectToPeerFailed.Broadcast(-2);
		return;
	}
#if PLATFORM_ANDROID && USE_ANDROID_JNI
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();

    jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
    jmethodID ConnectToDeviceMethod = Env->GetMethodID(ActivityClass, "connectToDevice", "(Ljava/lang/String;)V");

    jstring JavaDeviceAddress = Env->NewStringUTF(TCHAR_TO_ANSI(*DeviceAddress));

    Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), ConnectToDeviceMethod, JavaDeviceAddress);

    Env->DeleteLocalRef(ActivityClass);
    Env->DeleteLocalRef(JavaDeviceAddress);
#endif
}

void UWifiDirectInterface::StartPeerDiscovering()
{
	if (bIsP2pGroupFormed)
	{
		OnDiscoverPeersFailed.Broadcast(-1);
		return;
	}
#if PLATFORM_ANDROID && USE_ANDROID_JNI
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();

    jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
    jmethodID DiscoverPeersMethod = Env->GetMethodID(ActivityClass, "discoverPeers", "()V");

    Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), DiscoverPeersMethod);

    Env->DeleteLocalRef(ActivityClass);
#endif
}

void UWifiDirectInterface::StopPeerDiscovering()
{
	if (bIsP2pGroupFormed)
	{
		OnDiscoverPeersFailed.Broadcast(-1);
		return;
	}
#if PLATFORM_ANDROID && USE_ANDROID_JNI
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();

    jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
    jmethodID StopPeerDiscoveryMethod = Env->GetMethodID(ActivityClass, "stopPeerDiscovery", "()V");

    Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), StopPeerDiscoveryMethod);

    Env->DeleteLocalRef(ActivityClass);
#endif
}

void UWifiDirectInterface::Refresh()
{
	RefreshP2pAvailability();
	if (!bIsP2pAvailable)
	{
		return;
	}

	if (!bIsP2pServiceAdded)
	{
		bIsP2pServiceAdded = true;
		AddLocalService();
	}

	if (!bIsP2pGroupFormed && !bIsP2pPeerDiscovering)
	{
		StartPeerDiscovering();
	}

	RefreshDiscoveryState();
	RefreshGroupInfo();
	RefreshPeerList();
	OnWifiDirectUpdated.Broadcast();
}

void UWifiDirectInterface::Reset()
{
	GeneralPeers.Empty();
	ShootingStarPeers.Empty();
	bIsP2pGroupFormed = false;
	bIsP2pGroupOwner = false;
	bIsP2pAvailable = false;
	bIsP2pPeerDiscovering = false;
	GroupOwnerIpAddress.Empty();

	StopPeerDiscovering();
	RemoveGroup();
	RemoveLocalService();
	OnWifiDirectUpdated.Broadcast();
}

void UWifiDirectInterface::StopDiscovering()
{
	StopPeerDiscovering();
	RemoveLocalService();
}

void UWifiDirectInterface::RefreshP2pAvailability()
{
#if PLATFORM_ANDROID && USE_ANDROID_JNI
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();

    jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
    jmethodID RequestP2pStateMethod = Env->GetMethodID(ActivityClass, "requestP2pState", "()V");

    Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), RequestP2pStateMethod);

    Env->DeleteLocalRef(ActivityClass);
#endif
}

void UWifiDirectInterface::RefreshPeerList()
{
	// ShootingStarPeers에 추가해야 할 피어가 있다면 추가
	for (const FWifiDirectPeerDeviceInfo& GeneralPeer : GeneralPeers)
	{
		if (!ShootingStarPeers.ContainsByPredicate([&GeneralPeer](const auto& ShootingStarPeer)
		{
			return ShootingStarPeer.DeviceMacAddress.Equals(GeneralPeer.DeviceMacAddress, ESearchCase::IgnoreCase);
		}))
		{
			ShootingStarPeers.Push(GeneralPeer);
		}
	}

	// ShootingStarPeers에 유효하지 않은 피어가 있다면 삭제
	for (int32 i = ShootingStarPeers.Num() - 1; i >= 0; --i)
	{
		const FWifiDirectPeerDeviceInfo& ShootingStarPeer = ShootingStarPeers[i];
		bool bFound = false;
		for (const FWifiDirectPeerDeviceInfo& GeneralPeer : GeneralPeers)
		{
			if (ShootingStarPeer.DeviceMacAddress.Equals(GeneralPeer.DeviceMacAddress, ESearchCase::IgnoreCase))
			{
				bFound = true;
				break;
			}
		}

		if (!bFound)
		{
			ShootingStarPeers.RemoveAt(i);
		}
	}

	OnWifiDirectUpdated.Broadcast();
#if PLATFORM_ANDROID && USE_ANDROID_JNI
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();

    jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
    jmethodID RequestPeersMethod = Env->GetMethodID(ActivityClass, "requestPeers", "()V");

    Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), RequestPeersMethod);

    Env->DeleteLocalRef(ActivityClass);
#endif
}

void UWifiDirectInterface::RefreshDiscoveryState()
{
#if PLATFORM_ANDROID && USE_ANDROID_JNI
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();

    jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
    jmethodID RequestDiscoveryStateMethod = Env->GetMethodID(ActivityClass, "requestDiscoveryState", "()V");

    Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), RequestDiscoveryStateMethod);

    Env->DeleteLocalRef(ActivityClass);
#endif
}

void UWifiDirectInterface::RefreshGroupInfo()
{
#if PLATFORM_ANDROID && USE_ANDROID_JNI
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();

    jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
    jmethodID RequestConnectionInfoMethod = Env->GetMethodID(ActivityClass, "requestConnectionInfo", "()V");

    Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), RequestConnectionInfoMethod);

    Env->DeleteLocalRef(ActivityClass);
#endif
}

void UWifiDirectInterface::ClearPeerList(const int32 ErrorCode)
{
	ShootingStarPeers.Empty();
	GeneralPeers.Empty();
	UE_LOG(LogWifiDirect, Error, TEXT("WiFi Direct discoverPeers() error: %d"), ErrorCode);
	OnWifiDirectUpdated.Broadcast();
}

void UWifiDirectInterface::ClearIsP2pServiceAdded(const int32 ErrorCode)
{
	bIsP2pServiceAdded = false;
	UE_LOG(LogWifiDirect, Error, TEXT("WiFi Direct addLocalService() error: %d"), ErrorCode);
	OnWifiDirectUpdated.Broadcast();
}

void UWifiDirectInterface::RemoveLocalService()
{
	bIsP2pServiceAdded = false;
#if PLATFORM_ANDROID && USE_ANDROID_JNI
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
	jmethodID RemoveLocalServiceMethod = Env->GetMethodID(ActivityClass, "removeLocalService", "()V");

	Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), RemoveLocalServiceMethod);

	Env->DeleteLocalRef(ActivityClass);
#endif
}

void UWifiDirectInterface::RemoveGroup()
{
#if PLATFORM_ANDROID && USE_ANDROID_JNI
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();

    jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
    jmethodID RemoveGroupMethod = Env->GetMethodID(ActivityClass, "removeGroup", "()V");

    Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), RemoveGroupMethod);

    Env->DeleteLocalRef(ActivityClass);
#endif
}

void UWifiDirectInterface::AddLocalService()
{
#if PLATFORM_ANDROID && USE_ANDROID_JNI
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();

	jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
	jmethodID AddLocalServiceMethod = Env->GetMethodID(ActivityClass, "addLocalService", "()V");

	Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), AddLocalServiceMethod);

	Env->DeleteLocalRef(ActivityClass);
#endif
}

#if PLATFORM_ANDROID && USE_ANDROID_JNI
extern "C" JNIEXPORT void JNICALL Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectDiscoverPeersErrorFunction(JNIEnv * Env, jclass clazz, jint JavaErrorCode)
{
    int32 ErrorCode;
    ErrorCode = static_cast<int32>(JavaErrorCode);
    
    FFunctionGraphTask::CreateAndDispatchWhenReady([ErrorCode]()
        {
            UWifiDirectInterface::GetWifiDirectInterface()->OnDiscoverPeersFailed.Broadcast(ErrorCode);
        }, TStatId(), nullptr, ENamedThreads::GameThread);
}

extern "C" JNIEXPORT void JNICALL Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectConnectErrorFunction(JNIEnv * Env, jclass clazz, jint JavaErrorCode)
{
    int32 ErrorCode;
    ErrorCode = static_cast<int32>(JavaErrorCode);
    
    FFunctionGraphTask::CreateAndDispatchWhenReady([ErrorCode]()
        {
            UWifiDirectInterface::GetWifiDirectInterface()->OnConnectToPeerFailed.Broadcast(ErrorCode);
        }, TStatId(), nullptr, ENamedThreads::GameThread);
}

extern "C" JNIEXPORT void JNICALL Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectAddLocalServiceErrorFunction(JNIEnv * Env, jclass clazz, jint JavaErrorCode)
{
	int32 ErrorCode;
	ErrorCode = static_cast<int32>(JavaErrorCode);
    
	FFunctionGraphTask::CreateAndDispatchWhenReady([ErrorCode]()
		{
			UWifiDirectInterface::GetWifiDirectInterface()->OnAddLocalServiceFailed.Broadcast(ErrorCode);
		}, TStatId(), nullptr, ENamedThreads::GameThread);
}

extern "C" JNIEXPORT void JNICALL Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectRefreshPeerListFunction(JNIEnv * Env, jclass clazz, jobjectArray peerDeviceNames, jobjectArray peerDeviceAddresses)
{
    jsize arrayLength = Env->GetArrayLength(peerDeviceNames);
    
    TArray<FWifiDirectPeerDeviceInfo> WifiDirectPeers;
    WifiDirectPeers.Reserve(arrayLength);
    
    for (jsize i = 0; i < arrayLength; i++)
    {
        jstring jName = (jstring)Env->GetObjectArrayElement(peerDeviceNames, i);
        jstring jAddress = (jstring)Env->GetObjectArrayElement(peerDeviceAddresses, i);
        
        const char* nameChars = Env->GetStringUTFChars(jName, nullptr);
        const char* addressChars = Env->GetStringUTFChars(jAddress, nullptr);
        
        FString DeviceName = FString(UTF8_TO_TCHAR(nameChars));
        FString DeviceMacAddress = FString(UTF8_TO_TCHAR(addressChars));
        
        Env->ReleaseStringUTFChars(jName, nameChars);
        Env->ReleaseStringUTFChars(jAddress, addressChars);
        
        Env->DeleteLocalRef(jName);
        Env->DeleteLocalRef(jAddress);
        
        FWifiDirectPeerDeviceInfo PeerInfo;
        PeerInfo.DeviceName = DeviceName;
        PeerInfo.DeviceMacAddress = DeviceMacAddress;
        WifiDirectPeers.Add(PeerInfo);
    }
    
    FFunctionGraphTask::CreateAndDispatchWhenReady([WifiDirectPeers]()
    {
    	UWifiDirectInterface* const Interface = UWifiDirectInterface::GetWifiDirectInterface();
    	Interface->GeneralPeers = WifiDirectPeers;
    }, TStatId(), nullptr, ENamedThreads::GameThread);
}

extern "C" JNIEXPORT void JNICALL Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectRefreshGroupFunction(JNIEnv * Env, jclass clazz, jboolean isGroupFormed, jboolean isGroupOwner, jstring groupOwnerIpAddress)
{
    bool bIsGroupFormed = (isGroupFormed == JNI_TRUE);
    bool bIsGroupOwner = (isGroupOwner == JNI_TRUE);
    
    FString GroupOwnerIp;
    if (groupOwnerIpAddress != nullptr)
    {
        const char* ipChars = Env->GetStringUTFChars(groupOwnerIpAddress, nullptr);
        GroupOwnerIp = FString(UTF8_TO_TCHAR(ipChars));
        Env->ReleaseStringUTFChars(groupOwnerIpAddress, ipChars);
    }
    
    FFunctionGraphTask::CreateAndDispatchWhenReady([bIsGroupFormed, bIsGroupOwner, GroupOwnerIp]()
    {
        UWifiDirectInterface* const Interface = UWifiDirectInterface::GetWifiDirectInterface();
        Interface->bIsP2pGroupFormed = bIsGroupFormed;
        Interface->bIsP2pGroupOwner = bIsGroupOwner;
        Interface->GroupOwnerIpAddress = GroupOwnerIp;
        Interface->OnWifiDirectUpdated.Broadcast();
    }, TStatId(), nullptr, ENamedThreads::GameThread);
}

extern "C" JNIEXPORT void JNICALL Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectRefreshP2pStateFunction(JNIEnv * Env, jclass clazz, jboolean isAvailable)
{
    bool bIsAvailable = (isAvailable == JNI_TRUE);
    
    FFunctionGraphTask::CreateAndDispatchWhenReady([bIsAvailable]()
    {
        UWifiDirectInterface* const Interface = UWifiDirectInterface::GetWifiDirectInterface();
        Interface->bIsP2pAvailable = bIsAvailable;
        Interface->OnWifiDirectUpdated.Broadcast();
    }, TStatId(), nullptr, ENamedThreads::GameThread);
}

extern "C" JNIEXPORT void JNICALL Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectRefreshDiscoveryStateFunction(JNIEnv * Env, jclass clazz, jboolean isDiscovering)
{
    bool bIsDiscovering = (isDiscovering == JNI_TRUE);
    
    FFunctionGraphTask::CreateAndDispatchWhenReady([bIsDiscovering]()
    {
        UWifiDirectInterface* const Interface = UWifiDirectInterface::GetWifiDirectInterface();
        Interface->bIsP2pPeerDiscovering = bIsDiscovering;
        Interface->OnWifiDirectUpdated.Broadcast();
    }, TStatId(), nullptr, ENamedThreads::GameThread);
}

extern "C" JNIEXPORT void JNICALL Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectDnsSdServiceAvailableFunction(JNIEnv * Env, jclass clazz, jstring javaDeviceName, jstring javaDeviceMacAddress)
{
	FString DeviceName;
	if (javaDeviceName != nullptr)
	{
		const char* nameChars = Env->GetStringUTFChars(javaDeviceName, nullptr);
		DeviceName = FString(UTF8_TO_TCHAR(nameChars));
		Env->ReleaseStringUTFChars(javaDeviceName, nameChars);
	}
	
	FString DeviceMacAddress;
	if (javaDeviceMacAddress != nullptr)
	{
		const char* ipChars = Env->GetStringUTFChars(javaDeviceMacAddress, nullptr);
		DeviceMacAddress = FString(UTF8_TO_TCHAR(ipChars));
		Env->ReleaseStringUTFChars(javaDeviceMacAddress, ipChars);
	}
    
	FFunctionGraphTask::CreateAndDispatchWhenReady([DeviceName, DeviceMacAddress]()
	{
		UWifiDirectInterface* const Interface = UWifiDirectInterface::GetWifiDirectInterface();
		Interface->ShootingStarPeers.Push(FWifiDirectPeerDeviceInfo { DeviceName, DeviceMacAddress });
	}, TStatId(), nullptr, ENamedThreads::GameThread);
}

#endif
