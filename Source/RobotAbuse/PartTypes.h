#pragma once

#include "CoreMinimal.h"
#include "PartTypes.generated.h"

// Types of attachable robot parts used for attachment compatibility checks.
UENUM(BlueprintType)
enum class EPartType : uint8
{
	LeftArm    UMETA(DisplayName = "Left Arm"),
	RightArm   UMETA(DisplayName = "Right Arm"),
	Universal  UMETA(DisplayName = "Universal")
};