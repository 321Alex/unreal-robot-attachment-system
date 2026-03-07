#include "AttachablePart.h"

#include "AttachmentPointActor.h"
#include "RobotInteractionComponent.h"
#include "Components/StaticMeshComponent.h"

AAttachablePart::AAttachablePart()
{
	PrimaryActorTick.bCanEverTick = true;

	// Root mesh for the robot part.
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;

	// This part is moved/attached by gameplay code (not physics simulation), but still needs collision
	// for interactions / traces.
	MeshComponent->SetSimulatePhysics(false);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	CurrentState = EPartState::DETACHED;
}

EInteractionOutcome AAttachablePart::OnInteract_Implementation()
{
	if (CurrentState == EPartState::DETACHED)
	{
		PickUp();
		return EInteractionOutcome::StartDrag;
	}

	if (CurrentState == EPartState::ATTACHED)
	{
		RequestDetachFromPoint();
		PickUp();
		return EInteractionOutcome::StartDrag;
	}

	return EInteractionOutcome::None;
}

void AAttachablePart::PickUp()
{
	CurrentState = EPartState::HELD;
}

void AAttachablePart::Drop()
{
	CurrentState = EPartState::DETACHED;
}

void AAttachablePart::OnAttachToPoint(AAttachmentPointActor* Point)
{
	// Track the attachment point we are currently anchored to.
	CurrentAttachmentPoint = Point;
	CurrentState = EPartState::ATTACHED;
}

void AAttachablePart::OnDetachFromPoint()
{
	CurrentAttachmentPoint = nullptr;
	CurrentState = EPartState::DETACHED;
}

void AAttachablePart::RequestDetachFromPoint()
{
	if (!CurrentAttachmentPoint)
	{
		return;
	}

	// Ask the point to detach us (so the point can update its own state/slots),
	// then clear our local attachment reference/state.
	CurrentAttachmentPoint->DetachPart(this);
	OnDetachFromPoint();
}
