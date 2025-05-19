// Copyright 2025 ShootingStar. All Rights Reserved.


#include "Custom_UserWidget.h"

void UCustom_UserWidget::GetAngleRotation(FVector2D Center, FVector2D Border, FVector2D& Output1, FVector2D& Output2)
{
    FVector V1(Center.X, Center.Y, 0.0f);
    FVector V2(Border.X, Border.Y, 0.0f);


    FRotator LookAtRotation = (V2 - V1).Rotation();


    float Yaw = LookAtRotation.Yaw;


    float YawRad = FMath::DegreesToRadians(Yaw);


    float SinYaw = FMath::Sin(YawRad);
    float CosYaw = FMath::Cos(YawRad);


    Output1 = FVector2D(SinYaw, CosYaw);


    Output2 = FVector2D(CosYaw, -SinYaw);
}

float UCustom_UserWidget::GetCondition(FVector2D CanvasPane, FVector2D Center, FVector2D TouchThumb)
{
    float DistanceBC = FVector2D::Distance(Center, TouchThumb);


    float HalfCanvasPaneX = CanvasPane.X / 2.0f;


    float SelectValue = (DistanceBC > HalfCanvasPaneX) ? HalfCanvasPaneX : DistanceBC;

    return SelectValue;
}
FVector2D UCustom_UserWidget::MultiplyAndSwap(FVector2D Vector, float Flot)
{
    float NewX = Vector.X * Flot;
    float NewY = Vector.Y * Flot;


    FVector2D Result(NewY, NewX);

    return Result;
}