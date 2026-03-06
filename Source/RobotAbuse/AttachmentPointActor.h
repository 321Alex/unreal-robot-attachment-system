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

	UFUNCTION(BlueprintPure, Category="Attachment")
	bool IsAvailable() const { return AttachedPart == nullptr; }

	UFUNCTION(BlueprintCallable, Category="Attachment")
	bool CanAcceptPart(AAttachablePart* Part) const;

	UFUNCTION(BlueprintCallable, Category="Attachment")
	bool TryAttachPart(AAttachablePart* Part);

	UFUNCTION(BlueprintCallable, Category="Attachment")
	bool DetachPart(AAttachablePart* Part);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attachment", meta=(AllowPrivateAccess="true"))
	EPartType AcceptedArmType = EPartType::Universal;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USceneComponent> SnapPoint = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USphereComponent> Collision = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStaticMeshComponent> Visual = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UChildActorComponent> InitialPartChild = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<AAttachablePart> AttachedPart = nullptr;

	void RegisterInitialPartIfAny();
	void RefreshAvailabilityVisuals();
};
