#pragma once

#include "CoreMinimal.h"
#include "PartTypes.h"
#include "GameFramework/Actor.h"
#include "AttachmentPointActor.generated.h"

class AAttachablePart;
class UChildActorComponent;
class USceneComponent;
class USphereComponent;
class UStaticMeshComponent;

// AttachmentPointActor represents a "socket" on the robot that can accept compatible parts.
// - Provides a snap transform for attached parts
// - Exposes a clickable/traceable collider for interaction
// - Optionally spawns an initial part via ChildActorComponent at startup
UCLASS()
class ROBOTABUSE_API AAttachmentPointActor : public AActor
{
	GENERATED_BODY()

public:
	AAttachmentPointActor();

	// Root for a clean BP-friendly component hierarchy.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> Root = nullptr;

	// SnapPoint is the parent transform for the attached part. This allows offset/rotation tuning in BP.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USceneComponent> SnapPoint = nullptr;

	// Click/trace the target (intended for mouse traces / interaction).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USphereComponent> Collision = nullptr;

	// Visual indicator for availability (configured in BP by selecting this component).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> Visual = nullptr;

	// Optional: set a ChildActorClass in BP to start with a part already attached.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UChildActorComponent> InitialPartChild = nullptr;

	// Which parts can attach here. Universal accepts anything.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attachment")
	EPartType AcceptedArmType = EPartType::Universal;

	// True if no part is currently occupying this socket.
	UFUNCTION(BlueprintCallable, Category="Attachment")
	bool IsAvailable() const { return AttachedPart == nullptr; }

	// Returns true if the given part is compatible and this socket is currently available.
	UFUNCTION(BlueprintCallable, Category="Attachment")
	bool CanAcceptPart(AAttachablePart* Part) const;

	// Attempts to attach the part and update bookkeeping/visuals.
	UFUNCTION(BlueprintCallable, Category="Attachment")
	bool TryAttachPart(AAttachablePart* Part);

	// Attempts to detach the provided part (must match the currently attached part).
	UFUNCTION(BlueprintCallable, Category="Attachment")
	bool DetachPart(AAttachablePart* Part);

protected:
	// Runs in editor when properties/transform change, and also at spawn time.
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

private:
	// Transient runtime occupancy (not intended to be serialized).
	UPROPERTY(Transient)
	TObjectPtr<AAttachablePart> AttachedPart = nullptr;

	// If InitialPartChild has a valid AAttachablePart child at startup, register it as attached.
	void RegisterInitialPartIfAny();

	// Toggle marker visibility and collision depending on whether the socket is available.
	void RefreshAvailabilityVisuals();
};
