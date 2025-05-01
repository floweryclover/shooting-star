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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWifiDirectError, const FString&, Error);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FConnectionFailed, const FString&, DeviceName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FP2pStateChanged, bool, bIsP2pAvailable);

#if PLATFORM_ANDROID && USE_ANDROID_JNI
extern "C"
{
	void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnErrorFunction(JNIEnv * Env, jclass Clazz, jstring JavaError);
	void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnConnectionFailedFunction(JNIEnv * Env, jclass Clazz, jstring JavaDeviceName, jstring JavaDeviceMacAddress);
	void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnRefreshGroupFunction(JNIEnv * Env, jclass Clazz, jboolean JavaIsGroupFormed, jboolean JavaIsGroupOwner, jstring JavaGroupOwnerIpAddress);
	void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnServiceFoundFunction(JNIEnv * Env, jclass Clazz, jstring JavaDeviceName, jstring JavaDeviceMacAddress);
	void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnP2pStateChangedFunction(JNIEnv * Env, jclass Clazz, jboolean JavaIsP2pAvailable);
}
#endif

UCLASS()
class WIFIDIRECT_API UWifiDirectInterface final : public UObject
{
	GENERATED_BODY()

public:
	UWifiDirectInterface();
	
	/**
	 * Error 메시지의 에러코드 설명 <br/>
	 * 음수: 임의로 정의한 에러 코드<br/>
	 * 0: Android 내부 문제를 의미하지만 경험상 권한 문제였음<br/>
	 * 1: P2P 미지원 기기<br/>
	 * 2: 다른 작업을 처리 중임(ex. 연결 시도 중에 discoverPeers() 호출 등)<br/>
	 * 3: Service Request 관련 오류<br/>
	 * 4: 권한 없음. requestDeviceInfo() 등 권한 필요한 메소드 호출했는데 권한 없는 경우 - 아마 볼 일 없을 것, 사전에 설정을 해 두었기 때문에
	 */
	UPROPERTY(BlueprintAssignable)
	FWifiDirectError OnWifiDirectError;
	
	UPROPERTY(BlueprintAssignable)
	FConnectionFailed OnConnectionFailed;

	UPROPERTY(BlueprintAssignable)
	FP2pStateChanged OnP2pStateChanged;
	
	UFUNCTION(BlueprintCallable)
	void Connect(const FString& DeviceMacAddress);
	
	/**
	 * 연결된 P2P 그룹이 있다면 나가며, WiFi Direct 동작을 모두 중지하고 초기화합니다.
	 */
	UFUNCTION(BlueprintCallable)
	void Clear();

	/**
	 * P2P 그룹 상태는 유지하고 탐색 및 브로드캐스트 동작만 중지합니다.
	 */
	UFUNCTION(BlueprintCallable)
	void StopBroadcastAndDiscovery();

	/**
	 * 수동 업데이트 함수입니다. 반드시 한 번에 하나의 객체가 호출하게 하세요.
	 * @param DeltaSeconds 
	 */
	UFUNCTION(BlueprintCallable)
	void Update(float DeltaSeconds);

	/**
	 * 로컬 서비스를 등록합니다. 최초 탐색 시작 이전이나 Clear()를 호출한 후 다시 탐색을 시작할 경우 호출하세요.
	 */
	UFUNCTION(BlueprintCallable)
	void RegisterService();

	UFUNCTION(Blueprintable)
	void CheckAndRequestPermissions();

	UFUNCTION(BlueprintCallable)
	void CancelConnect();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UWifiDirectInterface* GetWifiDirectInterface();
	
	const TArray<FWifiDirectPeerDeviceInfo>& GetShootingStarPeers() const
	{
		return ShootingStarPeers;
	}
	
	bool IsP2pGroupFormed() const
	{
		return bIsP2pGroupFormed;
	}
	
	bool IsP2pGroupOwner() const
	{
		return bIsP2pGroupOwner;
	}
	
	const FString& GetGroupOwnerIpAddress() const
	{
		return GroupOwnerIpAddress;
	}
	
protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<FWifiDirectPeerDeviceInfo> ShootingStarPeers;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsP2pGroupFormed;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsP2pGroupOwner;

	UPROPERTY(BlueprintReadOnly)
	bool bIsP2pAvailable;
	
	UPROPERTY(BlueprintReadOnly)
	FString GroupOwnerIpAddress;

	UPROPERTY(BlueprintReadOnly)
	bool bIsConnecting;

	UPROPERTY(BlueprintReadOnly)
	float DiscoveryInterval = 20.0f;

	UPROPERTY(BlueprintReadOnly)
	float GroupUpdateInterval = 0.5f;

	UPROPERTY(BlueprintReadOnly)
	float ConnectingElapsed;

	UPROPERTY(BlueprintReadOnly)
	float ConnectTimeout = 5.0f;

	UPROPERTY(BlueprintReadOnly)
	float DiscoveryElapsed;

	UPROPERTY(BlueprintReadOnly)
	float GroupUpdateElapsed;

private:
	void RefreshGroupInfo();

	void RefreshServiceDiscovery();
	
	void OnServiceFound(const FString& DeviceName, const FString& DeviceMacAddress);
	
	void OnConnectionFailedCallback(const FString& DeviceName, const FString& DeviceMacAddress);

#if PLATFORM_ANDROID && USE_ANDROID_JNI
	friend void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnErrorFunction(JNIEnv * Env, jclass Clazz, jstring JavaError);
	friend void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnConnectionFailedFunction(JNIEnv * Env, jclass Clazz, jstring JavaDeviceName, jstring JavaDeviceMacAddress);
	friend void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnRefreshGroupFunction(JNIEnv * Env, jclass Clazz, jboolean JavaIsGroupFormed, jboolean JavaIsGroupOwner, jstring JavaGroupOwnerIpAddress);
	friend void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnServiceFoundFunction(JNIEnv * Env, jclass Clazz, jstring JavaDeviceName, jstring JavaDeviceMacAddress);
	friend void Java_com_shootingstar_wifidirect_WifiDirectCallbacks_nativeOnP2pStateChangedFunction(JNIEnv * Env, jclass Clazz, jboolean JavaIsP2pAvailable);
#endif

};
