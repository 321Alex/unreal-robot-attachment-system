#pragma once

#include "CoreMinimal.h"
#include "HighlightStrategy.h"
#include "HighlightStrategy_Emissive.generated.h"

class UMaterialInstanceDynamic;
class UMeshComponent;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class ROBOTABUSE_API UHighlightStrategy_Emissive : public UHighlightStrategy
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Highlight")
	FName EmissiveParameterName = "EmissiveStrength";

	UPROPERTY(EditAnywhere, Category="Highlight")
	FName EmissiveColorParameterName = "EmissiveColor";

	UPROPERTY(EditAnywhere, Category="Highlight")
	float HighlightValue = 5.0f;

	UPROPERTY(EditAnywhere, Category="Highlight")
	FLinearColor HoverColor = FLinearColor(0.0f, 0.35f, 1.0f);

	UPROPERTY(EditAnywhere, Category="Highlight")
	FLinearColor SelectedColor = FLinearColor(1.0f, 0.85f, 0.0f);

	UPROPERTY(EditAnywhere, Category="Highlight")
	FLinearColor InvalidColor = FLinearColor(1.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category="Highlight")
	float ClearValue = 0.0f;

	UPROPERTY(EditAnywhere, Category="Pulse")
	bool bPulseHighlight = true;

	UPROPERTY(EditAnywhere, Category="Pulse", meta=(EditCondition="bPulseHighlight", ClampMin="0.0"))
	float PulseMinValue = 1.0f;

	UPROPERTY(EditAnywhere, Category="Pulse", meta=(EditCondition="bPulseHighlight", ClampMin="0.0"))
	float PulseMaxValue = 3.0f;

	UPROPERTY(EditAnywhere, Category="Pulse", meta=(EditCondition="bPulseHighlight", ClampMin="0.0"))
	float PulseSpeed = 3.5f;
	
	UPROPERTY(EditAnywhere, Category="Highlight")
	bool HighlightChildren = false;

	// Optional: specify meshes explicitly. If empty, Setup() will gather all UMeshComponent on the actor.
	UPROPERTY(EditAnywhere, Category="Highlight")
	TArray<TObjectPtr<UMeshComponent>> TargetMeshes;

	virtual void Setup(AActor* Target) override;
	virtual void Apply(AActor* Target) override;
	virtual void Apply(AActor* Target, EHighlightVisualState State) override;
	virtual void TickHighlight(AActor* Target, EHighlightVisualState State, float DeltaTime) override;
	virtual void Clear(AActor* Target) override;

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UMaterialInstanceDynamic>> DynamicMaterials;

	TArray<FLinearColor> OriginalColors;
	TArray<bool> bHasColorParameter;

	float PulseTime = 0.0f;

	void SetEmissive(float Value, const FLinearColor* Color = nullptr);
	void RestoreOriginalColors();
	float GetCurrentHighlightValue() const;
	FLinearColor GetColorForState(EHighlightVisualState State) const;

	static bool MaterialHasScalarParam(UMaterialInterface* Mat, FName ParamName);
	static bool MaterialHasVectorParam(UMaterialInterface* Mat, FName ParamName);
};
