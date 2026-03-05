#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HighlightComponent.generated.h"

class UHighlightStrategy;

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

	UFUNCTION(BlueprintPure, Category="Highlight")
	bool IsHighlighted() const { return bIsHighlighted; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleInstanceOnly, Category="Highlight")
	bool bIsHighlighted = false;
};
