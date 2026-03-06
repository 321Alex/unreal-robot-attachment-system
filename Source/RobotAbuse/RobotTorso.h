#pragma once

#include "CoreMinimal.h"
#include "IInteractable.h"
#include "GameFramework/Actor.h"
#include "RobotTorso.generated.h"

class UStaticMeshComponent;

// Root actor for the robot assembly.
// The torso participates in the interaction system but currently does not
// implement additional interaction behavior beyond being selectable/highlightable.
UCLASS()
class ROBOTABUSE_API ARobotTorso : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	ARobotTorso();

	// IInteractable
	virtual void OnInteract_Implementation(AActor* InteractingActor) override;

protected:
	// Main mesh for the robot torso (acts as the root component).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* RootMesh = nullptr;
};