// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h" 
#include "Components/CapsuleComponent.h" 
#include "Wand.h"

// Sets default values
ACollisionProjectile::ACollisionProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("Projectile Movement"));
	ProjectileMovement->bAutoActivate = false;}

// Called when the game starts or when spawned
void ACollisionProjectile::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ACollisionProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// In order to counterract the gravity the projectile experiences
	ProjectileMovement->AddForce(FVector(0, 0, 981));
	
	if (GetWorld()->GetTimeSeconds() - ActivationTime > DeleteTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("destroy"))
			Destroy();
	}
}

void ACollisionProjectile::LaunchProjectile(FVector Velocity)
{
	ProjectileMovement->SetVelocityInLocalSpace(Velocity);
	ProjectileMovement->Activate();
	ActivationTime = GetWorld()->GetTimeSeconds();
}

void ACollisionProjectile::Hit(UPrimitiveComponent* tHitComponent,
	AActor* tOtherActor,
	UPrimitiveComponent* tOtherComp,
	FVector tNormalImpulse,
	const FHitResult& tResult)
	{
	HitComponent = tHitComponent;
	OtherActor = tOtherActor;
	NormalImpulse = tNormalImpulse;
	Result = tResult;
	//SelectHitAction();
}

//void ACollisionProjectile::SelectHitAction()
//{
//	switch (Spell)
//	{
//	case ESpell::Stupefy:
//		return ImpulseReaction();
//		break;
//	}
//}

/* Add impulse (debug as needs to be on stupefy not here) */
void ACollisionProjectile::ImpulseReaction()
{
	Destroy();
	UCapsuleComponent* CharacterCapsule = Cast<UCapsuleComponent>(OtherActor);
	UStaticMeshComponent* ImpulseComponent = Cast<UStaticMeshComponent>(OtherComp);
	if (!ensure(ImpulseComponent)) { return; }
	// If physics object or is hitting player
	if (ImpulseComponent->IsSimulatingPhysics() || CharacterCapsule)
	{
		UE_LOG(LogTemp, Warning, TEXT("impulse"))
			FVector ImpulseDirection = (Result.TraceEnd - Result.TraceStart).GetSafeNormal();
		ImpulseComponent->AddForce(ImpulseDirection * 10000000);
	}
}
