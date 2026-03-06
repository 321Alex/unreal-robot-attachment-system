#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RobotInteractionComponent.generated.h"

class AAttachablePart;
class APlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPartStateChanged, AAttachablePart*, Part);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ROBOTABUSE_API URobotInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URobotInteractionComponent();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void OnMouseClick();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void BeginDraggingActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void EndDragAndResumeHover(bool bClearDraggedHighlight = true);

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnPartStateChanged OnPartStateChanged;

private:
	void HandleNewClick(AActor* Actor);
	void HandleDropOrAttach(const FHitResult& Hit);

	void StartHighlightTimer();
	void StopHighlightTimer();
	void UpdateCurrentTarget();
	void SetCurrentTarget(AActor* NewTarget);
	void SetHighlightIfPresent(AActor* Actor, bool bOn);

private:
	UPROPERTY()
	TObjectPtr<APlayerController> CachedPC = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> DraggedActor = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget = nullptr;

	FTimerHandle HighlightTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float HoverPollInterval = 0.03f;
};