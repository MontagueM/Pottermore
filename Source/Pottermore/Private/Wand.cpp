// Fill out your copyright notice in the Description page of Project Settings.


#include "Wand.h"
#include "Components/StaticMeshComponent.h"
#include "MotionControllerComponent.h"
#include "Engine/PointLight.h" 

#include "DrawDebugHelpers.h" 

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

	TryFire();
}

void AWand::SetHand(EControllerHand SetHand)
{
	Hand = SetHand;
	MotionController->SetTrackingSource(Hand);
}

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

void AWand::TriggerPericulum()
{
	PericulumFire.Broadcast(WandMesh);
}

void AWand::TriggerProtego()
{
	ProtegoFire.Broadcast(WandMesh);
}

void AWand::DebugSpell()
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
		}
}

bool AWand::bWandVelocityForSpell()
{
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
		//UE_LOG(LogTemp, Warning, TEXT("Diff %f"), Difference)
		if (abs(Difference) > SpellActivationVelocity && WandVelocity < 50 && GetWorld()->GetTimeSeconds() - WandActivationTimer > 0.5)
		{
			WandActivationTimer = GetWorld()->GetTimeSeconds();
			return true;
		}
	}
	return false;
}

void AWand::TryFire()
{
	if (bWandVelocityForSpell()) { DebugSpell(); }
}
