// Fill out your copyright notice in the Description page of Project Settings.


#include "Wand.h"
#include "Components/StaticMeshComponent.h"
#include "MotionControllerComponent.h"



// Sets default values
AWand::AWand()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Root->SetupAttachment(GetRootComponent());

	WandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WandMesh"));
	WandMesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void AWand::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWand::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWand::SetHand(EControllerHand SetHand) {
	Hand = SetHand;
	MotionController->SetTrackingSource(Hand);
}