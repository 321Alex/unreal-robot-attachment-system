#include "RobotTorso.h"
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

void ARobotTorso::OnInteract_Implementation()
{
	// Intentionally empty.
	// The torso participates in the interaction system, so it can be detected
	// by the controller, but currently no additional behavior is required.
}