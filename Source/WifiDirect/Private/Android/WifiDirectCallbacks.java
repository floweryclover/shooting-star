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

   public static native void nativeOnWifiDirectAddLocalServiceErrorFunction(int errorCode);

   public static void onWifiDirectAddLocalServiceError(int errorCode) {
       nativeOnWifiDirectAddLocalServiceErrorFunction(errorCode);
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
       
    public static native void nativeOnWifiDirectRefreshDiscoveryStateFunction(boolean isDiscovering);

    public static void onWifiDirectRefreshDiscoveryState(boolean isDiscovering) {
        nativeOnWifiDirectRefreshDiscoveryStateFunction(isDiscovering);
    }
    
    public static native void nativeOnWifiDirectDnsSdServiceAvailableFunction(String deviceName, String deviceMacAddress);

    public static void onWifiDirectDnsSdServiceAvailable(String deviceName, String deviceMacAddress) {
        nativeOnWifiDirectDnsSdServiceAvailableFunction(deviceName, deviceMacAddress);
    }
}