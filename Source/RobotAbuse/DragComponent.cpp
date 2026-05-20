#include "DragComponent.h"

#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

UDragComponent::UDragComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// DragComponent only needs to tick while dragging
	SetComponentTickEnabled(false);
}

void UDragComponent::BeginDrag(APlayerController* PC, const FHitResult* GrabHit)
{
	check(PC);
	AActor* Owner = GetOwner();
	check(Owner);

	DragController = PC;

	FVector GrabWorldLocation = Owner->GetActorLocation();
	if (GrabHit && GrabHit->bBlockingHit && GrabHit->GetActor() == Owner)
	{
		GrabWorldLocation = GrabHit->ImpactPoint;
	}

	GrabOffsetLocal = Owner->GetActorTransform().InverseTransformPosition(GrabWorldLocation);

	// Cache the camera-to-grab-point distance so the clicked spot stays locked to the mouse ray.
	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);

	InitialDragDistance = FVector::Dist(CamLoc, GrabWorldLocation);

	InitialDragDistance = FMath::Clamp(InitialDragDistance, MinDragDistance, MaxDragDistance);

	bDragging = true;
	SetComponentTickEnabled(true);
}

void UDragComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bDragging)
	{
		return;
	}

	check(DragController);

	FVector MouseWorldOrigin;
	FVector MouseWorldDir;
	if (!DragController->DeprojectMousePositionToWorld(MouseWorldOrigin, MouseWorldDir))
	{
		return;
	}

	// Project the original grabbed point along the mouse ray at the cached distance.
	const FVector GrabWorldPos = MouseWorldOrigin + (MouseWorldDir * InitialDragDistance);

	if (AActor* Owner = GetOwner())
	{
		const FVector TargetActorLocation = GrabWorldPos - Owner->GetActorTransform().TransformVector(GrabOffsetLocal);
		ApplyWorldPosition(TargetActorLocation, DeltaTime);
	}
}

void UDragComponent::EndDrag()
{
	bDragging = false;
	InitialDragDistance = 0.f;
	GrabOffsetLocal = FVector::ZeroVector;
	DragController = nullptr;

	SetComponentTickEnabled(false);
}

void UDragComponent::ApplyWorldPosition(const FVector& WorldPos, float DeltaTime)
{
	// Apply optional designer offset after the automatic grab-point correction.
	const FVector TargetPos = bUseOffset ? (WorldPos + DragOffset) : WorldPos;

	if (AActor* Owner = GetOwner())
	{
		const FVector CurrentPos = Owner->GetActorLocation();

		// Smoothly move toward the target position.
		const FVector NewPos = FMath::VInterpTo(CurrentPos, TargetPos, DeltaTime, DragSmoothSpeed);

		Owner->SetActorLocation(NewPos);
	}
}

