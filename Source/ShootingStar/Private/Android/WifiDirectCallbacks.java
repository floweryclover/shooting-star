// Copyright floweryclover 2024, All rights reserved.

package com.shootingstar.wifidirect;

public class WifiDirectCallbacks {
	public static native void nativeOnWifiDirectDiscoverPeersErrorFunction(int errorCode);

	public static void onWifiDirectDiscoverPeersError(int errorCode) {
		nativeOnWifiDirectDiscoverPeersErrorFunction(errorCode);
	}

	public static native void nativeOnWifiDirectConnectErrorFunction(int errorCode);

	public static void onWifiDirectConnectError(int errorCode) {
		nativeOnWifiDirectConnectErrorFunction(errorCode);
	}

	public static native void nativeOnWifiDirectRemoveGroupErrorFunction(int errorCode);

	public static void onWifiDirectRemoveGroupError(int errorCode) {
		nativeOnWifiDirectRemoveGroupErrorFunction(errorCode);
	}

	public static native void nativeOnWifiDirectRefreshPeerListFunction(String[] peerDeviceNames, String[] peerDeviceAddresses);

	public static void onWifiDirectRefreshPeerList(String[] peerDeviceNames, String[] peerDeviceAddresses) {
		nativeOnWifiDirectRefreshPeerListFunction(peerDeviceNames, peerDeviceAddresses);
	}
	
    public static native void nativeOnWifiDirectRefreshGroupFunction(boolean isGroupFormed, boolean isGroupOwner, String groupOwnerIpAddress);

    public static void onWifiDirectRefreshGroup(boolean isGroupFormed, boolean isGroupOwner, String groupOwnerIpAddress) {
        nativeOnWifiDirectRefreshGroupFunction(isGroupFormed, isGroupOwner, groupOwnerIpAddress);
    }
    
    public static native void nativeOnWifiDirectRefreshP2pStateFunction(boolean isAvailable);

    public static void onWifiDirectRefreshP2pState(boolean isAvailable) {
        nativeOnWifiDirectRefreshP2pStateFunction(isAvailable);
    }
}