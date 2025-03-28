// Copyright 2025 ShootingStar. All Rights Reserved.


#include "WifiDirectInterface.h"

#if PLATFORM_ANDROID && USE_ANDROID_JNI
#include <Android/AndroidApplication.h>
#include <Android/AndroidJNI.h>
#endif

UWifiDirectInterface* UWifiDirectInterface::GetWifiDirectInterface()
{
    static UWifiDirectInterface* Instance;
    if (!Instance) {
        Instance = NewObject<UWifiDirectInterface>();
        Instance->AddToRoot();
    }
    return Instance;
}

void UWifiDirectInterface::ConnectToNearbyDevice(const FString& DeviceAddress)
{
	if (bIsP2pGroupFormed)
	{
		OnConnectToPeerFailed.Broadcast(-1);
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
#if PLATFORM_ANDROID && USE_ANDROID_JNI
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();

    jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
    jmethodID RequestPeersMethod = Env->GetMethodID(ActivityClass, "requestPeers", "()V");

    Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), RequestPeersMethod);

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
        UWifiDirectInterface::GetWifiDirectInterface()->OnPeerListUpdated.Broadcast(WifiDirectPeers);
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
        UWifiDirectInterface::GetWifiDirectInterface()->OnGroupInfoUpdated.Broadcast(bIsGroupFormed, bIsGroupOwner, GroupOwnerIp);
    }, TStatId(), nullptr, ENamedThreads::GameThread);
}

extern "C" JNIEXPORT void JNICALL Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectRefreshP2pStateFunction(JNIEnv * Env, jclass clazz, jboolean isAvailable)
{
    bool bIsAvailable = (isAvailable == JNI_TRUE);
    
    FFunctionGraphTask::CreateAndDispatchWhenReady([bIsAvailable]()
    {
        UWifiDirectInterface::GetWifiDirectInterface()->OnP2pAvailabilityUpdated.Broadcast(bIsAvailable);
    }, TStatId(), nullptr, ENamedThreads::GameThread);
}

#endif
