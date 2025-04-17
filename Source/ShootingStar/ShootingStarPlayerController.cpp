// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShootingStarPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "ShootingStarCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
// Debug Codes
#include "Blueprint/UserWidget.h"
#include "InventoryComponent.h"
#include "ResourceActor.h"
#include "ShootingStar.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AShootingStarPlayerController::AShootingStarPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;

	static ConstructorHelpers::FClassFinder<UUserWidget> InventoryUIBPFinder{ TEXT("/Game/Blueprints/UI/BP_Inventory") };
	ensure(InventoryUIBPFinder.Succeeded());
	if (InventoryUIBPFinder.Succeeded())
	{
		InventoryWidgetClass = InventoryUIBPFinder.Class;
	}

	// Attach Inventory Component
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

void AShootingStarPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void AShootingStarPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	LookMouse();
}

void AShootingStarPlayerController::Interact_Resources()
{
	if (!IsLocalPlayerController())
		return;

	APawn* OwnerPawn = GetPawn();
	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("OwnerPawn is nullptr!"));
		return;
	}

	FVector Start = OwnerPawn->GetActorLocation();
	Start.Z = 0.f;
	FRotator Rotation = OwnerPawn->GetActorRotation();

	FVector End = Start + Rotation.Vector() * 100.f;

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.0f, 0, 1.0f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerPawn);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, CollisionChannels::ResourceActor, Params))
	{
		DrawDebugPoint(GetWorld(), Hit.Location, 10, FColor::Red, false, 2.0f);
		AResourceActor* Resource = Cast<AResourceActor>(Hit.GetActor());

		// Server 
		Resource->Harvest(InventoryComponent);
	}
}

void AShootingStarPlayerController::ToggleInventoryWidget()
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

void AShootingStarPlayerController::SetupInputComponent()
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
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShootingStarPlayerController::Move);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &AShootingStarPlayerController::Attack);

	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AShootingStarPlayerController::Move(const FInputActionValue& Value)
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

void AShootingStarPlayerController::LookMouse()
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

void AShootingStarPlayerController::Attack()
{
	AShootingStarCharacter* ControlledCharacter = Cast<AShootingStarCharacter>(GetPawn());
	if (ControlledCharacter)
	{
		ControlledCharacter->PullTrigger();
	}
}