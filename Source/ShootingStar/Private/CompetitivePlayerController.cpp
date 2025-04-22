// Copyright 2025 ShootingStar. All Rights Reserved.


#include "CompetitivePlayerController.h"

#include "ClientComponent.h"
#include "TeamComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Pawn.h"
#include "CompetitivePlayerCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Blueprint/UserWidget.h"
#include "InventoryComponent.h"
#include "ServerComponent.h"

ACompetitivePlayerController::ACompetitivePlayerController()
{

	TeamComponent = CreateDefaultSubobject<UTeamComponent>(TEXT("TeamComponent"));
	ServerComponent = CreateDefaultSubobject<UServerComponent>(TEXT("ServerComponent"));
	ClientComponent = CreateDefaultSubobject<UClientComponent>(TEXT("ClientComponent"));
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	static ConstructorHelpers::FClassFinder<UUserWidget> ScoreBoardUIBPFinder{ TEXT("/Game/Blueprints/UI/BP_ScoreUI") };
	ensure(ScoreBoardUIBPFinder.Succeeded());
	if (ScoreBoardUIBPFinder.Succeeded())
	{
		ScoreBoardUIClass = ScoreBoardUIBPFinder.Class;
	}
	
	// Attach Inventory Component
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

void ACompetitivePlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		ensure(ScoreBoardUIClass);
		if (ScoreBoardUIClass)
		{
			ScoreBoardUI = CreateWidget<UUserWidget>(GetWorld(), ScoreBoardUIClass);
			ensure(ScoreBoardUI);
			if (ScoreBoardUI)
			{
				ScoreBoardUI->AddToViewport();
			}
		}
	}
}

void ACompetitivePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	LookMouse();
}

void ACompetitivePlayerController::ToggleInventoryWidget()
{
	if (InventoryWidget && InventoryWidget->IsInViewport())
	{
		InventoryWidget->RemoveFromParent();
		InventoryWidget = nullptr;
	}
	else
	{
		if (!InventoryWidgetClass) return;
	
		InventoryWidget = CreateWidget<UUserWidget>(this, InventoryWidgetClass);
		if (InventoryWidget)
		{
			InventoryWidget->AddToViewport();
		}
	}
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
		EnhancedInputComponent->BindAction(EquipKnifeAction, ETriggerEvent::Triggered, this, &ACompetitivePlayerController::EquipKnifeWeapon);
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
	
	if (Hit.bBlockingHit && IsLocalController())
	{
		APawn* const MyPawn = GetPawn();
		if (MyPawn)
		{
			FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(MyPawn->GetActorLocation(),
				FVector(Hit.Location.X, Hit.Location.Y, MyPawn->GetActorLocation().Z));
			//MyPawn->SetActorRotation(LookRotation); 동기화 문제때문에 아래 함수로 교체
			SetControlRotation(LookRotation);
		}
	}
}

void ACompetitivePlayerController::Attack()
{
	ACompetitivePlayerCharacter* ControlledCharacter = Cast<ACompetitivePlayerCharacter>(GetPawn());
	if (ControlledCharacter)
	{
		ControlledCharacter->Attack();
	}
}

void ACompetitivePlayerController::EquipWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("Trigger EquipWeapon"));
	ACompetitivePlayerCharacter* ControlledCharacter = Cast<ACompetitivePlayerCharacter>(GetPawn());
	if (ControlledCharacter)
	{
		ControlledCharacter->WeaponChange();
	}
}
void ACompetitivePlayerController::EquipKnifeWeapon()
{
	ACompetitivePlayerCharacter* ControlledCharacter = Cast<ACompetitivePlayerCharacter>(GetPawn());
	if (ControlledCharacter)
	{
		ControlledCharacter->WeaponKnifeChange();
	}
}
