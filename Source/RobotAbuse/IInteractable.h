#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IInteractable.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for actors that can respond to player interaction.
 * Called when the player expresses interaction intent (e.g., click, use key).
 * Implementable in Blueprint or C++ (BlueprintNativeEvent).
 */
class ROBOTABUSE_API IInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
	void OnInteract();
};