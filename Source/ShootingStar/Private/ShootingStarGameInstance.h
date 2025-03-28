// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ShootingStarGameInstance.generated.h"

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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRemoveGroupFailed, int32, ErrorCode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPeerListUpdated, const TArray<FWifiDirectPeerDeviceInfo>&, PeerList);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FGroupInfoUpdated, bool, bIsGroupFormed, bool, bIsGroupOwner, const FString&, GroupOwnerIpAddress);
/**
 * 
 */
UCLASS()
class UShootingStarGameInstance final : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	/**
	 * @details
	 * 에러 코드 0: 주로 권한 문제, 2: Wifi 꺼짐
	 */
	UPROPERTY(BlueprintAssignable)
	FDiscoverPeersFailed OnDiscoverPeersFailed;

	UPROPERTY(BlueprintAssignable)
	FConnectToPeerFailed OnConnectToPeerFailed;

	UPROPERTY(BlueprintAssignable)
	FRemoveGroupFailed OnRemoveGroupFailed;

	UPROPERTY(BlueprintAssignable)
	FPeerListUpdated OnPeerListUpdated;

	UPROPERTY(BlueprintAssignable)
	FGroupInfoUpdated OnGroupInfoUpdated;

	/**
	 * 현재 참여 중인 WiFi P2P 그룹이 있다면 나가며, 자신이 그룹 오너인 경우 모든 피어가 연결이 해제됩니다.
	 */
	UFUNCTION(BlueprintCallable)
	void RemoveGroup();
	
	UFUNCTION(BlueprintCallable)
	void ConnectToNearbyDevice(const FString& DeviceMacAddress);
	
	UFUNCTION(BlueprintCallable)
	void StartPeerDiscovering();

	UFUNCTION(BlueprintCallable)
	void StopPeerDiscovering();
	
	const TArray<FWifiDirectPeerDeviceInfo>& GetWifiDirectPeers() const
	{
		return WifiDirectPeers;
	}

protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<FWifiDirectPeerDeviceInfo> WifiDirectPeers;

	UPROPERTY(BlueprintReadOnly)
	bool bIsP2pGroupFormed;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsP2pGroupOwner;

	UPROPERTY(BlueprintReadOnly)
	FString GroupOwnerIpAddress;
};
