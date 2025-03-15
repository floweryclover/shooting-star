// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UClientComponent::UClientComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UClientComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UClientComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UClientComponent::JumpClient_Implementation()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(
		GetOwner()))
	{
		if (ACharacter* Character = Cast<ACharacter>(PlayerController->GetPawn()))
		{
			Character->Jump();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Character was nullptr."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController was nullptr."));
	}
}
