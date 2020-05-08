// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollisionProjectile.generated.h"

UCLASS()
class POTTERMORE_API ACollisionProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACollisionProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void LaunchProjectile(FVector Velocity);
	void SelectHitAction();
	void ImpulseReaction();

	class AWand* Wand = nullptr;
	UPrimitiveComponent* HitComponent = nullptr;
	AActor* OtherActor = nullptr;
	UPrimitiveComponent* OtherComp = nullptr;
	FVector NormalImpulse;
	FHitResult Result;
	bool bIsProtegoWall;

private:
	class UProjectileMovementComponent* ProjectileMovement = nullptr;
	//class UAudioComponent* AudioComponent = nullptr;
	UPROPERTY(EditDefaultsOnly)
	float DeleteTime = 5;
	float ActivationTime = 0;

	UFUNCTION(BlueprintCallable, Category = Setup)
		void Hit(UPrimitiveComponent* HitComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			FVector NormalImpulse,
			const FHitResult& Result,
			bool bIsProtegoWall);

	UPROPERTY(EditDefaultsOnly, Category = Audio)
		class USoundCue* ProtegoDeflectCue;
};
