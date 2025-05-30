// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MapObjectActor.h"
#include "TumbleWeed.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTumbleWeedOverlapChanged, ATumbleWeed*, TumbleWeed, const TArray<ACompetitivePlayerCharacter*>&, OverlappingCharacters);

class ACompetitivePlayerCharacter;
class USphereComponent;
/**
 * 
 */
UCLASS()
class SHOOTINGSTAR_API ATumbleWeed final : public AMapObjectActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FTumbleWeedOverlapChanged OnTumbleWeedOverlapChanged;

    ATumbleWeed();
	
protected:
	virtual void BeginPlay() override;
	
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<ACompetitivePlayerCharacter>> OverlappingCharacters; 

private:
	UFUNCTION()
	void OnActorBeginOverlapHandler(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnActorEndOverlapHandler(AActor* OverlappedActor, AActor* OtherActor);
};
