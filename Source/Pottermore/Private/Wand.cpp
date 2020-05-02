// Fill out your copyright notice in the Description page of Project Settings.


#include "Wand.h"
#include "Components/StaticMeshComponent.h"
#include "MotionControllerComponent.h"
#include "Engine/PointLight.h" 


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
}

// Called when the game starts or when spawned
void AWand::BeginPlay()
{
	Super::BeginPlay();
	if (ensure(WandMesh)) { WandEndLocation = WandMesh->GetSocketLocation(TEXT("WandEnd")); }
}

// Called every frame
void AWand::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	UE_LOG(LogTemp, Warning, TEXT("trigger lumos 1"))
	if (!Light) 
	{
		UE_LOG(LogTemp, Warning, TEXT("trigger lumos 2"))
		Light = GetWorld()->SpawnActor<APointLight>(WandEndLocation, GetActorRotation(), SpawnParams);
		if (!ensure(Light)) { return; }
		Light->AttachToComponent(WandMesh, AttachRules, TEXT("WandEnd"));
		UE_LOG(LogTemp, Warning, TEXT("trigger lumos 3"))
	}
	else { UE_LOG(LogTemp, Warning, TEXT("trigger lumos 4")) Light->Destroy(); Light = nullptr; }
}

void AWand::DebugSpell()
{
	UE_LOG(LogTemp, Warning, TEXT("debug spell in wand"))
		if (SelectedSpell == ESpell::Lumos) { UE_LOG(LogTemp, Warning, TEXT("lumos")) TriggerLumos(); }
		else { UE_LOG(LogTemp, Warning, TEXT("not lumos")) }
}