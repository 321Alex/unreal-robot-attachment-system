#pragma once

#include "CoreMinimal.h"
#include "HighlightStrategy.h"
#include "Components/ActorComponent.h"
#include "HighlightComponent.generated.h"

// Component that toggles highlight on its owning actor using a pluggable Strategy.
// Strategy is instanced per-component (not a shared asset), allowing different highlight behaviors per actor.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ROBOTABUSE_API UHighlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Pluggable highlight behavior (instanced per component).
	UPROPERTY(EditAnywhere, Instanced, Category="Highlight")
	TObjectPtr<UHighlightStrategy> Strategy = nullptr;

	UFUNCTION(BlueprintCallable, Category="Highlight")
	void SetHighlighted(bool bHighlighted);

	UFUNCTION(BlueprintCallable, Category="Highlight")
	void SetHighlightState(EHighlightVisualState NewState);

	UFUNCTION(BlueprintPure, Category="Highlight")
	bool IsHighlighted() const { return HighlightState != EHighlightVisualState::None; }

	UFUNCTION(BlueprintPure, Category="Highlight")
	EHighlightVisualState GetHighlightState() const { return HighlightState; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleInstanceOnly, Category="Highlight")
	EHighlightVisualState HighlightState = EHighlightVisualState::None;
};
