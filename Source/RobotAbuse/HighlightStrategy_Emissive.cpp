#include "HighlightStrategy_Emissive.h"

#include "HighlightComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

// Emissive highlight strategy:
// - Setup() scans static mesh materials and replaces any material that supports the emissive scalar parameter
//   with a dynamic instance so we can drive highlight intensity at runtime.
// - Apply()/Clear() optionally propagate highlight state to descendant actors. We traverse both attached actors
//   and ChildActorComponent-spawned actors because they don't always appear in GetAttachedActors().

bool UHighlightStrategy_Emissive::MaterialHasScalarParam(UMaterialInterface* Mat, FName ParamName)
{
	// Avoid creating dynamic instances for materials that cannot be driven by our emissive scalar parameter.
	if (!Mat) return false;

	TArray<FMaterialParameterInfo> Infos;
	TArray<FGuid> Ids;
	Mat->GetAllScalarParameterInfo(Infos, Ids);

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
	// Clear any previous references. In this demo we don't restore original materials;
	// a production system may cache and restore originals on teardown.
	DynamicMaterials.Empty();
	
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

			// Skip materials that don't have the emissive scalar parameter.
			if (!MaterialHasScalarParam(BaseMat, EmissiveParameterName))
			{
				continue;
			}

			// Create a dynamic material instance so we can adjust emissive intensity at runtime.
			// Using the mesh as the Outer ties the instance lifetime to the component.
			UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(BaseMat, Mesh);
			if (!DynMat) continue;

			Mesh->SetMaterial(i, DynMat);
			DynamicMaterials.Add(DynMat);
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
	if (!Root) return;

	CollectHighlightDescendants_Internal(Root, OutActors);

	// Remove the root because this strategy already highlights the root actor directly.
	OutActors.RemoveSingleSwap(Root);
}

void UHighlightStrategy_Emissive::Apply(AActor* Target)
{
	SetEmissive(HighlightValue);

	if (!HighlightChildren) return;

	TArray<AActor*> Descendants;
	GetHighlightChildActors(Target, Descendants);

	for (AActor* Child : Descendants)
	{
		if (!Child) continue;

		// Only toggle highlight on actors that participate in the highlight system.
		if (UHighlightComponent* Highlight = Child->FindComponentByClass<UHighlightComponent>())
		{
			Highlight->SetHighlighted(true);
		}
	}
}

void UHighlightStrategy_Emissive::Clear(AActor* Target)
{
	SetEmissive(ClearValue);

	if (!HighlightChildren) return;

	TArray<AActor*> ChildActors;
	GetHighlightChildActors(Target, ChildActors);

	for (AActor* Child : ChildActors)
	{
		if (!Child) continue;

		if (UHighlightComponent* Highlight = Child->FindComponentByClass<UHighlightComponent>())
		{
			Highlight->SetHighlighted(false);
		}
	}
}

void UHighlightStrategy_Emissive::SetEmissive(float Value)
{
	// Drive the emissive scalar parameter on all cached dynamic material instances.
	for (UMaterialInstanceDynamic* Mat : DynamicMaterials)
	{
		if (!Mat) continue;
		Mat->SetScalarParameterValue(EmissiveParameterName, Value);
	}
}