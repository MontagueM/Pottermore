// Fill out your copyright notice in the Description page of Project Settings.


#include "Wand.h"
#include "Components/StaticMeshComponent.h"
#include "MotionControllerComponent.h"
#include "Engine/PointLight.h" 
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h" 
#include "CollisionProjectile.h"

// Sets default values
AWand::AWand()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	SetRootComponent(MotionController);

	WandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WandMesh"));
	WandMesh->SetupAttachment(GetRootComponent());

	LumosLightSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LumosLightSphere"));
	LumosLightSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AWand::BeginPlay()
{
	Super::BeginPlay();
	LumosLightSphere->SetVisibility(false);
}

// Called every frame
void AWand::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//DrawDebugLine(GetWorld(),
	//GetActorLocation(),
	//GetActorLocation() + GetActorForwardVector() * 1000,
	//FColor::Red);
	//DrawDebugLine(GetWorld(),
	//GetActorLocation(),
	//GetActorLocation() + GetActorUpVector() * 1000,
	//FColor::Blue);
	//DrawDebugLine(GetWorld(),
	//GetActorLocation(),
	//GetActorLocation() + GetActorRightVector() * 1000,
	//FColor::Yellow);
	//DrawDebugLine(GetWorld(),
	//GetActorLocation(),
	//GetActorLocation() + WandMesh->GetComponentRotation().Vector().RotateAngleAxis(90, GetActorRightVector()) * 1000,
	//FColor::White);

	TryFire();
}

void AWand::SetHand(EControllerHand SetHand)
{
	Hand = SetHand;
	MotionController->SetTrackingSource(Hand);
}

/* Lumos spell activation */
void AWand::TriggerLumos()
{
	FActorSpawnParameters SpawnParams;
	FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	if (!LumosLight) 
	{
		LumosLight = GetWorld()->SpawnActor<APointLight>(WandMesh->GetSocketLocation(TEXT("WandEnd")), GetActorRotation(), SpawnParams);
		if (!ensure(LumosLight)) { return; }
		LumosLight->AttachToComponent(WandMesh, AttachRules, TEXT("WandEnd"));
		LumosLightSphere->SetVisibility(true);
	}
	else
	{ 
		LumosLight->Destroy(); 
		LumosLight = nullptr;
		LumosLightSphere->SetVisibility(false);
	}
}

/* Periculum spell activation */
void AWand::TriggerPericulum()
{
	PericulumFire.Broadcast(WandMesh);
}

/* Protego spell activation */
void AWand::TriggerProtego()
{
	ProtegoFire.Broadcast(WandMesh);
}

/* Stupefy spell activation */
void AWand::TriggerStupefy()
{
	StupefyFire.Broadcast(WandMesh);

	// Projectile tracing to match with niagara effect
	float ProjectileSpeed = 3000;
	float ProjectileTime = 4;
	WandProjectileTrace(ProjectileSpeed, ProjectileTime);
}

/* Activates spell effect for different spells */
void AWand::SpellTrigger()
{
		switch (SelectedSpell)
		{
		case ESpell::Lumos:
			UE_LOG(LogTemp, Warning, TEXT("lumos"))
			TriggerLumos();
			break;
		case ESpell::Periculum:
			UE_LOG(LogTemp, Warning, TEXT("periculum"))
			TriggerPericulum();
			break;
		case ESpell::Protego:
			UE_LOG(LogTemp, Warning, TEXT("protego"))
			TriggerProtego();
			break;
		case ESpell::Stupefy:
			UE_LOG(LogTemp, Warning, TEXT("stupefy"))
				TriggerStupefy();
			break;
		}
}

/* Projectile trace to find collisions of spell */
bool AWand::WandProjectileTrace(float ProjectileSpeed, float ProjectileTime)
{
	FTransform WandEndTransform = WandMesh->GetSocketTransform(TEXT("WandEnd"));
	FVector StartLocation = WandEndTransform.GetLocation();
	FVector Direction = WandMesh->GetComponentRotation().Vector().RotateAngleAxis(90, GetActorRightVector());
	// Spawn and fire projectile to get flight time pathing
	if (!ensure(ProjectileBlueprint)) { return false; }

	bool bNoCollisionFail = true;
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = bNoCollisionFail ? ESpawnActorCollisionHandlingMethod::AlwaysSpawn : ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	ACollisionProjectile* SpawnedProjectile = GetWorld()->SpawnActor<ACollisionProjectile>(ProjectileBlueprint, StartLocation, FRotator::ZeroRotator, ActorSpawnParams);	SpawnedProjectile->LaunchProjectile(Direction * ProjectileSpeed);

	return false;
}

/* Check if wand was fast but is slow now + delay timer */
bool AWand::bBaseCheckForSpell()
{
	float WandVelocity = WandMesh->GetPhysicsAngularVelocityInDegrees().Size();
	//UE_LOG(LogTemp, Warning, TEXT("Diff %f"), Difference)
	if (bVelocityForSpell() && WandVelocity < 50 && GetWorld()->GetTimeSeconds() - WandActivationTimer > 0.5)
	{
		WandActivationTimer = GetWorld()->GetTimeSeconds();
		return true;
	}
	return false;
}

/* Check to see if the wand has enough angular velocity for spell activation */
bool AWand::bVelocityForSpell()
{
	//float WandVel = WandMesh->ComponentVelocity.Size();
	//UE_LOG(LogTemp, Warning, TEXT("WandVel %f"), WandVel)
	float WandVelocity = WandMesh->GetPhysicsAngularVelocityInDegrees().Size();
	//UE_LOG(LogTemp, Warning, TEXT("WandVel %f"), WandVelocity)
	if (WandVelocityHistory.Num() < WandVelocityHistoryMax)
	{
		WandVelocityHistory.Add(WandVelocity);
	}
	else
	{
		WandVelocityHistory.RemoveAt(0, 1, true);
		WandVelocityHistory.Add(WandVelocity);
	}

	if (WandVelocityHistory.Num() == WandVelocityHistoryMax)
	{
		/*
		Acceleration must be very large
		Velocity must be near-zero
		*/
		float Difference = WandVelocityHistory[WandVelocityHistoryMax - 1] - WandVelocityHistory[0];
		return abs(Difference) > SpellActivationVelocity;
	}
	return false;
}

/* Check to see if the wand is vertical enough for spell activation */
bool AWand::bVerticalSpell()
{
	return true; // TODO richer return value
}

/* Checking activation systems for different spells */
bool AWand::bCanTriggerSpell()
{
	switch (SelectedSpell)
	{
	case ESpell::Lumos:
		return bBaseCheckForSpell();
		break;
	case ESpell::Periculum:
		return bBaseCheckForSpell() && bVerticalSpell();
		break;
	case ESpell::Protego:
		return bVelocityForSpell();
		break;
	case ESpell::Stupefy:
		return bBaseCheckForSpell();
		break;
	}
	return false;
}

/* External call from VRCharacter to start spell mechanism */
void AWand::TryFire()
{
	if (bCanTriggerSpell()) { SpellTrigger(); }
}
