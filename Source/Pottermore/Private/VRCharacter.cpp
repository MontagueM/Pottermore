// Fill out your copyright notice in the Description page of Project Settings.

#include "VRCharacter.h"
#include "Camera/CameraComponent.h"
#include "Runtime/HeadMountedDisplay/Public/HeadMountedDisplayFunctionLibrary.h"
#include "Components/InputComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "VRController.h"
#include "Runtime/Engine/Classes/GameFramework/InputSettings.h"
#include "InputCoreTypes.h"
#include "Runtime/CoreUObject/Public/UObject/UObjectGlobals.h"
#include "Components/PostProcessComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Wand.h"

// Sets default values
AVRCharacter::AVRCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	VRRoot->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);

	PostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	PostProcess->SetupAttachment(GetRootComponent());

	Wand = CreateDefaultSubobject<AWand>(TEXT("Wand"));
}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();

	// so that we aren't in the floor
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	// we want to spawn the specific class here (BP)
	LeftController = GetWorld()->SpawnActor<AVRController>(HandControllerClass);
	if (!ensure(LeftController)) { return; }
	LeftController->AttachToComponent(VRRoot, FAttachmentTransformRules::KeepRelativeTransform);
	LeftController->SetOwner(this);
	LeftController->SetHand(EControllerHand::Left);

	RightController = GetWorld()->SpawnActor<AVRController>(HandControllerClass);
	if (!ensure(RightController)) { return; }
	RightController->AttachToComponent(VRRoot, FAttachmentTransformRules::KeepRelativeTransform);
	RightController->SetOwner(this);
	RightController->SetHand(EControllerHand::Right);

	SetupWand();
}

// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (!ensure(PlayerInputComponent)) { return; }
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// delete all old keymaps
	UInputSettings* InputSettings = const_cast<UInputSettings*>(GetDefault<UInputSettings>());
	TArray<FInputActionKeyMapping> ActionMappings = InputSettings->GetActionMappings();
	TArray<FInputAxisKeyMapping> AxisMappings = InputSettings->GetAxisMappings();
	for (FInputActionKeyMapping Mapping : ActionMappings) { InputSettings->RemoveActionMapping(Mapping); }
	for (FInputAxisKeyMapping Mapping : AxisMappings) { InputSettings->RemoveAxisMapping(Mapping); }

	if (LeftController && RightController)
	{
		if (GetTeleportController() == LeftController)
		{
			// DEBUG
			UpdateActionMapping(InputSettings, TEXT("Teleport"), FKey(), EKeys::SpaceBar);
			UpdateActionMapping(InputSettings, TEXT("CheckTeleport"), FKey(), EKeys::LeftShift);
			UpdateAxisMapping(InputSettings, TEXT("Forward"), EKeys::W, 1);
			UpdateAxisMapping(InputSettings, TEXT("Forward"), EKeys::S, -1);
			UpdateAxisMapping(InputSettings, TEXT("Right"), EKeys::D, 1);
			UpdateAxisMapping(InputSettings, TEXT("Right"), EKeys::A, -1);

			UpdateAxisMapping(InputSettings, TEXT("Teleport"), EKeys::OculusTouch_Left_Thumbstick_Y, 1); // sometimes oculus controllers dont work so for debug using spacebar
			UpdateActionMapping(InputSettings, TEXT("CheckTeleport"), FKey(), EKeys::OculusTouch_Left_Thumbstick_Down);
			UpdateAxisMapping(InputSettings, TEXT("Forward"), EKeys::OculusTouch_Right_Thumbstick_Y, 1);
			//UpdateAxisMapping(InputSettings, TEXT("Forward"), EKeys::OculusTouch_Right_Thumbstick_Down, -1);
			UpdateAxisMapping(InputSettings, TEXT("Right"), EKeys::OculusTouch_Right_Thumbstick_X, 1);
			//UpdateAxisMapping(InputSettings, TEXT("Right"), EKeys::OculusTouch_Right_Thumbstick_Left, -1);
			UpdateAxisMapping(InputSettings, TEXT("TurnRight"), EKeys::OculusTouch_Left_Thumbstick_X, 1);
		}
		else
		{
			// DEBUG
			UpdateActionMapping(InputSettings, TEXT("Teleport"), FKey(), EKeys::SpaceBar);
			UpdateActionMapping(InputSettings, TEXT("CheckTeleport"), FKey(), EKeys::LeftShift);
			UpdateAxisMapping(InputSettings, TEXT("Forward"), EKeys::W, 1);
			UpdateAxisMapping(InputSettings, TEXT("Forward"), EKeys::S, -1);
			UpdateAxisMapping(InputSettings, TEXT("Right"), EKeys::D, 1);
			UpdateAxisMapping(InputSettings, TEXT("Right"), EKeys::A, -1);

			UpdateAxisMapping(InputSettings, TEXT("Teleport"), EKeys::OculusTouch_Right_Thumbstick_Y, 1); // sometimes oculus controllers dont work so for debug using spacebar
			UpdateActionMapping(InputSettings, TEXT("CheckTeleport"), FKey(), EKeys::OculusTouch_Right_Thumbstick_Down);
			UpdateAxisMapping(InputSettings, TEXT("Forward"), EKeys::OculusTouch_Left_Thumbstick_Y, 1);
			//UpdateAxisMapping(InputSettings, TEXT("Forward"), EKeys::OculusTouch_Left_Thumbstick_Down, -1);
			UpdateAxisMapping(InputSettings, TEXT("Right"), EKeys::OculusTouch_Left_Thumbstick_X, 1);
			//UpdateAxisMapping(InputSettings, TEXT("Right"), EKeys::OculusTouch_Left_Thumbstick_Left, -1);
			UpdateAxisMapping(InputSettings, TEXT("TurnRight"), EKeys::OculusTouch_Right_Thumbstick_X, 1);
		}
	}
	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &AVRCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &AVRCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("TurnRight"), this, &AVRCharacter::TurnRight);
	PlayerInputComponent->BindAxis(TEXT("Teleport"), this, &AVRCharacter::TryTeleport);
	PlayerInputComponent->BindAction(TEXT("CheckTeleport"), IE_Pressed, this, &AVRCharacter::StartTeleportationCheck);
	PlayerInputComponent->BindAction(TEXT("CheckTeleport"), IE_Released, this, &AVRCharacter::StopTeleportationCheck);
	
	if (WandHand == EControllerHand::Left) { UpdateActionMapping(InputSettings, TEXT("DebugSpellFire"), FKey(), EKeys::SpaceBar); }
	else { UpdateActionMapping(InputSettings, TEXT("DebugSpellFire"), FKey(), EKeys::OculusTouch_Right_A_Click); }
	UE_LOG(LogTemp, Warning, TEXT("A"))
	PlayerInputComponent->BindAction(TEXT("DebugSpellFire"), IE_Released, this, &AVRCharacter::DebugSpellFire);
}

void AVRCharacter::MoveForward(float Scale)
{
	// If Camera, will be Head coupled
	if (MovementCoupling == EMovementCoupling::Head) { AddMovementInput(Camera->GetForwardVector(), Scale); }
	else if (MovementCoupling == EMovementCoupling::Hand)
	{
		// Adding both so you need both hands moving in one direction to make a difference.
		FVector Input = GetTeleportController()->GetActorForwardVector() + GetMovementController()->GetActorForwardVector();
		AddMovementInput(Input.GetSafeNormal(), Scale);
	}
}

void AVRCharacter::MoveRight(float Scale)
{
	AddMovementInput(Camera->GetRightVector(), Scale);
}

void AVRCharacter::TurnRight(float Scale)
{
	FRotator CurrentRotation = VRRoot->GetComponentRotation();
	if (TurnType == ETurnType::Snap && !HaveSnapped && abs(Scale) > SnapTurnActivationScale)
	{
		if (Scale > SnapTurnActivationScale) { Scale = 1; }
		else if (Scale < -SnapTurnActivationScale) { Scale = -1; }
		VRRoot->SetWorldRotation(CurrentRotation + FRotator(0, AngleToSnap * Scale, 0));
		HaveSnapped = true;
	}
	else if (TurnType == ETurnType::Snap && abs(Scale) < SnapTurnActivationScale && abs(Scale) > 0)
	{
		HaveSnapped = false;
	}
	else if (TurnType == ETurnType::Smooth && abs(Scale) > SmoothTurnActivationScale)
	{
		VRRoot->SetWorldRotation(CurrentRotation + FRotator(0, Scale * SmoothTurnSpeed / 50, 0));
	}
}

