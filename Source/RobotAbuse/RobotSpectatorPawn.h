#pragma once

#include "GameFramework/SpectatorPawn.h"
#include "RobotSpectatorPawn.generated.h"

class AActor;
class AAttachablePart;
class APlayerController;
class UInputComponent;
class UUserWidget;
struct FHitResult;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPartStateChanged, AAttachablePart*, Part);

// Spectator pawn that:
// - Handles mouse click interaction with IInteractable actors
// - Initiates dragging via UDragComponent
// - Manages hover highlighting via a timer (avoids per-frame hit tests)
// - Broadcasts selected part state changes for UI
UCLASS()
class ROBOTABUSE_API ARobotSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()

public:
	// UI can listen for selection/drag state changes (null when no part is selected).
	UPROPERTY(BlueprintAssignable, Category="UI Events")
	FOnPartStateChanged OnPartStateChanged;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	// ===== Input =====
	void OnMouseClick();

	// ===== Interaction =====
	void HandleNewClick(AActor* Actor);
	void HandleDropOrAttach(const FHitResult& Hit);

	// ===== Drag lifecycle =====
	void EndDragAndResumeHover(bool bClearDraggedHighlight);

	// ===== Hover / Highlight =====
	void UpdateCurrentTarget();
	void SetCurrentTarget(AActor* NewTarget);

	void StartHighlightTimer();
	void StopHighlightTimer();

	void SetHighlightIfPresent(AActor* Actor, bool bOn);

	// ===== UI =====
	void CreateArmStatusWidget();

protected:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> ArmStatusWidgetClass;

private:
	// Timer for hover detection (used instead of Tick to reduce per-frame work).
	FTimerHandle HighlightTimerHandle;

	UPROPERTY()
	TObjectPtr<APlayerController> CachedPC = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> DraggedActor = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget = nullptr;
};