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

	UPROPERTY(BlueprintAssignable, Category="UI Events")
	FOnPartStateChanged OnPartStateChanged;

	UFUNCTION()
	void OnMouseClick();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="Hover")
	float HoverPollInterval = 0.1f;

private:
	void HandleNewClick(AActor* Actor);
	void HandleDropOrAttach(const FHitResult& Hit);

	void EndDragAndResumeHover(bool bClearDraggedHighlight);

	void UpdateCurrentTarget();
	void SetCurrentTarget(AActor* NewTarget);

	void StartHighlightTimer();
	void StopHighlightTimer();

	void SetHighlightIfPresent(AActor* Actor, bool bOn);

	FTimerHandle HighlightTimerHandle;

	UPROPERTY()
	TObjectPtr<APlayerController> CachedPC = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> DraggedActor = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget = nullptr;
};