void AVRCharacter::TryTeleport(float Scale)
{
	if (GetTeleportController()->bAllowCharacterTeleport && bVelocityForTeleport(Scale) && !bCurrentlyTeleporting)
	{
		bCurrentlyTeleporting = true;
		// Fade out
		PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		PlayerCameraManager->StartCameraFade(0, 1, TeleportBlinkTime / 2, FLinearColor::Black, false, true); // last needs to be true otherwise flashes white
		FTimerHandle Handle;
		GetWorldTimerManager().SetTimer(Handle, this, &AVRCharacter::EndTeleport, TeleportBlinkTime / 2);
	}
}

void AVRCharacter::EndTeleport()
{
	PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	FVector TeleportLocation;
	GetTeleportController()->FindTeleportDestination(TeleportLocation); // could be more efficient to simply grab the position as usual instead of recalculating it all
	StopTeleportationCheck(); // we do this to reset the meshes sticking around
	SetActorLocation(TeleportLocation + FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight())); // Capsule added to stop teleporting into floor
	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AVRCharacter::FadeOutFromTeleport, TeleportTime);
}

void AVRCharacter::FadeOutFromTeleport()
{
	PlayerCameraManager->StartCameraFade(1, 0, TeleportBlinkTime / 2, FLinearColor::Black, false, true);
	bCurrentlyTeleporting = false;
}

AVRController* AVRCharacter::GetTeleportController()
{
	if (LeftController->bCanHandTeleport()) { return LeftController; }
	else { return RightController; }
}

AVRController* AVRCharacter::GetMovementController()
{
	if (LeftController->bCanHandMove()) { return LeftController; }
	else { return RightController; }
}

void AVRCharacter::UpdateActionMapping(UInputSettings* InputSettings, FName ActionName, FKey OldKey, FKey NewKey)
{
	if (OldKey.IsValid()) { InputSettings->RemoveActionMapping(FInputActionKeyMapping(ActionName, OldKey)); }
	if (!ensure(NewKey.IsValid())) { return; }
	InputSettings->AddActionMapping(FInputActionKeyMapping(ActionName, NewKey));
	InputSettings->SaveKeyMappings();
}

void AVRCharacter::UpdateAxisMapping(UInputSettings* InputSettings, FName AxisName, FKey Key, float Scale)
{
	if (!ensure(Key.IsValid())) { return; }
	InputSettings->AddAxisMapping(FInputAxisKeyMapping(AxisName, Key, Scale));
	InputSettings->SaveKeyMappings();
}

void AVRCharacter::StartTeleportationCheck()
{
	GetTeleportController()->SetCanCheckTeleport(true);
}

void AVRCharacter::StopTeleportationCheck()
{
	GetTeleportController()->SetCanCheckTeleport(false);
}

bool AVRCharacter::bVelocityForTeleport(float Scale)
{
	if (ScaleHistory.Num() < ScaleHistoryMaxNum)
		ScaleHistory.Add(Scale);
	else
	{
		ScaleHistory.RemoveAt(0, 1, true);
		ScaleHistory.Add(Scale);
	}

	if (ScaleHistory.Num() == ScaleHistoryMaxNum)
	{
		float Difference = ScaleHistory[ScaleHistoryMaxNum - 1] - ScaleHistory[0];
		bool bAllScalesNegative = true;
		for (float S : ScaleHistory)
		{
			if (S > 0)
			{
				bAllScalesNegative = false;
				break;
			}
		}
		if (Difference > TeleportActivationScale && bAllScalesNegative) // Difference needs to be negative as only when pulling back up
		{
			return true;
		}
	}
	return false;
}

void AVRCharacter::SetupWand()
{
	/*
	Attach the same way as controllers
	*/
	Wand = GetWorld()->SpawnActor<AWand>(WandClass);
	if (!ensure(Wand)) { return; }
	Wand->AttachToComponent(VRRoot, FAttachmentTransformRules::KeepRelativeTransform);
	Wand->SetOwner(this);
	Wand->SetHand(WandHand);
}

void AVRCharacter::DebugSpellFire()
{
	UE_LOG(LogTemp, Warning, TEXT("debug spell in char"))
	Wand->DebugSpell();
}
