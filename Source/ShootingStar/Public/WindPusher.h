// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WindPusher.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTINGSTAR_API UWindPusher : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWindPusher();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Push(const FVector& Impulse);
	void Random_Push();

private:
	UStaticMeshComponent* OwnerMesh{};
	float AccTime{};
	float BaseTime{};
	
	UPROPERTY(EditAnywhere)
	FVector ImplusePower_Initial{};

	UPROPERTY(EditAnywhere)
	FVector ImpulsePower_Min{};

	UPROPERTY(EditAnywhere)
	FVector ImpulsePower_Max{};
};
