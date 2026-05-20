#include "HighlightStrategy_Emissive.h"

#include "AttachablePart.h"
#include "HighlightComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Math/UnrealMathUtility.h"

// Emissive highlight strategy:
// - Setup() scans static mesh materials and replaces any material that supports the emissive scalar parameter
//   with a dynamic instance so we can drive highlight intensity at runtime.
// - Apply()/Clear() optionally propagate highlight state to descendant actors. We traverse both attached actors
//   and ChildActorComponent-spawned actors because they don't always appear in GetAttachedActors().

bool UHighlightStrategy_Emissive::MaterialHasScalarParam(UMaterialInterface* Mat, FName ParamName)
{
	if (!Mat) return false;

	TArray<FMaterialParameterInfo> Infos;
	TArray<FGuid> Ids;
	Mat->GetAllScalarParameterInfo(Infos, Ids);
	
	// Avoid creating dynamic instances for materials that cannot be driven by our emissive scalar parameter.
	for (const FMaterialParameterInfo& Info : Infos)
	{
		if (Info.Name == ParamName)
		{
			return true;
		}
	}

	return false;
}

bool UHighlightStrategy_Emissive::MaterialHasVectorParam(UMaterialInterface* Mat, FName ParamName)
{
	if (!Mat) return false;

	TArray<FMaterialParameterInfo> Infos;
	TArray<FGuid> Ids;
	Mat->GetAllVectorParameterInfo(Infos, Ids);

	for (const FMaterialParameterInfo& Info : Infos)
	{
		if (Info.Name == ParamName)
		{
			return true;
		}
	}

	return false;
}

void UHighlightStrategy_Emissive::Setup(AActor* Target)
{
	// Clear any previous references.
	DynamicMaterials.Empty();
	OriginalColors.Empty();
	bHasColorParameter.Empty();
	
	TArray<UStaticMeshComponent*> MeshComponents;
	Target->GetComponents<UStaticMeshComponent>(MeshComponents);

	for (UStaticMeshComponent* Mesh : MeshComponents)
	{
		if (!Mesh) continue;

		const int32 NumMaterials = Mesh->GetNumMaterials();
		for (int32 i = 0; i < NumMaterials; i++)
		{
			UMaterialInterface* BaseMat = Mesh->GetMaterial(i);
			if (!BaseMat) continue;

			const bool bHasScalar = MaterialHasScalarParam(BaseMat, EmissiveParameterName);
			const bool bHasVector = MaterialHasVectorParam(BaseMat, EmissiveColorParameterName);

			// Skip materials that don't have a highlight parameter we can drive.
			if (!bHasScalar && !bHasVector)
			{
				continue;
			}

			// Create a dynamic material instance so we can adjust emissive intensity at runtime.
			UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(BaseMat, Mesh);
			if (!DynMat) continue;

			Mesh->SetMaterial(i, DynMat);
			DynamicMaterials.Add(DynMat);
			OriginalColors.Add(bHasVector ? DynMat->K2_GetVectorParameterValue(EmissiveColorParameterName) : FLinearColor::White);
			bHasColorParameter.Add(bHasVector);
		}
	}
}

static void CollectHighlightDescendants_Internal(AActor* Actor, TArray<AActor*>& OutActors)
{
	if (!Actor) return;

	OutActors.AddUnique(Actor);

	// 1) Regular attached actors
	TArray<AActor*> AttachedActors;
	Actor->GetAttachedActors(AttachedActors);
	for (AActor* Child : AttachedActors)
	{
		CollectHighlightDescendants_Internal(Child, OutActors);
	}

	// 2) ChildActorComponent-spawned actors
	// ChildActorComponent can spawn an actor that isn't always returned by GetAttachedActors(),
	// so we explicitly traverse those as well.
	TArray<UChildActorComponent*> ChildActorComps;
	Actor->GetComponents<UChildActorComponent>(ChildActorComps);
	for (UChildActorComponent* CAC : ChildActorComps)
	{
		if (!CAC) continue;

		if (AActor* ChildActor = CAC->GetChildActor())
		{
			CollectHighlightDescendants_Internal(ChildActor, OutActors);
		}
	}
}

