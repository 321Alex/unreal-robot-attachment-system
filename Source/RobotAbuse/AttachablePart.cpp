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

void AAttachablePart::OnInteract_Implementation(AActor* InteractingActor)
{
	if (!InteractingActor)
	{
		return;
	}

	URobotInteractionComponent* InteractionComp =
		InteractingActor->FindComponentByClass<URobotInteractionComponent>();

	if (!InteractionComp)
	{
		return;
	}

	switch (CurrentState)
	{
	case EPartState::DETACHED:
		PickUp(InteractingActor);
		break;

	case EPartState::ATTACHED:
		RequestDetachFromPoint();
		PickUp(InteractingActor);
		break;

	default:
		break;
	}

	InteractionComp->BeginDraggingActor(this);
}

void AAttachablePart::PickUp(AActor* InteractingActor)
{
	if (!InteractingActor)
	{
		return;
	}

	URobotInteractionComponent* InteractionComp =
		InteractingActor->FindComponentByClass<URobotInteractionComponent>();

	if (!InteractionComp)
	{
		return;
	}

	InteractionComp->BeginDraggingActor(this);
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
