#pragma once

#include "CoreMinimal.h"
#include "InteractionOutcome.generated.h"

UENUM(BlueprintType)
enum class EInteractionOutcome : uint8
{
	None      UMETA(DisplayName = "None"),
	StartDrag UMETA(DisplayName = "Start Drag")
};