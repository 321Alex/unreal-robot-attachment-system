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
	float HighlightValue = 5.0f;

	UPROPERTY(EditAnywhere, Category="Highlight")
	float ClearValue = 0.0f;
	
	UPROPERTY(EditAnywhere, Category="Highlight")
	bool HighlightChildren = false;

	// Optional: specify meshes explicitly. If empty, Setup() will gather all UMeshComponent on the actor.
	UPROPERTY(EditAnywhere, Category="Highlight")
	TArray<TObjectPtr<UMeshComponent>> TargetMeshes;

	virtual void Setup(AActor* Target) override;
	virtual void Apply(AActor* Target) override;
	virtual void Clear(AActor* Target) override;

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UMaterialInstanceDynamic>> DynamicMaterials;

	void SetEmissive(float Value);

	static bool MaterialHasScalarParam(UMaterialInterface* Mat, FName ParamName);
};
