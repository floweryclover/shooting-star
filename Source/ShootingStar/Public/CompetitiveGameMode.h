// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CompetitiveGameMode.generated.h"

class ACompetitivePlayerController;
class ACompetitivePlayerCharacter;
class UCompetitiveSystemComponent;
class UMapGeneratorComponent;
class ASupplyActor;
class ASafeZoneActor;

/**
 * 2대2 3선승 게임 모드입니다.
 */
UCLASS()
class SHOOTINGSTAR_API ACompetitiveGameMode final : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACompetitiveGameMode();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	
	virtual void Tick(float DeltaSeconds) override;

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual void OnPostLogin(AController* NewPlayer) override;
	
	virtual void Logout(AController* Exiting) override;

	virtual void SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC) override;
	
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;

	virtual void RestartPlayer(AController* NewPlayer) override;

	/**
	 * Killer에게 점수를 줍니다. 실제로 액터를 죽이거나 하지 않습니다.
	 * @param Killer 
	 * @param Killee 
	 */
	UFUNCTION(BlueprintCallable)
	void HandleKill(AActor* Killer, AActor* Killee);
	
	virtual int32 GetNumPlayers() override
	{
		Super::GetNumPlayers();
		return NumPlayers;
	}

	//
	// Getter/Setter
	//

	float GetWaitingForGameStartSeconds() const
	{
		return WaitingForGameStartSeconds;
	}
	
	UCompetitiveSystemComponent* GetCompetitiveSystemComponent() const
	{
		return CompetitiveSystemComponent;
	}
	
	UMapGeneratorComponent* GetMapGeneratorComponent() const
	{
		return MapGeneratorComponent;
	}

	ASafeZoneActor* GetSafeZone() const
	{
		return SafeZoneActor;
	}

	UFUNCTION(BlueprintCallable)
	void InteractResource(AController* Controller);

	UFUNCTION(BlueprintCallable)
	void CraftWeapon(AController* Controller, const FWeaponData& Weapon, const TArray<int32>& Resources);

	/**
	 * MapGeneratorComponent::GetPlayerPoints()의 좌표들 중 Player를 제외한 모든 플레이어와의 거리값 합이 최대인 좌표를 반환하는 함수.
	 * @return 
	 */
	UFUNCTION(BlueprintCallable)
	FVector GetMostIsolatedSpawnPointFor(APlayerController* Player) const;

protected:
	UPROPERTY(BlueprintReadOnly)
	int32 NumPlayers;
	
	UPROPERTY(BlueprintReadOnly)
	float WaitingForGameStartSeconds = 10.0f;

	UPROPERTY(BlueprintReadOnly)
	FName ExitLevel = TEXT("/Game/Levels/Lobby");
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCompetitiveSystemComponent> CompetitiveSystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UMapGeneratorComponent> MapGeneratorComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Supply")
	TSubclassOf<ASupplyActor> SupplyActorClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASafeZoneActor> SafeZoneActorClass;

	UPROPERTY()
	int32 CurrentSupplyIndex;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ASafeZoneActor> SafeZoneActor;

private:
	void AssignTeamIfNone(APlayerController* Player);

	UFUNCTION()
	void OnGameStarted();

	UFUNCTION()
	void HandleSupplyDrop(FVector Location);
};