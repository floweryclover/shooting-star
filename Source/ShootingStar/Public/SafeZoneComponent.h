#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SafeZoneComponent.generated.h"

class UCompetitiveSystemComponent;  // 전방 선언으로 변경

/**
 * 자기장 영역을 시각적으로 표현하고 크기를 조절하는 컴포넌트입니다.
 * Static Mesh Actor에 부착하여 사용합니다.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHOOTINGSTAR_API USafeZoneComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USafeZoneComponent();

    UFUNCTION(BlueprintCallable)
    float GetCurrentRadius() const { return CurrentRadius; };

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // 자기장 크기 설정
    UPROPERTY(EditAnywhere, Category = "SafeZone Settings")
    float InitialSafeRadius = 150.f;
    
    UPROPERTY(EditAnywhere, Category = "SafeZone Settings")
    float FinalSafeRadius = 40.f;

private:
    float CurrentRadius;

    // 자기장 크기(Scale)
    FVector SafeZoneScale;

    UPROPERTY()
    UCompetitiveSystemComponent* CompetitiveSystem;

    UPROPERTY()
    UStaticMeshComponent* OwnerMeshComponent;

    void UpdateSafeZoneScale();
    void InitializeComponents();

    float CalculateCurrentRadius() const;
};
