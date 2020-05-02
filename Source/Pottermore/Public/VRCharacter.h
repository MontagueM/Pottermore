// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VRCharacter.generated.h"

UENUM()
enum class ETurnType : uint8
{
	Snap,
	Smooth
};

UENUM()
enum class EMovementCoupling : uint8
{
	Head,
	Hand
};

UCLASS()
class POTTERMORE_API AVRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void StopTeleportationCheck();
private:
	UPROPERTY(VisibleAnywhere)
		class UCameraComponent* Camera = nullptr;
	UPROPERTY(VisibleAnywhere)
		class USceneComponent* VRRoot = nullptr;

	class AWand* Wand = nullptr;
	class AVRController* LeftController = nullptr;
	class AVRController* RightController = nullptr;

	UPROPERTY(VisibleAnywhere)
		class UPostProcessComponent* PostProcess;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AVRController> HandControllerClass;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AWand> WandClass;
	UPROPERTY(EditDefaultsOnly)
		EMovementCoupling MovementCoupling = EMovementCoupling::Head;
	UPROPERTY(EditDefaultsOnly)
		float TeleportBlinkTime = 0.3;
	UPROPERTY(EditDefaultsOnly)
		float TeleportTime = 0.1;
	UPROPERTY(EditDefaultsOnly)
		ETurnType TurnType = ETurnType::Snap;
	UPROPERTY(EditDefaultsOnly)
		float AngleToSnap = 45;
	UPROPERTY(EditDefaultsOnly)
		float SmoothTurnSpeed = 22;
	UPROPERTY(EditDefaultsOnly)
		float TeleportActivationScale = 0.2;
	UPROPERTY(EditDefaultsOnly)
		float SnapTurnActivationScale = 0.6;
	UPROPERTY(EditDefaultsOnly)
		float SmoothTurnActivationScale = 0.7;

	int32 ScaleHistoryMaxNum = 5;
	TArray<float> ScaleHistory;
	bool bCurrentlyTeleporting = false;
	class APlayerCameraManager* PlayerCameraManager = nullptr;
	bool HaveSnapped = false;

private:
	void MoveForward(float Scale);
	void MoveRight(float Scale);
	void TurnRight(float Scale);
	void TryTeleport(float Scale);
	void EndTeleport();
	void FadeOutFromTeleport();
	void UpdateActionMapping(class UInputSettings* InputSettings, FName ActionName, FKey OldKey, FKey NewKey);
	void UpdateAxisMapping(class UInputSettings* InputSettings, FName AxisName, FKey Key, float Scale);
	void StartTeleportationCheck();
	bool bVelocityForTeleport(float Scale);
	AVRController* GetTeleportController();
	AVRController* GetMovementController();

// Wand
private:
	UPROPERTY(EditDefaultsOnly)
		EControllerHand WandHand = EControllerHand::Left;

	void SetupWand();
	void DebugSpellFire();
};
