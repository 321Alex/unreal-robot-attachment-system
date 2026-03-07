#include "RobotTorso.h"

#include "RobotInteractionComponent.h"
#include "Components/StaticMeshComponent.h"

// The torso is the root actor for the robot assembly. 
// It supports interaction through the IInteractable interface,
// but currently does not require any special logic when clicked.
ARobotTorso::ARobotTorso()
{
	PrimaryActorTick.bCanEverTick = false;

	RootMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootMesh"));
	RootComponent = RootMesh;

	RootMesh->SetSimulatePhysics(false);
}

EInteractionOutcome ARobotTorso::OnInteract_Implementation()
{
	return EInteractionOutcome::StartDrag;
}