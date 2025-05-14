// Copyright 2025 ShootingStar. All Rights Reserved.


#include "CompetitivePlayerController.h"

#include "CompetitiveGameState.h"
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
#include "SupplyActor.h"
#include "SupplyIndicatorUI.h"
#include "ShootingStar/ShootingStar.h"

ACompetitivePlayerController::ACompetitivePlayerController()
{
	TeamComponent = CreateDefaultSubobject<UTeamComponent>(TEXT("TeamComponent"));
	ServerComponent = CreateDefaultSubobject<UServerComponent>(TEXT("ServerComponent"));
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	//* ScoreBoard UI
	static ConstructorHelpers::FClassFinder<UUserWidget> ScoreBoardUIBPFinder{ TEXT("/Game/Blueprints/UI/BP_ScoreUI") };
	ensure(ScoreBoardUIBPFinder.Succeeded());
	if (ScoreBoardUIBPFinder.Succeeded())
	{
		ScoreBoardUIClass = ScoreBoardUIBPFinder.Class;
	} 

	//* Inventory UI
	static ConstructorHelpers::FClassFinder<UUserWidget> InventoryWidgetBPFinder(TEXT("/Game/Blueprints/UI/BP_Inventory"));
	ensure(InventoryWidgetBPFinder.Succeeded());
	if (InventoryWidgetBPFinder.Succeeded())
	{
		InventoryWidgetClass = InventoryWidgetBPFinder.Class;
	}
	
	// Attach Inventory Component
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	//* SupplyIndicator UI
	static ConstructorHelpers::FClassFinder<UUserWidget> SupplyIndicatorBPFinder(TEXT("/Game/Blueprints/UI/BP_SupplyIndicatorUI"));
	ensure(SupplyIndicatorBPFinder.Succeeded());
	if (SupplyIndicatorBPFinder.Succeeded())
	{
		SupplyIndicatorUIClass = SupplyIndicatorBPFinder.Class;
	}

	//* GameState UI
	static ConstructorHelpers::FClassFinder<UUserWidget> GameStateUIBPFinder{ TEXT("/Game/Blueprints/UI/BP_GameStateUI") };
	ensure(GameStateUIBPFinder.Succeeded());
	if (GameStateUIBPFinder.Succeeded())
	{
		GameStateUIClass = GameStateUIBPFinder.Class;
	}
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

		ensure(GameStateUIClass);
		if (GameStateUIClass)
		{
			GameStateUI = CreateWidget<UUserWidget>(GetWorld(), GameStateUIClass);
			ensure(GameStateUI);
			if (GameStateUI)
			{
				GameStateUI->AddToViewport();
			}
		}
	}
}

void ACompetitivePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetIgnoreMoveInput(!bCanMove);

	if (bCanMove) 
	{
		LookMouse();
	}

	ACompetitiveGameState* const GameState = Cast<ACompetitiveGameState>(GetWorld()->GetGameState());
	if (!IsValid(GameState))
	{
		return;
	}
	
	for (ASupplyActor* const GameStateSupplyActor : GameState->GetSupplyActors())
	{
		// 새로 그려야 할 것이 있다면 그리기
		bool bIsDrawing = false;
		for (int i = SupplyIndicatorUIArray.Num()-1; i>=0; --i)
		{
			USupplyIndicatorUI* const UI = SupplyIndicatorUIArray[i];
			
			// 확인하는 겸 삭제해야 한다면 삭제하기
			if (!IsValid(UI->GetTargetSupplyActor()))
			{
				UI->RemoveFromParent();
				SupplyIndicatorUIArray.RemoveAt(i);
				continue;
			}
			
			if (UI->GetTargetSupplyActor() == GameStateSupplyActor)
			{
				bIsDrawing = true;
				break;
			}
		}
		
		if (!bIsDrawing)
		{
			if (SupplyIndicatorUIClass)
			{
				USupplyIndicatorUI* UI = CreateWidget<USupplyIndicatorUI>(GetWorld(), SupplyIndicatorUIClass);
				if (UI)
				{
					SupplyIndicatorUIArray.Push(UI);
					UI->AddToViewport();
					UI->InitSupply(GameStateSupplyActor);
					UE_LOG(LogShootingStar, Log, TEXT("Push Supply"));
				}
			}
		}
	}
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
		EnhancedInputComponent->BindAction(EquipRocketLauncherAction, ETriggerEvent::Triggered, this, &ACompetitivePlayerController::EquipRocketLauncher);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &ACompetitivePlayerController::Dash);
		EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Triggered, this, &ACompetitivePlayerController::ToggleInventoryWidget);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ACompetitivePlayerController::InteractResource);
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
	GetHitResultUnderCursor(ECC_GameTraceChannel2, false, Hit);
	
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
	SetCanMove(false);
	ACharacter* const ControllingCharacter = GetCharacter();
	ACompetitivePlayerCharacter* CompetitiveCharacter = Cast<ACompetitivePlayerCharacter>(ControllingCharacter);

	if (!IsValid(CompetitiveCharacter))
		return;
	
	ServerComponent->RequestMining();
	
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&ACompetitivePlayerController::InteractResource,
		1.0f,
		false
	);
}
void ACompetitivePlayerController::EquipRocketLauncher()
{
	ACharacter* const ControllingCharacter = GetCharacter();
	ACompetitivePlayerCharacter* CompetitiveCharacter = Cast<ACompetitivePlayerCharacter>(ControllingCharacter);
	CompetitiveCharacter->EquipRocketLauncher();
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
