#include "RobotInteractionComponent.h"

#include "AttachablePart.h"
#include "AttachmentPointActor.h"
#include "DragComponent.h"
#include "HighlightComponent.h"
#include "IInteractable.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

URobotInteractionComponent::URobotInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void URobotInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	CachedPC = Cast<APlayerController>(OwnerPawn->GetController());

	// Mouse settings for selecting/interacting.
	CachedPC->bShowMouseCursor = true;
	CachedPC->bEnableClickEvents = true;
	CachedPC->bEnableMouseOverEvents = true;

	CreateArmStatusWidget();

	// Use a timer instead of Tick for hover checks to reduce per-frame work.
	StartHighlightTimer();
}

void URobotInteractionComponent::OnMouseClick()
{
	FHitResult Hit;
	CachedPC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	// If already dragging something, clicking will either attach to a socket or drop.
	if (DraggedActor)
	{
		HandleDropOrAttach(Hit);
		return;
	}

	// Not dragging -> try to interact/pick up whatever we clicked.
	if (AActor* HitActor = Hit.GetActor())
	{
		HandleNewClick(HitActor);
	}
}

void URobotInteractionComponent::HandleNewClick(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	// Only interact with interactable actors.
	if (!Actor->Implements<UInteractable>())
	{
		return;
	}

	// Stop hover highlighting while we process an interaction.
	StopHighlightTimer();
	SetCurrentTarget(nullptr);

	// Allow the interactable to update its own state first.
	IInteractable::Execute_OnInteract(Actor);

	// Start dragging if the actor has a drag component.
	if (UDragComponent* DragComp = Actor->FindComponentByClass<UDragComponent>())
	{
		DraggedActor = Actor;

		DragComp->BeginDrag(CachedPC);

		SetHighlightIfPresent(Actor, true);

		// UI event reporting the state of the targeted part.
		if (AAttachablePart* Part = Cast<AAttachablePart>(Actor))
		{
			OnPartStateChanged.Broadcast(Part);
		}

		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("%s is interactable but has no DragComponent"), *GetNameSafe(Actor));

	// Resume hover highlights since we didn't start dragging.
	StartHighlightTimer();
}

void URobotInteractionComponent::HandleDropOrAttach(const FHitResult& Hit)
{
	if (!DraggedActor)
	{
		return;
	}

	AAttachablePart* DraggedPart = Cast<AAttachablePart>(DraggedActor);
	AAttachmentPointActor* Point = Cast<AAttachmentPointActor>(Hit.GetActor());

	// If we clicked on an attachment point while dragging a part, try to attach.
	if (Point && DraggedPart)
	{
		if (Point->TryAttachPart(DraggedPart))
		{
			// End drag session on the part.
			if (UDragComponent* DragComp = DraggedActor->FindComponentByClass<UDragComponent>())
			{
				DragComp->EndDrag();
			}
			else
			{
				ensureMsgf(false, TEXT("DraggedActor %s lost DragComponent during drag"), *GetNameSafe(DraggedActor));
			}

			SetHighlightIfPresent(DraggedActor, false);

			// Update UI
			OnPartStateChanged.Broadcast(DraggedPart);

			DraggedActor = nullptr;

			// Resume hover highlights
			StartHighlightTimer();
			return;
		}

		// Attach failed (wrong type, occupied, etc.) -> keep dragging.
		return;
	}

	// Clicked empty space (or non-socket target) -> drop.
	EndDragAndResumeHover(true);

	if (DraggedPart)
	{
		DraggedPart->Drop();
		OnPartStateChanged.Broadcast(DraggedPart);
	}
}

void URobotInteractionComponent::EndDragAndResumeHover(bool bClearDraggedHighlight)
{
	if (!DraggedActor)
	{
		return;
	}

	if (UDragComponent* DragComp = DraggedActor->FindComponentByClass<UDragComponent>())
	{
		DragComp->EndDrag();
	}
	else
	{
		ensureMsgf(false, TEXT("DraggedActor %s lost DragComponent during drag"), *GetNameSafe(DraggedActor));
	}

	if (bClearDraggedHighlight)
	{
		SetHighlightIfPresent(DraggedActor, false);
	}

	DraggedActor = nullptr;

	StartHighlightTimer();
}

// ===== Hover highlight timer =====

void URobotInteractionComponent::StartHighlightTimer()
{
	if (!GetWorld())
	{
		return;
	}

	FTimerManager& TM = GetWorld()->GetTimerManager();

	// Avoid activating if already running.
	if (TM.IsTimerActive(HighlightTimerHandle))
	{
		return;
	}

	TM.SetTimer(
		HighlightTimerHandle,
		this,
		&URobotInteractionComponent::UpdateCurrentTarget,
		HoverPollInterval,
		true
	);
}

void URobotInteractionComponent::StopHighlightTimer()
{
	if (!GetWorld())
	{
		return;
	}

	FTimerManager& TM = GetWorld()->GetTimerManager();

	if (!TM.IsTimerActive(HighlightTimerHandle))
	{
		return;
	}

	TM.ClearTimer(HighlightTimerHandle);

	// Clear any current highlight when stopping.
	if (CurrentTarget)
	{
		SetHighlightIfPresent(CurrentTarget, false);
	}

	CurrentTarget = nullptr;
}

void URobotInteractionComponent::SetCurrentTarget(AActor* NewTarget)
{
	if (NewTarget == CurrentTarget)
	{
		return;
	}

	// Turn off the highlight on the old target.
	SetHighlightIfPresent(CurrentTarget, false);

	CurrentTarget = NewTarget;

	// Turn on the highlight on the new target.
	SetHighlightIfPresent(CurrentTarget, true);

	// Update UI
	if (AAttachablePart* Part = Cast<AAttachablePart>(CurrentTarget))
	{
		OnPartStateChanged.Broadcast(Part);
	}
	else
	{
		OnPartStateChanged.Broadcast(nullptr);
	}
}

void URobotInteractionComponent::UpdateCurrentTarget()
{
	if (!CachedPC || DraggedActor)
	{
		return;
	}

	FHitResult Hit;
	CachedPC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	AActor* HitActor = Hit.GetActor();

	// Only hover things that can be interacted with.
	AActor* NewTarget = (HitActor && HitActor->Implements<UInteractable>()) ? HitActor : nullptr;

	SetCurrentTarget(NewTarget);
}

void URobotInteractionComponent::SetHighlightIfPresent(AActor* Actor, bool bOn)
{
	if (!Actor)
	{
		return;
	}

	if (UHighlightComponent* Highlight = Actor->FindComponentByClass<UHighlightComponent>())
	{
		Highlight->SetHighlighted(bOn);
	}
}

// ===== UI =====

void URobotInteractionComponent::CreateArmStatusWidget()
{
	if (!ArmStatusWidgetClass)
	{
		return;
	}

	UUserWidget* Widget = CreateWidget<UUserWidget>(CachedPC, ArmStatusWidgetClass);
	if (ensure(Widget))
	{
		Widget->AddToViewport();
	}
}