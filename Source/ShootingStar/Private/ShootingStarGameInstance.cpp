// Copyright 2025 ShootingStar. All Rights Reserved.

#include "ShootingStarGameInstance.h"
#include "Kismet/GameplayStatics.h"

#if PLATFORM_ANDROID
#include <Android/AndroidApplication.h>
#include <Android/AndroidJNI.h>
#endif

namespace
{
	UShootingStarGameInstance* GameInstance{nullptr};
}

void UShootingStarGameInstance::Init()
{
    Super::Init();
    GameInstance = this;
}

void UShootingStarGameInstance::ConnectToNearbyDevice(const FString& DeviceAddress)
{
	if (bIsP2pGroupFormed)
	{
		OnConnectToPeerFailed.Broadcast(-1);
		return;
	}
#if PLATFORM_ANDROID
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();

    jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
    jmethodID ConnectToDeviceMethod = Env->GetMethodID(ActivityClass, "connectToDevice", "(Ljava/lang/String;)V");

    jstring JavaDeviceAddress = Env->NewStringUTF(TCHAR_TO_ANSI(*DeviceAddress));

    Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), ConnectToDeviceMethod, JavaDeviceAddress);

    Env->DeleteLocalRef(ActivityClass);
    Env->DeleteLocalRef(JavaDeviceAddress);
#endif
}

void UShootingStarGameInstance::StartPeerDiscovering()
{
	if (bIsP2pGroupFormed)
	{
		OnDiscoverPeersFailed.Broadcast(-1);
		return;
	}
#if PLATFORM_ANDROID
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();

    jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
    jmethodID DiscoverPeersMethod = Env->GetMethodID(ActivityClass, "discoverPeers", "()V");

    Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), DiscoverPeersMethod);

    Env->DeleteLocalRef(ActivityClass);
#endif
}

void UShootingStarGameInstance::StopPeerDiscovering()
{
    if (bIsP2pGroupFormed)
    {
        OnDiscoverPeersFailed.Broadcast(-1);
        return;
    }
#if PLATFORM_ANDROID
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();

    jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
    jmethodID StopPeerDiscoveryMethod = Env->GetMethodID(ActivityClass, "stopPeerDiscovery", "()V");

    Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), StopPeerDiscoveryMethod);

    Env->DeleteLocalRef(ActivityClass);
#endif
}

void UShootingStarGameInstance::RemoveGroup()
{
#if PLATFORM_ANDROID
    JNIEnv* Env = FAndroidApplication::GetJavaEnv();

    jclass ActivityClass = Env->GetObjectClass(FAndroidApplication::GetGameActivityThis());
    jmethodID RemoveGroupMethod = Env->GetMethodID(ActivityClass, "removeGroup", "()V");

    Env->CallVoidMethod(FAndroidApplication::GetGameActivityThis(), RemoveGroupMethod);

    Env->DeleteLocalRef(ActivityClass);
#endif
}

#if PLATFORM_ANDROID
extern "C" JNIEXPORT void JNICALL Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectDiscoverPeersErrorFunction(JNIEnv * Env, jclass clazz, jint JavaErrorCode)
{
    int32 ErrorCode;
    ErrorCode = static_cast<int32>(JavaErrorCode);
    
    FFunctionGraphTask::CreateAndDispatchWhenReady([ErrorCode]()
        {
            GameInstance->OnDiscoverPeersFailed.Broadcast(ErrorCode);
        }, TStatId(), nullptr, ENamedThreads::GameThread);
}

extern "C" JNIEXPORT void JNICALL Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectConnectErrorFunction(JNIEnv * Env, jclass clazz, jint JavaErrorCode)
{
    int32 ErrorCode;
    ErrorCode = static_cast<int32>(JavaErrorCode);
    
    FFunctionGraphTask::CreateAndDispatchWhenReady([ErrorCode]()
        {
            GameInstance->OnConnectToPeerFailed.Broadcast(ErrorCode);
        }, TStatId(), nullptr, ENamedThreads::GameThread);
}

