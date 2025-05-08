// Copyright 2025 ShootingStar. All Rights Reserved.


#include "CompetitivePlayerController.h"

#include "ClientComponent.h"
#include "CompetitiveGameMode.h"
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
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerState.h"
#include "ShootingStar/ShootingStar.h"
#include "CompetitiveSystemComponent.h"

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

	static ConstructorHelpers::FClassFinder<UUserWidget> InventoryWidgetBPFinder(TEXT("/Game/Blueprints/UI/BP_Inventory"));
	ensure(InventoryWidgetBPFinder.Succeeded());
	if (InventoryWidgetBPFinder.Succeeded())
	{
		InventoryWidgetClass = InventoryWidgetBPFinder.Class;
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
		SetInputMode(FInputModeGameOnly());
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
	if (ACompetitiveGameMode* GameMode = GetWorld()->GetAuthGameMode<ACompetitiveGameMode>())
	{
		if (UCompetitiveSystemComponent* SystemComp = GameMode->GetCompetitiveSystemComponent())
		{
			SystemComp->OnSupplyDrop.AddDynamic(this, &ACompetitivePlayerController::RenderSupplyIndicator);
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

void ACompetitivePlayerController::RenderSupplyIndicator(FVector Location)
{
	int a = 3;
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
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &ACompetitivePlayerController::Dash);
		EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Triggered, this, &ACompetitivePlayerController::ToggleInventoryWidget);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ACompetitivePlayerController::Mining);
	}
}

void ACompetitivePlayerController::Move(const FInputActionValue& Value)
{
    // 자신에게 할당된 캐릭터에 대한 이동 입력은 자동 동기화, 즉시 이 함수에서 진행
	ACharacter* const ControllingCharacter = GetCharacter();
	if (!IsValid(ControllingCharacter))
	{
		return;
	}
	
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
	ControllingCharacter->AddMovementInput(ForwardDirection, MovementVector.Y);
	ControllingCharacter->AddMovementInput(RightDirection, MovementVector.X);
}

void ACompetitivePlayerController::LookMouse()
{
	// Move와 마찬가지로 회전 입력은 리플리케이션 없이 즉시 반영
	
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
void ACompetitivePlayerController::Mining()
{
	ACharacter* const ControllingCharacter = GetCharacter();
	ACompetitivePlayerCharacter* CompetitiveCharacter = Cast<ACompetitivePlayerCharacter>(ControllingCharacter);

	if (!IsValid(CompetitiveCharacter))
		return;
	CompetitiveCharacter->PlayMiningAnim();
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&ACompetitivePlayerController::InteractResource,
		1.0f,
		false
	);
}
void ACompetitivePlayerController::Dash()
{
	ServerComponent->RequestDash();
}
void ACompetitivePlayerController::InteractResource()
{
	ServerComponent->RequestInteractResource();
}

void ACompetitivePlayerController::Attack()
{
	ServerComponent->RequestAttack();
}

void ACompetitivePlayerController::EquipWeapon()
{
	ServerComponent->RequestEquipWeapon();
}
void ACompetitivePlayerController::EquipKnifeWeapon()
{
	ServerComponent->RequestEquipKnifeWeapon();
}
