// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h" 

// Sets default values
ACollisionProjectile::ACollisionProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));

	//Collider = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Collider"));
	//Collider->SetupAttachment(GetRootComponent());
	//Collider->RegisterComponent();

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
	//if (Collider->IsCollisionEnabled())
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("wewqew"))
	//}
	ActivationTime = GetWorld()->GetTimeSeconds();
}

void ACollisionProjectile::Setup(enum ESpell Spell, AWand* WandPointer, float Time)
{
	Spell = Spell;
	Wand = WandPointer;
	DeleteTime = Time; // TODO change this back
}

void ACollisionProjectile::Hit(UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Result)
	{
		Destroy();
		//UE_LOG(LogTemp, Warning, TEXT("hit projectile!!"))
		// Add impulse (debug as needs to be on stupefy not here)
		UStaticMeshComponent* ImpulseComponent = Cast<UStaticMeshComponent>(OtherComp);
		if (!ensure(ImpulseComponent)) { return; }
		if (ImpulseComponent->IsSimulatingPhysics())
		{
			FVector ImpulseDirection = (Result.TraceEnd - Result.TraceStart).GetSafeNormal();
			//UE_LOG(LogTemp, Warning, TEXT("adding impulse dir %s"), *ImpulseDirection.ToString())
			ImpulseComponent->AddForce(ImpulseDirection * 10000000);
		}
}