static void GetHighlightChildActors(AActor* Root, TArray<AActor*>& OutActors)
{
	OutActors.Reset();
	CollectHighlightDescendants_Internal(Root, OutActors);

	// Remove the root because this strategy already highlights the root actor directly.
	OutActors.RemoveSingleSwap(Root);
}

void UHighlightStrategy_Emissive::Apply(AActor* Target)
{
	Apply(Target, EHighlightVisualState::Hover);
}

void UHighlightStrategy_Emissive::Apply(AActor* Target, EHighlightVisualState State)
{
	const FLinearColor Color = GetColorForState(State);
	PulseTime = 0.0f;
	SetEmissive(GetCurrentHighlightValue(), &Color);

	if (!HighlightChildren) return;

	TArray<AActor*> Descendants;
	GetHighlightChildActors(Target, Descendants);

	for (AActor* Child : Descendants)
	{
		if (!Child) continue;
		
		// Only highlight parts that are actually attached (gameplay state)
		if (AAttachablePart* Part = Cast<AAttachablePart>(Child))
		{
			if (!Part->IsAttached())
				continue;
		}

		// Only toggle highlight on actors that participate in the highlight system.
		if (UHighlightComponent* Highlight = Child->FindComponentByClass<UHighlightComponent>())
		{
			Highlight->SetHighlightState(State);
		}
	}
}

void UHighlightStrategy_Emissive::Clear(AActor* Target)
{
	SetEmissive(ClearValue);
	RestoreOriginalColors();
	PulseTime = 0.0f;

	if (!HighlightChildren) return;

	TArray<AActor*> ChildActors;
	GetHighlightChildActors(Target, ChildActors);

	for (AActor* Child : ChildActors)
	{
		if (!Child) continue;
		
		if (AAttachablePart* Part = Cast<AAttachablePart>(Child))
		{
			if (!Part->IsAttached())
				continue;
		}

		if (UHighlightComponent* Highlight = Child->FindComponentByClass<UHighlightComponent>())
		{
			Highlight->SetHighlighted(false);
		}
	}
}

void UHighlightStrategy_Emissive::RestoreOriginalColors()
{
	for (int32 i = 0; i < DynamicMaterials.Num(); i++)
	{
		if (!bHasColorParameter.IsValidIndex(i) || !bHasColorParameter[i])
		{
			continue;
		}

		if (!OriginalColors.IsValidIndex(i))
		{
			continue;
		}

		if (UMaterialInstanceDynamic* Mat = DynamicMaterials[i])
		{
			Mat->SetVectorParameterValue(EmissiveColorParameterName, OriginalColors[i]);
		}
	}
}

void UHighlightStrategy_Emissive::TickHighlight(AActor* Target, EHighlightVisualState State, float DeltaTime)
{
	if (!bPulseHighlight)
	{
		return;
	}

	PulseTime += DeltaTime;
	SetEmissive(GetCurrentHighlightValue());
}

void UHighlightStrategy_Emissive::SetEmissive(float Value, const FLinearColor* Color)
{
	// Drive the emissive parameters on all cached dynamic material instances.
	for (int32 i = 0; i < DynamicMaterials.Num(); i++)
	{
		UMaterialInstanceDynamic* Mat = DynamicMaterials[i];
		if (!Mat)
		{
			continue;
		}

		Mat->SetScalarParameterValue(EmissiveParameterName, Value);
		if (Color && bHasColorParameter.IsValidIndex(i) && bHasColorParameter[i])
		{
			Mat->SetVectorParameterValue(EmissiveColorParameterName, *Color);
		}
	}
}

float UHighlightStrategy_Emissive::GetCurrentHighlightValue() const
{
	if (!bPulseHighlight)
	{
		return HighlightValue;
	}

	const float Alpha = (FMath::Sin(PulseTime * PulseSpeed) + 1.0f) * 0.5f;
	return FMath::Lerp(PulseMinValue, PulseMaxValue, Alpha);
}

FLinearColor UHighlightStrategy_Emissive::GetColorForState(EHighlightVisualState State) const
{
	switch (State)
	{
	case EHighlightVisualState::Selected:
		return SelectedColor;
	case EHighlightVisualState::Invalid:
		return InvalidColor;
	case EHighlightVisualState::Hover:
	default:
		return HoverColor;
	}
}
