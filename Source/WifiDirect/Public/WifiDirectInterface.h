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
 * On-Failed 이벤트의 ErrorCode 설명 <br/>
 * 음수: 임의로 정의한 에러 코드<br/>
 * 0: Android 내부 문제를 의미하지만 경험상 권한 문제였음<br/>
 * 1: P2P 미지원 기기<br/>
 * 2: 다른 작업을 처리 중임(ex. 연결 시도 중에 discoverPeers() 호출 등)<br/>
 * 3: Service Request 관련 오류로 볼 일 없을 듯<br/>
 * 4: 권한 없음. requestDeviceInfo() 등 권한 필요한 메소드 호출했는데 권한 없는 경우 - 아마 볼 일 없을 것, 사전에 설정을 해 두었기 때문에
 */
UCLASS()
class WIFIDIRECT_API UWifiDirectInterface final : public UObject
{
	GENERATED_BODY()

public:
	UWifiDirectInterface();
	
	UPROPERTY(BlueprintAssignable)
	FDiscoverPeersFailed OnDiscoverPeersFailed;

	/**
	 * 에러 코드 -1: P2P 그룹이 존재하는 상황에서 호출됨, -2: 주소값이 비었음, -3: 타임아웃
	 */
	UPROPERTY(BlueprintAssignable)
	FConnectToPeerFailed OnConnectToPeerFailed;

	UPROPERTY(BlueprintAssignable)
	FAddLocalServiceFailed OnAddLocalServiceFailed;
	
	UFUNCTION(BlueprintCallable)
	void ConnectToNearbyDevice(const FString& DeviceMacAddress);
	
	/**
	 * WiFi Direct 동작을 모두 중지하고 초기화합니다.
	 */
	UFUNCTION(BlueprintCallable)
	void Reset();

	/**
	 * 수동 업데이트 함수입니다. 반드시 한 번에 하나의 객체가 호출하게 하세요.
	 * @param DeltaSeconds 
	 */
	UFUNCTION(BlueprintCallable)
	void Update(float DeltaSeconds);

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

	UPROPERTY(BlueprintReadOnly)
	bool bIsConnecting;

	UPROPERTY(BlueprintReadOnly)
	float ConnectingElapsed;

	UPROPERTY(BlueprintReadOnly)
	float ConnectionTimeOutSeconds = 5.0f;

	UPROPERTY(BlueprintReadOnly)
	float UpdateInterval = 1.0f;

	UPROPERTY(BlueprintReadOnly)
	float UpdateElapsed;

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
	
	void CancelConnect();
	
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
