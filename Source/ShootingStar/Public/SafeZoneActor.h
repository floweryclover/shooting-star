#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SafeZoneActor.generated.h"

/**
 * 자기장 영역을 시각적으로 표현하고 크기를 조절하는 액터입니다.
 */
UCLASS()
class SHOOTINGSTAR_API ASafeZoneActor final : public AActor
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    float GetRadius() const { return GetActorScale3D().X * 50.0f; };

    UFUNCTION(BlueprintCallable)
    void SetRadiusByAlpha(const float Alpha01)
    {
        const float Radius = FMath::Lerp(InitialSafeRadius, FinalSafeRadius, Alpha01);

        SetActorScale3D({Radius / 50.0f, Radius / 50.0f, Radius / 50.0f });    
    }
    
protected:
    virtual void BeginPlay() override;
    
    // 자기장 크기 설정, uu 단위(언리얼 유닛 = cm, Location의 그 단위).
    UPROPERTY(EditAnywhere, Category = "SafeZone Settings")
    float InitialSafeRadius = 5000.0f;
    
    UPROPERTY(EditAnywhere, Category = "SafeZone Settings")
    float FinalSafeRadius = 1000.0f;
};
