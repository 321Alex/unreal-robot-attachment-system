#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DragComponent.generated.h"

class APlayerController;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ROBOTABUSE_API UDragComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDragComponent();

	void BeginDrag(APlayerController* PC);
	void EndDrag();

protected:
	// Component owns drag updates while dragging (tick enabled only during drag).
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void ApplyWorldPosition(const FVector& WorldPos, float DeltaTime);

private:
	UPROPERTY()
	TObjectPtr<APlayerController> DragController = nullptr;

	UPROPERTY()
	bool bDragging = false;

	UPROPERTY()
	float InitialDragDistance = 0.f;

	// Offset so the object can appear under the cursor even if its pivot is not centered, have not configured this yet.
	UPROPERTY(EditAnywhere, Category="Drag")
	bool bUseOffset = false;

	UPROPERTY(EditAnywhere, Category="Drag", meta=(EditCondition="bUseOffset"))
	FVector DragOffset = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, Category="Drag", meta=(ClampMin="0.0"))
	float DragSmoothSpeed = 15.f;

	// Clamp drag depth for safety / tuning
	UPROPERTY(EditAnywhere, Category="Drag", meta=(ClampMin="0.0"))
	float MinDragDistance = 50.f;

	UPROPERTY(EditAnywhere, Category="Drag", meta=(ClampMin="0.0"))
	float MaxDragDistance = 500.f;
};
