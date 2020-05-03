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

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Root->SetupAttachment(GetRootComponent());

	WandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WandMesh"));
	WandMesh->SetupAttachment(Root);

	LumosLightSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LumosLightSphere"));
	LumosLightSphere->SetupAttachment(Root);
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
//	if (something)
	//{
		// Cast to blueprint to get this done
	UE_LOG(LogTemp, Warning, TEXT("Trigger per c++"))
	PericulumFire.Broadcast();
	//}
}

void AWand::DebugSpell()
{
		switch (SelectedSpell)
		{
		case ESpell::Lumos:
			TriggerLumos();
		case ESpell::Periculum:
			TriggerPericulum();
		}
}