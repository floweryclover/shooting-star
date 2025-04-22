// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CompetitiveGameMode.generated.h"

class UCompetitiveSystemComponent;
class UMapGeneratorComponent;

/**
 * 2대2 3선승 게임 모드입니다.
 */
UCLASS()
class SHOOTINGSTAR_API ACompetitiveGameMode final : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACompetitiveGameMode();

	virtual void Tick(float DeltaSeconds) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual void Logout(AController* Exiting) override;

	virtual void SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC) override;

	virtual int32 GetNumPlayers() override
	{
		Super::GetNumPlayers();
		return NumPlayers;
	}

	//
	// Getter/Setter
	//
	
	UCompetitiveSystemComponent* GetCompetitiveSystemComponent() const
	{
		return CompetitiveSystemComponent;
	}
	
	UMapGeneratorComponent* GetMapGeneratorComponent() const
	{
		return MapGeneratorComponent;
	}

	UFUNCTION(BlueprintCallable)
	void InteractResource(AController* Controller);

	UFUNCTION(BlueprintCallable)
	void CraftWeapon(AController* Controller, const FWeaponData& Weapon, const TArray<int32>& Resources);

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadOnly)
	int32 NumPlayers;
	
	UPROPERTY(BlueprintReadOnly)
	float GameStartSeconds = 10.0f;

	UPROPERTY(BlueprintReadOnly)
	FName ExitLevel = TEXT("/Game/Levels/Lobby");
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCompetitiveSystemComponent> CompetitiveSystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UMapGeneratorComponent> MapGeneratorComponent;
};