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
	Periculum,
	Protego,
	Stupefy
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPericulumEvent, UStaticMeshComponent*, WandMesh);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FProtegoEvent, UStaticMeshComponent*, WandMesh);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStupefyEvent, UStaticMeshComponent*, WandMesh);

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
	class APointLight* LumosLight = nullptr;
	TArray<float> WandVelocityHistory;
	int32 WandVelocityHistoryMax = 10;
	UPROPERTY(EditDefaultsOnly)
	float SpellActivationVelocity = 250;
	float WandActivationTimer = 0;

	void TriggerLumos();
	void TriggerPericulum();
	void TriggerProtego();
	void TriggerStupefy();
	bool bBaseCheckForSpell();
	bool bVelocityForSpell();
	bool bVerticalSpell();
	bool bCanTriggerSpell();
	void SpellTrigger();
	UPROPERTY(EditDefaultsOnly, Category = Setup)
		TSubclassOf<class ACollisionProjectile> ProjectileBlueprint;

	TArray<ESpell> SpellCycleOrder = { ESpell::Stupefy,
	ESpell::Periculum,
	ESpell::Protego,
	ESpell::Lumos };

public:
	EControllerHand Hand;

	void SetHand(EControllerHand SetHand);
	void TryFire();
	void CycleSpells();

	UFUNCTION(BlueprintCallable)
	void WandProjectileTrace(float ProjectileSpeed, float ProjectileTime);


	UPROPERTY(EditAnywhere)
		ESpell SelectedSpell = ESpell::Protego;
	UPROPERTY(BlueprintAssignable)
		FPericulumEvent PericulumFire;
	UPROPERTY(BlueprintAssignable)
		FProtegoEvent ProtegoFire;
	UPROPERTY(BlueprintAssignable)
		FStupefyEvent StupefyFire;

private:
	UPROPERTY(EditDefaultsOnly, Category = Audio)
		class USoundCue* StupefyAttackCue;
};
