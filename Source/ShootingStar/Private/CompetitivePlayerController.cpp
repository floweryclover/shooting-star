// Copyright 2025 ShootingStar. All Rights Reserved.


#include "CompetitivePlayerController.h"
#include "TeamComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "ShootingStar/ShootingStarCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Blueprint/UserWidget.h"

ACompetitivePlayerController::ACompetitivePlayerController()
{
	TeamComponent = CreateDefaultSubobject<UTeamComponent>(TEXT("TeamComponent"));
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	static ConstructorHelpers::FClassFinder<UUserWidget> ScoreBoardUIBPFinder{ TEXT("/Game/Blueprints/UI/BP_ScoreUI") };
	ensure(ScoreBoardUIBPFinder.Succeeded());
	if (ScoreBoardUIBPFinder.Succeeded())
	{
		ScoreBoardUIClass = ScoreBoardUIBPFinder.Class;
	}
}

void ACompetitivePlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//if (IsLocalPlayerController())
	//{
	//	ensure(ScoreBoardUIClass);
	//	if (ScoreBoardUIClass)
	//	{
	//		ScoreBoardUI = CreateWidget<UUserWidget>(GetWorld(), ScoreBoardUIClass);
	//		ensure(ScoreBoardUI);
	//		if (ScoreBoardUI)
	//		{
	//			ScoreBoardUI->AddToViewport();
	//		}
	//	}
	//}
}

void ACompetitivePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	LookMouse();
}

void ACompetitivePlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup Move input events
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACompetitivePlayerController::Move);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ACompetitivePlayerController::Attack);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ACompetitivePlayerController::EquipWeapon);
	}
}

void ACompetitivePlayerController::Move(const FInputActionValue& Value)
{

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// find out which way is forward
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// add movement 
	GetCharacter()->AddMovementInput(ForwardDirection, MovementVector.Y);
	GetCharacter()->AddMovementInput(RightDirection, MovementVector.X);
}

void ACompetitivePlayerController::LookMouse()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		APawn* const MyPawn = GetPawn();
		if (MyPawn)
		{
			FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(MyPawn->GetActorLocation(),
				FVector(Hit.Location.X, Hit.Location.Y, MyPawn->GetActorLocation().Z));
			MyPawn->SetActorRotation(LookRotation);
		}
	}

}

void ACompetitivePlayerController::Attack()
{
	AShootingStarCharacter* ControlledCharacter = Cast<AShootingStarCharacter>(GetPawn());
	if (ControlledCharacter)
	{
		ControlledCharacter->Attack();
	}
}

void ACompetitivePlayerController::EquipWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("Trigger EquipWeapon"));
	AShootingStarCharacter* ControlledCharacter = Cast<AShootingStarCharacter>(GetPawn());
	if (ControlledCharacter)
	{
		ControlledCharacter->WeaponChange();
	}
}
