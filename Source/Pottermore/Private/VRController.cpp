// Fill out your copyright notice in the Description page of Project Settings.


#include "VRController.h"
#include "MotionControllerComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineMeshComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Engine/StaticMeshActor.h" 
#include "Kismet/KismetMathLibrary.h" 

#include "DrawDebugHelpers.h" 

using namespace std;

// Sets default values
AVRController::AVRController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	SetRootComponent(MotionController);

	DestinationMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DestinationMarker"));
	DestinationMarker->SetupAttachment(GetRootComponent());
	DestinationMarker->SetWorldScale3D(DestinationMarkerScale);

	MarkerPoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MarkerPoint"));
	MarkerPoint->SetupAttachment(DestinationMarker);

	TeleportPath = CreateDefaultSubobject<USplineComponent>(TEXT("TeleportPath"));
	TeleportPath->SetupAttachment(GetRootComponent());

	ControllerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ControllerMesh"));
	ControllerMesh->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AVRController::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AVRController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCanHandTeleport() && bCanCheckTeleport)
	{
		bAllowCharacterTeleport = UpdateTeleportationCheck();
	}
}

void AVRController::SetHand(EControllerHand SetHand) {
	Hand = SetHand;
	MotionController->SetTrackingSource(Hand);
	if (Hand == EControllerHand::Left)
	{
		ControllerMesh->SetStaticMesh(LeftControllerMesh);
	}
	else if (Hand == EControllerHand::Right)
	{
		ControllerMesh->SetStaticMesh(RightControllerMesh);
	}
}

bool AVRController::FindTeleportDestination(FVector& Location)
{
	/// Using rotateangleaxis for easiness in teleportation handling (rotates it down from the controller)

	FVector StartLocation = GetActorLocation() + GetActorForwardVector() * 5;
	FVector Direction = GetActorForwardVector().RotateAngleAxis(15, GetActorRightVector());

	FPredictProjectilePathResult Result;
	bool bHit = ProjectilePathingUpdate(Result,
		TeleportProjectileRadius,
		StartLocation,
		Direction,
		TeleportProjectileSpeed,
		TeleportSimulationTime,
		ECollisionChannel::ECC_Visibility);
	UpdateSpline(PathPointDataToFVector(Result.PathData), TeleportPath);

	/// We want to make sure we are also allowed to teleport there
	UNavigationSystemV1* UNavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!ensure(UNavSystem)) { return false; }
	FNavLocation NavLocation;
	bool bValidDestination = UNavSystem->ProjectPointToNavigation(FVector(Result.HitResult.Location), NavLocation, TeleportNavExtent);
	Location = NavLocation.Location;

	return bHit && bValidDestination;
}

bool AVRController::ProjectilePathingUpdate(FPredictProjectilePathResult& Result, float ProjectileRadius, FVector StartLocation, FVector Direction, float ProjectileSpeed, float SimulationTime, ECollisionChannel CollisionChannel)
{
	FPredictProjectilePathParams Params(ProjectileRadius,
		StartLocation,
		Direction * ProjectileSpeed,
		SimulationTime,
		CollisionChannel,
		this);
	//Params.DrawDebugType = EDrawDebugTrace::ForOneFrame;
	Params.bTraceComplex = true; // to stop it not showing teleport places due to weird collisions in the map
	Params.SimFrequency = TeleportSimulationFrequency; // dictates smoothness of arc
	bool bHit = UGameplayStatics::PredictProjectilePath(this, Params, Result);
	/// Draw teleport curve, annoying its here though could move it
	return bHit;
}

TArray<FVector> AVRController::PathPointDataToFVector(TArray<FPredictProjectilePathPointData> PathData)
{
	TArray<FVector> OutArray;
	for (FPredictProjectilePathPointData Point : PathData) { OutArray.Add(Point.Location); }
	return OutArray;
}

void AVRController::UpdateSpline(TArray<FVector> PathData, USplineComponent* PathToUpdate)
{
	// to hide left over spline components
	ModifySplinePoints(PathToUpdate, true, true);
	for (int i = 0; i < PathData.Num(); i++)
	{
		if (PathData.Num() > MeshObjects.Num()) // only add if we need to add another one
		{

			SplineMesh = NewObject<USplineMeshComponent>(this);
			SplineMesh->SetStaticMesh(TeleportArcMesh);
			SplineMesh->SetMaterial(0, TeleportArcMaterial);
			SplineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SplineMesh->RegisterComponent();
			MeshObjects.Add(SplineMesh);
		}
		PathToUpdate->AddSplinePoint(PathData[i], ESplineCoordinateSpace::Local, ESplinePointType::Curve);
		/// Orienting the meshes
		if (i > 0)
		{
			SplineMesh = MeshObjects[i - 1];
			FVector LocationStart, TangentStart, LocationEnd, TangentEnd;
			PathToUpdate->GetLocalLocationAndTangentAtSplinePoint(i - 1, LocationStart, TangentStart);
			PathToUpdate->GetLocalLocationAndTangentAtSplinePoint(i, LocationEnd, TangentEnd);
			SplineMesh->SetStartAndEnd(LocationStart, TangentStart, LocationEnd, TangentEnd);
			MeshObjects[i - 1]->SetVisibility(true);
			if (bCanCheckTeleport)
			{
				if (i == PathData.Num() - 1)
				{
					MarkerPoint->SetWorldLocation(LocationEnd);
				}
			}
		}
	}
	if (bCanCheckTeleport) { MarkerPoint->SetVisibility(true); }
}

bool AVRController::UpdateTeleportationCheck()
{
	/// Destination for teleport
	FVector TeleportLocation;
	bool bTeleportDestinationExists = FindTeleportDestination(TeleportLocation);
	if (bTeleportDestinationExists && bCanHandTeleport() && bCanCheckTeleport)
	{
		FCollisionQueryParams TraceParams(FName(TEXT("Trace")), false, GetOwner());
		/// Ray-cast out to reach distance
		FHitResult Hit;
		GetWorld()->LineTraceSingleByObjectType(OUT Hit,
			TeleportLocation,
			TeleportLocation + FVector(0, 0, -200),
			FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
			TraceParams
		);
		if (Hit.bBlockingHit) { DestinationMarker->SetWorldLocation(Hit.Location); }
		else { DestinationMarker->SetWorldLocation(TeleportLocation); }

		DestinationMarker->SetWorldRotation(FRotator::ZeroRotator);
		DestinationMarker->SetVisibility(true);
		return true; // We only want to allow teleporting if not trying to flick
	}
	else
	{
		DestinationMarker->SetVisibility(false);
		return false;
	}
}

bool AVRController::bCanHandTeleport()
{
	if (Hand == TeleportHand) { return true; }

	return false;
}

bool AVRController::bCanHandMove()
{
	if (Hand == TeleportHand) { return false; }

	return true;
}

void AVRController::SetCanCheckTeleport(bool bCheck)
{
	bCanCheckTeleport = bCheck;
	DestinationMarker->SetVisibility(false);
	MarkerPoint->SetVisibility(false);
	ModifySplinePoints(TeleportPath, true, true);
}

void AVRController::ModifySplinePoints(USplineComponent* PathToUpdate, bool bHidePoints, bool bClear)
{
	for (USplineMeshComponent* u : MeshObjects)
	{
		u->SetVisibility(!bHidePoints);
	}
	if (bClear) { PathToUpdate->ClearSplinePoints(true); }
	//UE_LOG(LogTemp, Error, TEXT("ClearSplinePoints"))
}