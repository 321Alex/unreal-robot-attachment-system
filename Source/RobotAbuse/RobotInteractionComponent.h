#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RobotInteractionComponent.generated.h"

class AActor;
class AAttachablePart;
class APlayerController;
class UUserWidget;
struct FHitResult;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPartStateChanged, AAttachablePart*, Part);

//Orchestrates click interaction + hover highlight + drag/attach flow. 
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ROBOTABUSE_API URobotInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URobotInteractionComponent();

	// UI can listen for selection/drag state changes (null when no part is selected).
	UPROPERTY(BlueprintAssignable, Category="UI Events")
	FOnPartStateChanged OnPartStateChanged;

	// Call from Pawn input binding.
	UFUNCTION()
	void OnMouseClick();

protected:
	virtual void BeginPlay() override;

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
	// Widget to display the current selected part status.
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> ArmStatusWidgetClass;

	// How often to poll under-cursor hit tests for hover highlight.
	UPROPERTY(EditDefaultsOnly, Category="Hover")
	float HoverPollInterval = 0.1f;

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