#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HighlightStrategy.generated.h"

// Strategy interface for highlight behavior.
// Lifecycle:
// - Setup() called once (cache materials/components/etc.)
// - Apply()/Clear() called as highlight toggles
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class ROBOTABUSE_API UHighlightStrategy : public UObject
{
	GENERATED_BODY()

public:
	// Optional one-time setup hook.
	virtual void Setup(AActor* Target) {}
	
	virtual void Apply(AActor* Target) PURE_VIRTUAL(UHighlightStrategy::Apply, );
	virtual void Clear(AActor* Target) PURE_VIRTUAL(UHighlightStrategy::Clear, );
};
