#pragma once

#include "IInteractable.h"
#include "PartTypes.h"
#include "GameFramework/Actor.h"
#include "AttachablePart.generated.h"

class AAttachmentPointActor;
class UStaticMeshComponent;

// High-level interaction state for an attachable robot part.
// This drives both gameplay behavior (attach/detach/pickup) and UI readouts.
UENUM(BlueprintType)
enum class EPartState : uint8
{
	ATTACHED,
	HELD,
	DETACHED
};

UCLASS()
class ROBOTABUSE_API AAttachablePart : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AAttachablePart();
	
	virtual void OnInteract_Implementation(AActor* InteractingActor) override;

	// Player actions
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void PickUp(AActor* InteractingActor);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Drop();

	// State queries (used by UI / gameplay logic)
	UFUNCTION(BlueprintPure, Category = "State")
	bool IsAttached() const { return CurrentState == EPartState::ATTACHED; }

	UFUNCTION(BlueprintPure, Category = "State")
	bool IsHeld() const { return CurrentState == EPartState::HELD; }

	UFUNCTION(BlueprintPure, Category = "State")
	AAttachmentPointActor* GetAttachmentPoint() const { return CurrentAttachmentPoint; }

	// Request detachment from the current point (if any). The point manages its own slot/state,
	// then we clear our local attachment reference.
	UFUNCTION(BlueprintCallable, Category = "Attachment")
	void RequestDetachFromPoint();

	// What kind of attachment point this part can connect to.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Part Type")
	EPartType PartType = EPartType::Universal;

	// Current interaction/attachment state. Kept BlueprintReadOnly so UI can observe it
	// while gameplay code remains the authority for state transitions.
	UPROPERTY(BlueprintReadOnly, Category = "State")
	EPartState CurrentState = EPartState::DETACHED;

	// Called by an attachment point when this part becomes attached.
	void OnAttachToPoint(AAttachmentPointActor* Point);

	// Clears the attachment reference and updates the state.
	void OnDetachFromPoint();

protected:
	// Visual representation + collision for traces/interactions.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent = nullptr;

	// Convenience for UI: returns a human-readable string for the current state.
	UFUNCTION(BlueprintCallable, Category = "Part State")
	FString GetStateAsString() const
	{
		switch (CurrentState)
		{
		case EPartState::DETACHED: return TEXT("Detached");
		case EPartState::HELD:     return TEXT("Being Dragged");
		case EPartState::ATTACHED: return TEXT("Attached");
		default:                   return TEXT("Unknown");
		}
	}

	// The attachment point we are currently connected to (if any).
	UPROPERTY(BlueprintReadOnly, Category = "State")
	AAttachmentPointActor* CurrentAttachmentPoint = nullptr;
};
