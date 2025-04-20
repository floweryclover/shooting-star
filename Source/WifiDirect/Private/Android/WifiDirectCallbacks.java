// Copyright ShootingStar 2025, All rights reserved.

package com.shootingstar.wifidirect;

public class WifiDirectCallbacks {
    public static native void nativeOnErrorFunction(String error);
    
    public static void onError(String error) {
        nativeOnErrorFunction(error);
    }
    
    public static native void nativeOnConnectionFailedFunction(String deviceName, String deviceMacAddress);
    
    public static void onConnectionFailed(String deviceName, String deviceMacAddress) {
        nativeOnConnectionFailedFunction(deviceName, deviceMacAddress);
    }

    public static native void nativeOnRefreshGroupFunction(boolean isGroupFormed, boolean isGroupOwner, String groupOwnerIpAddress);

    public static void onRefreshGroup(boolean isGroupFormed, boolean isGroupOwner, String groupOwnerIpAddress) {
        nativeOnRefreshGroupFunction(isGroupFormed, isGroupOwner, groupOwnerIpAddress);
    }
    
    public static native void nativeOnServiceFoundFunction(String deviceName, String deviceMacAddress);

    public static void onServiceFound(String deviceName, String deviceMacAddress) {
        nativeOnServiceFoundFunction(deviceName, deviceMacAddress);
    }
    
    public static native void nativeOnP2pStateChangedFunction(boolean isP2pEnabled);

    public static void onP2pStateChanged(boolean isP2pEnabled) {
        nativeOnP2pStateChangedFunction(isP2pEnabled);
    }
}