// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Wand.generated.h"

UENUM()
enum class ESpell : uint8
{
	None,
	Lumos,
	Periculum
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPericulumEvent, UStaticMeshComponent*, WandMesh);

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
		class UMotionControllerComponent* MotionController = nullptr;
	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* WandMesh = nullptr;
	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* LumosLightSphere = nullptr;
	UPROPERTY(EditDefaultsOnly)
		ESpell SelectedSpell = ESpell::None;
	class APointLight* LumosLight = nullptr;
	TArray<float> WandVelocityHistory;
	int32 WandVelocityHistoryMax = 10;
	UPROPERTY(EditDefaultsOnly)
	float SpellActivationVelocity = 300;
	float WandActivationTimer = 0;

	void TriggerLumos();
	void TriggerPericulum();
	bool bWandVelocityForSpell();
	void TryFire();
public:
	EControllerHand Hand;

	void SetHand(EControllerHand SetHand);
	void DebugSpell();


	UPROPERTY(BlueprintAssignable)
		FPericulumEvent PericulumFire;

};
