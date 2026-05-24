#pragma once

#include "CoreMinimal.h"
#include "PartTypes.generated.h"

// Types of attachable robot parts used for attachment compatibility checks.
UENUM(BlueprintType)
enum class EPartType : uint8
{
	LeftArm    UMETA(DisplayName = "Left Arm"),
	RightArm   UMETA(DisplayName = "Right Arm"),
	LeftLeg    UMETA(DisplayName = "Left Leg"),
	RightLeg   UMETA(DisplayName = "Right Leg"),
	Universal  UMETA(DisplayName = "Universal")
};

FORCEINLINE bool ArePartTypesCompatible(EPartType AcceptedType, EPartType PartType)
{
	return AcceptedType == EPartType::Universal
		|| PartType == EPartType::Universal
		|| AcceptedType == PartType;
}
