// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Custom_UserWidget.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTINGSTAR_API UCustom_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
	//Get Angle Rotation
	UFUNCTION(BlueprintPure, Category = "CustomMobileController")
	void  GetAngleRotation(FVector2D Center, FVector2D Border, FVector2D& Output1, FVector2D& Output2);
	// Get Angle Condition
	UFUNCTION(BlueprintPure, Category = "CustomMobileController")
	float GetCondition(FVector2D CanvasPane, FVector2D Center, FVector2D TouchThumb);
	//Multiply And Swap
	UFUNCTION(BlueprintPure, Category = "CustomMobileController")
	FVector2D MultiplyAndSwap(FVector2D Vector, float Flot);
};
