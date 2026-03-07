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

	virtual EInteractionOutcome OnInteract_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Drop();

	UFUNCTION(BlueprintPure, Category = "State")
	bool IsAttached() const { return CurrentState == EPartState::ATTACHED; }

	UFUNCTION(BlueprintPure, Category = "State")
	bool IsHeld() const { return CurrentState == EPartState::HELD; }

	UFUNCTION(BlueprintPure, Category = "State")
	AAttachmentPointActor* GetAttachmentPoint() const { return CurrentAttachmentPoint; }

	UFUNCTION(BlueprintCallable, Category = "Attachment")
	void RequestDetachFromPoint();

	void OnAttachToPoint(AAttachmentPointActor* Point);
	void OnDetachFromPoint();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Part Type")
	EPartType PartType = EPartType::Universal;

	UFUNCTION(BlueprintPure, Category = "Part State")
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	EPartState CurrentState = EPartState::DETACHED;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent = nullptr;

private:
	void PickUp();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AAttachmentPointActor> CurrentAttachmentPoint = nullptr;
};
