#include "AttachablePart.h"

#include "AttachmentPointActor.h"
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

void AAttachablePart::OnInteract_Implementation()
{
    // Single interaction entry point.
    // - If detached, interacting picks the part up.
    // - If attached, interacting detaches it from its current point and then picks it up.
    switch (CurrentState)
    {
    case EPartState::DETACHED:
        PickUp();
        break;

    case EPartState::ATTACHED:
        RequestDetachFromPoint();
        PickUp();
        break;

    default:
        // HELD (or any future state): no-op for now.
        break;
    }
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

