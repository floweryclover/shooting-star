// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WifiDirectInterface.generated.h"

USTRUCT(BlueprintType)
struct FWifiDirectPeerDeviceInfo final
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString DeviceName;
	UPROPERTY(BlueprintReadWrite)
	FString DeviceMacAddress;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDiscoverPeersFailed, int32, ErrorCode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FConnectToPeerFailed, int32, ErrorCode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAddLocalServiceFailed, int32, ErrorCode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWifiDirectUpdated);

#if PLATFORM_ANDROID && USE_ANDROID_JNI
extern "C"
{
	void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectDiscoverPeersErrorFunction(JNIEnv * Env, jclass clazz, jint JavaErrorCode);
	void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectConnectErrorFunction(JNIEnv * Env, jclass clazz, jint JavaErrorCode);
	void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectAddLocalServiceErrorFunction(JNIEnv * Env, jclass clazz, jint JavaErrorCode);
	void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectRefreshPeerListFunction(JNIEnv * Env, jclass clazz, jobjectArray peerDeviceNames, jobjectArray peerDeviceAddresses);
	void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectRefreshGroupFunction(JNIEnv * Env, jclass clazz, jboolean isGroupFormed, jboolean isGroupOwner, jstring groupOwnerIpAddress);
	void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectRefreshP2pStateFunction(JNIEnv * Env, jclass clazz, jboolean isAvailable);
	void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectRefreshDiscoveryStateFunction(JNIEnv * Env, jclass clazz, jboolean isDiscovering);
	void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectDnsSdServiceAvailableFunction(JNIEnv * Env, jclass clazz, jstring javaDeviceName, jstring javaDeviceMacAddress);
}
#endif

/**
 * 
 */
UCLASS()
class WIFIDIRECT_API UWifiDirectInterface final : public UObject
{
	GENERATED_BODY()

public:
	UWifiDirectInterface();
	
	/**
	 * @details
	 * 에러 코드 0: 주로 권한 문제, 2: Wifi 꺼짐
	 */
	UPROPERTY(BlueprintAssignable)
	FDiscoverPeersFailed OnDiscoverPeersFailed;

	UPROPERTY(BlueprintAssignable)
	FConnectToPeerFailed OnConnectToPeerFailed;

	UPROPERTY(BlueprintAssignable)
	FAddLocalServiceFailed OnAddLocalServiceFailed;
	
	UPROPERTY(BlueprintAssignable)
	FWifiDirectUpdated OnWifiDirectUpdated;
	
	UFUNCTION(BlueprintCallable)
	void ConnectToNearbyDevice(const FString& DeviceMacAddress);
	
	UFUNCTION(BlueprintCallable)
	void Refresh();
	/**
	 * WiFi Direct 동작을 모두 중지하고 초기화합니다.
	 */
	UFUNCTION(BlueprintCallable)
	void Reset();

	/**
	 * 게임을 시작한 경우 등 더 이상 주변 피어에게서 검색되지 않게 합니다.
	 */
	UFUNCTION(BlueprintCallable)
	void StopDiscovering();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UWifiDirectInterface* GetWifiDirectInterface();
	
	const TArray<FWifiDirectPeerDeviceInfo>& GetShootingStarPeers() const
	{
		return ShootingStarPeers;
	}
	
	bool IsP2pServiceAdded() const
	{
		return bIsP2pServiceAdded;
	}
	
	bool IsP2pGroupFormed() const
	{
		return bIsP2pGroupFormed;
	}
	
	bool IsP2pGroupOwner() const
	{
		return bIsP2pGroupOwner;
	}
	
	bool IsP2pAvailable() const
	{
		return bIsP2pAvailable;
	}
	
	bool IsP2pPeerDiscovering() const
	{
		return bIsP2pPeerDiscovering;
	}
	
	const FString& GetGroupOwnerIpAddress() const
	{
		return GroupOwnerIpAddress;
	}
	
protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<FWifiDirectPeerDeviceInfo> ShootingStarPeers;

	UPROPERTY(BlueprintReadOnly)
	bool bIsP2pServiceAdded;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsP2pGroupFormed;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsP2pGroupOwner;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsP2pAvailable;

	UPROPERTY(BlueprintReadOnly)
	bool bIsP2pPeerDiscovering;

	UPROPERTY(BlueprintReadOnly)
	FString GroupOwnerIpAddress;

private:
	/**
	 * ShootingStar가 아닐 수도 있는, 주변 모든 기기
	 */
	TArray<FWifiDirectPeerDeviceInfo> GeneralPeers;
	
	void RemoveLocalService();
	
	void RemoveGroup();

	void AddLocalService();
	
	void StartPeerDiscovering();
	
	void StopPeerDiscovering();
	
	void RefreshP2pAvailability();
	
	void RefreshPeerList();
	
	void RefreshDiscoveryState();

	void RefreshGroupInfo();

	UFUNCTION()
	void ClearPeerList(int32 ErrorCode);

	UFUNCTION()
	void ClearIsP2pServiceAdded(int32 ErrorCode);

#if PLATFORM_ANDROID && USE_ANDROID_JNI
	friend void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectDiscoverPeersErrorFunction(JNIEnv * Env, jclass clazz, jint JavaErrorCode);
	friend void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectConnectErrorFunction(JNIEnv * Env, jclass clazz, jint JavaErrorCode);
	friend void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectAddLocalServiceErrorFunction(JNIEnv * Env, jclass clazz, jint JavaErrorCode);
	friend void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectRefreshPeerListFunction(JNIEnv * Env, jclass clazz, jobjectArray peerDeviceNames, jobjectArray peerDeviceAddresses);
	friend void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectRefreshGroupFunction(JNIEnv * Env, jclass clazz, jboolean isGroupFormed, jboolean isGroupOwner, jstring groupOwnerIpAddress);
	friend void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectRefreshP2pStateFunction(JNIEnv * Env, jclass clazz, jboolean isAvailable);
	friend void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectRefreshDiscoveryStateFunction(JNIEnv * Env, jclass clazz, jboolean isDiscovering);
	friend void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnWifiDirectDnsSdServiceAvailableFunction(JNIEnv * Env, jclass clazz, jstring javaDeviceName, jstring javaDeviceMacAddress);
#endif

};
