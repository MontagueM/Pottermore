// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollisionProjectile.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpawnBlockNiagaraEvent, FHitResult, HitResult);

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

	//UPROPERTY(BlueprintAssignable)
	//	FSpawnBlockNiagaraEvent SpawnBlockNiagara;
private:
	//UPROPERTY(VisibleAnywhere)
	//	class UStaticMeshComponent* Collider = nullptr;
	class UProjectileMovementComponent* ProjectileMovement = nullptr;
	UPROPERTY(EditDefaultsOnly)
	float DeleteTime = 5;
	float ActivationTime = 0;

	UFUNCTION(BlueprintCallable, Category = Setup)
		void Hit(UPrimitiveComponent* HitComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			FVector NormalImpulse,
			const FHitResult& Result);
};
