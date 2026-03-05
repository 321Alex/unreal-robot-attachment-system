#include "HighlightComponent.h"
#include "HighlightStrategy.h"
#include "GameFramework/Actor.h"

void UHighlightComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	ensureMsgf(Owner, TEXT("HighlightComponent has no owner (unexpected)."));
	if (!Owner)
	{
		return;
	}

	// Strategy is required for highlight behavior. If it's missing, the component is misconfigured.
	ensureMsgf(Strategy, TEXT("HighlightComponent on '%s' is missing a Strategy."), *GetNameSafe(Owner));
	if (!Strategy)
	{
		return;
	}

	// One-time setup so the strategy can cache materials/components/etc.
	Strategy->Setup(Owner);
}

void UHighlightComponent::SetHighlighted(bool bHighlighted)
{
	// Avoid redundant work when the requested state matches the current state.
	if (bIsHighlighted == bHighlighted)
	{
		return;
	}

	bIsHighlighted = bHighlighted;

	AActor* Owner = GetOwner();
	ensureMsgf(Owner, TEXT("HighlightComponent has no owner (unexpected)."));
	if (!Owner)
	{
		return;
	}

	// Missing strategy is a configuration error; we log once per call site via ensure.
	ensureMsgf(Strategy, TEXT("HighlightComponent on '%s' is missing a Strategy."), *GetNameSafe(Owner));
	if (!Strategy)
	{
		return;
	}

	if (bIsHighlighted)
	{
		Strategy->Apply(Owner);
	}
	else
	{
		Strategy->Clear(Owner);
	}
}

