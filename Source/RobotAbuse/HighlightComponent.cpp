#include "HighlightComponent.h"
#include "HighlightStrategy.h"
#include "GameFramework/Actor.h"

UHighlightComponent::UHighlightComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UHighlightComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	// Strategy is required for highlight behavior. If it's missing, the component is misconfigured.
	ensureMsgf(Strategy, TEXT("HighlightComponent on '%s' is missing a Strategy."), *GetNameSafe(Owner));
	if (!Strategy)
	{
		return;
	}

	// One-time setup so the strategy can cache materials/components/etc.
	Strategy->Setup(Owner);
}

void UHighlightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Strategy && HighlightState != EHighlightVisualState::None)
	{
		Strategy->TickHighlight(GetOwner(), HighlightState, DeltaTime);
	}
}

void UHighlightComponent::SetHighlighted(bool bHighlighted)
{
	SetHighlightState(bHighlighted ? EHighlightVisualState::Hover : EHighlightVisualState::None);
}

void UHighlightComponent::SetHighlightState(EHighlightVisualState NewState)
{
	AActor* Owner = GetOwner();

	// Missing strategy is a configuration error; we log once per call site via ensure.
	ensureMsgf(Strategy, TEXT("HighlightComponent on '%s' is missing a Strategy."), *GetNameSafe(Owner));
	if (!Strategy)
	{
		return;
	}

	if (HighlightState == NewState)
	{
		return;
	}

	HighlightState = NewState;

	if (HighlightState != EHighlightVisualState::None)
	{
		SetComponentTickEnabled(true);
		Strategy->Apply(Owner, HighlightState);
	}
	else
	{
		SetComponentTickEnabled(false);
		Strategy->Clear(Owner);
	}
}