extern "C" JNIEXPORT void JNICALL Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectRemoveGroupErrorFunction(JNIEnv * Env, jclass clazz, jint JavaErrorCode)
{
    int32 ErrorCode;
    ErrorCode = static_cast<int32>(JavaErrorCode);
    FFunctionGraphTask::CreateAndDispatchWhenReady([ErrorCode]()
        {
            GameInstance->OnRemoveGroupFailed.Broadcast(ErrorCode);
        }, TStatId(), nullptr, ENamedThreads::GameThread);
}

extern "C" JNIEXPORT void JNICALL Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectRefreshPeerListFunction(JNIEnv * Env, jclass clazz, jobjectArray peerDeviceNames, jobjectArray peerDeviceAddresses)
{
    // 배열 길이(두 배열의 길이가 동일하다고 가정)
    jsize arrayLength = Env->GetArrayLength(peerDeviceNames);
    
    // TArray에 변환할 결과 저장
    TArray<FWifiDirectPeerDeviceInfo> WifiDirectPeers;
    WifiDirectPeers.Reserve(arrayLength);
    
    for (jsize i = 0; i < arrayLength; i++)
    {
        // 각 인덱스의 이름과 주소 얻기
        jstring jName = (jstring)Env->GetObjectArrayElement(peerDeviceNames, i);
        jstring jAddress = (jstring)Env->GetObjectArrayElement(peerDeviceAddresses, i);
        
        // UTF-8 문자열로 변환
        const char* nameChars = Env->GetStringUTFChars(jName, nullptr);
        const char* addressChars = Env->GetStringUTFChars(jAddress, nullptr);
        
        // FString으로 변환 (UTF8_TO_TCHAR 매크로 사용)
        FString DeviceName = FString(UTF8_TO_TCHAR(nameChars));
        FString DeviceMacAddress = FString(UTF8_TO_TCHAR(addressChars));
        
        // JNI 메모리 해제
        Env->ReleaseStringUTFChars(jName, nameChars);
        Env->ReleaseStringUTFChars(jAddress, addressChars);
        
        // 로컬 참조 삭제
        Env->DeleteLocalRef(jName);
        Env->DeleteLocalRef(jAddress);
        
        // 구조체 생성 후 TArray에 추가
        FWifiDirectPeerDeviceInfo PeerInfo;
        PeerInfo.DeviceName = DeviceName;
        PeerInfo.DeviceMacAddress = DeviceMacAddress;
        WifiDirectPeers.Add(PeerInfo);
    }
    
    // 게임 스레드에서 실행되도록 태스크 디스패치 (예: 델리게이트 실행)
    FFunctionGraphTask::CreateAndDispatchWhenReady([WifiDirectPeers]()
    {
        // 델리게이트 실행: 인자 WifiDirectPeers를 전달
        GameInstance->OnPeerListUpdated.Broadcast(WifiDirectPeers);
    }, TStatId(), nullptr, ENamedThreads::GameThread);
}

extern "C" JNIEXPORT void JNICALL Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectRefreshGroupFunction(JNIEnv * Env, jclass clazz, jboolean isGroupFormed, jboolean isGroupOwner, jstring groupOwnerIpAddress)
{
    // jboolean을 bool로 변환
    bool bIsGroupFormed = (isGroupFormed == JNI_TRUE);
    bool bIsGroupOwner = (isGroupOwner == JNI_TRUE);
    
    FString GroupOwnerIp;
    if (groupOwnerIpAddress != nullptr)
    {
        const char* ipChars = Env->GetStringUTFChars(groupOwnerIpAddress, nullptr);
        GroupOwnerIp = FString(UTF8_TO_TCHAR(ipChars));
        Env->ReleaseStringUTFChars(groupOwnerIpAddress, ipChars);
    }
    
    // 게임 스레드에서 실행되도록 태스크 디스패치
    FFunctionGraphTask::CreateAndDispatchWhenReady([bIsGroupFormed, bIsGroupOwner, GroupOwnerIp]()
    {
        // 델리게이트 실행: 그룹 정보를 전달
        GameInstance->OnGroupInfoUpdated.Broadcast(bIsGroupFormed, bIsGroupOwner, GroupOwnerIp);
    }, TStatId(), nullptr, ENamedThreads::GameThread);
}

#endif
