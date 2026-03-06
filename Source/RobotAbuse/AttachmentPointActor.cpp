#include "AttachmentPointActor.h"

#include "AttachablePart.h"

#include "Components/ChildActorComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

AAttachmentPointActor::AAttachmentPointActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root scene component for a clean component hierarchy in Blueprint.
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// Collision used for mouse traces / interaction. We keep it QueryOnly.
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetupAttachment(Root);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // mouse trace channel
	Collision->SetGenerateOverlapEvents(false);

	// Visual indicator for "available" attachment point.
	Visual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visual"));
	Visual->SetupAttachment(Root);
	Visual->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// SnapPoint is the parent for any attached part, so we can offset/rotate sockets in BP.
	SnapPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SnapPoint"));
	SnapPoint->SetupAttachment(Root);

	// Optional: blueprint-configured initial part that starts attached at BeginPlay.
	InitialPartChild = CreateDefaultSubobject<UChildActorComponent>(TEXT("InitialPartChild"));
	InitialPartChild->SetupAttachment(SnapPoint);
}

void AAttachmentPointActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Keep the editor preview accurate: show/hide the marker based on availability.
	RefreshAvailabilityVisuals();
}

void AAttachmentPointActor::BeginPlay()
{
	Super::BeginPlay();

	// If a blueprint assigned an initial part, register it as attached at startup.
	RegisterInitialPartIfAny();

	RefreshAvailabilityVisuals();
}

void AAttachmentPointActor::RegisterInitialPartIfAny()
{
	if (!InitialPartChild)
	{
		return;
	}

	// ChildActorComponent may exist but not have an actor assigned/spawned.
	AActor* ChildActor = InitialPartChild->GetChildActor();
	if (!ChildActor)
	{
		return;
	}

	AAttachablePart* Part = Cast<AAttachablePart>(ChildActor);
	if (!Part)
	{
		// This is a configuration error: the child actor is not an AAttachablePart.
		UE_LOG(LogTemp, Error, TEXT("AttachmentPointActor '%s': InitialPartChild is not an AAttachablePart."),
		       *GetName());
		return;
	}

	// If we're already occupied, don't auto-attach a second part.
	if (AttachedPart)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttachmentPointActor '%s': Already has an attached part; ignoring initial part."),
		       *GetName());
		return;
	}

	AttachedPart = Part;

	// Snap the part to this point. We keep scale from the part (not the socket).
	Part->AttachToComponent(SnapPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Part->SetActorRelativeTransform(FTransform::Identity);

	// Let the part update its own bookkeeping.
	Part->OnAttachToPoint(this);
}

bool AAttachmentPointActor::CanAcceptPart(AAttachablePart* Part) const
{
	if (!IsAvailable())
	{
		return false;
	}

	// Universal parts fit anywhere; universal sockets accept anything.
	if (AcceptedArmType == EPartType::Universal || Part->PartType == EPartType::Universal)
	{
		return true;
	}

	return AcceptedArmType == Part->PartType;
}

bool AAttachmentPointActor::TryAttachPart(AAttachablePart* Part)
{
	if (!CanAcceptPart(Part))
	{
		return false;
	}

	AttachedPart = Part;

	Part->AttachToComponent(SnapPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Part->SetActorRelativeTransform(FTransform::Identity);

	Part->OnAttachToPoint(this);

	RefreshAvailabilityVisuals();
	return true;
}

bool AAttachmentPointActor::DetachPart(AAttachablePart* Part)
{
	if (!AttachedPart || AttachedPart != Part)
	{
		return false;
	}

	// Detach the actor, then clear the reference.
	AttachedPart->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	AttachedPart = nullptr;

	RefreshAvailabilityVisuals();
	return true;
}

void AAttachmentPointActor::RefreshAvailabilityVisuals()
{
	const bool bAvailable = IsAvailable();

	// Marker is visible only when empty.
	if (Visual)
	{
		Visual->SetHiddenInGame(!bAvailable, true);
		Visual->SetVisibility(bAvailable, true);
	}

	// Only clickable when empty (prevents interacting with an occupied socket).
	if (Collision)
	{
		Collision->SetCollisionEnabled(bAvailable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	}
}

