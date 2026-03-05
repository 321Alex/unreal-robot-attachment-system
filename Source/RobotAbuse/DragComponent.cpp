#include "DragComponent.h"

#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

UDragComponent::UDragComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// DragComponent only needs to tick while dragging
	SetComponentTickEnabled(false);
}

void UDragComponent::BeginDrag(APlayerController* PC)
{
	check(PC);

	AActor* Owner = GetOwner();
	check(Owner);

	DragController = PC;

	// Cache the camera-to-object distance at drag start so we can keep the object
	// "locked" to the mouse ray at a stable depth (prevents it from flying toward/away from the camera).
	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);

	InitialDragDistance = FVector::Dist(CamLoc, Owner->GetActorLocation());

	// Optional: clamp distance so dragging can't go absurdly close/far due to set up issues.
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

	// Project a point along the mouse ray at the cached distance.
	const FVector RawWorldPos = MouseWorldOrigin + (MouseWorldDir * InitialDragDistance);

	ApplyWorldPosition(RawWorldPos, DeltaTime);
}

void UDragComponent::EndDrag()
{
	bDragging = false;
	InitialDragDistance = 0.f;
	DragController = nullptr;

	SetComponentTickEnabled(false);
}

void UDragComponent::ApplyWorldPosition(const FVector& WorldPos, float DeltaTime)
{
	// Apply optional offset because the mesh pivot/config may not line up directly under the cursor.
	const FVector TargetPos = bUseOffset ? (WorldPos + DragOffset) : WorldPos;

	if (AActor* Owner = GetOwner())
	{
		const FVector CurrentPos = Owner->GetActorLocation();

		// Smoothly move toward the target position.
		const FVector NewPos = FMath::VInterpTo(CurrentPos, TargetPos, DeltaTime, DragSmoothSpeed);

		Owner->SetActorLocation(NewPos);
	}
}

