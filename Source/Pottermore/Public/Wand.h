// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Wand.generated.h"

UENUM()
enum class ESpell : uint8
{
	None,
	Lumos
};

UCLASS()
class POTTERMORE_API AWand : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWand();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
		class USceneComponent* Root = nullptr;
	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* WandMesh = nullptr;
	UPROPERTY(VisibleAnywhere)
		class UMotionControllerComponent* MotionController;
	UPROPERTY(EditDefaultsOnly)
		ESpell SelectedSpell = ESpell::None;
	EControllerHand Hand;
	FVector WandEndLocation;

public:
	void SetHand(EControllerHand SetHand);
	void TriggerLumos();
	void DebugSpell();

	class APointLight* Light = nullptr;
};
