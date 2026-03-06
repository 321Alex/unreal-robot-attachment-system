#include "RobotInteractionComponent.h"

#include "AttachablePart.h"
#include "AttachmentPointActor.h"
#include "DragComponent.h"
#include "HighlightComponent.h"
#include "IInteractable.h"

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
	CachedPC = OwnerPawn ? Cast<APlayerController>(OwnerPawn->GetController()) : nullptr;

	StartHighlightTimer();
}

void URobotInteractionComponent::OnMouseClick()
{
	if (!CachedPC)
	{
		return;
	}

	FHitResult Hit;
	CachedPC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	// If already dragging something, clicking will either attach to a socket or drop.
	if (DraggedActor)
	{
		HandleDropOrAttach(Hit);
		return;
	}

	// Not dragging -> tell the clicked actor it was interacted with.
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

	if (!Actor->Implements<UInteractable>())
	{
		return;
	}

	// Stop hover highlighting while we process an interaction.
	StopHighlightTimer();
	SetCurrentTarget(nullptr);

	// Let the clicked actor decide what interaction means.
	IInteractable::Execute_OnInteract(Actor, GetOwner());

	// If interaction did not start a drag session, resume hover highlighting.
	if (!DraggedActor)
	{
		StartHighlightTimer();
	}
}

void URobotInteractionComponent::BeginDraggingActor(AActor* Actor)
{
	if (!Actor || DraggedActor || !CachedPC)
	{
		return;
	}

	UDragComponent* DragComp = Actor->FindComponentByClass<UDragComponent>();
	if (!DragComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s requested dragging but has no DragComponent"), *GetNameSafe(Actor));
		return;
	}

	DraggedActor = Actor;

	DragComp->BeginDrag(CachedPC);

	SetHighlightIfPresent(Actor, true);

	if (AAttachablePart* Part = Cast<AAttachablePart>(Actor))
	{
		OnPartStateChanged.Broadcast(Part);
	}
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

			OnPartStateChanged.Broadcast(DraggedPart);

			DraggedActor = nullptr;

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

void URobotInteractionComponent::StartHighlightTimer()
{
	if (!GetWorld())
	{
		return;
	}

	FTimerManager& TM = GetWorld()->GetTimerManager();

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
	FTimerManager& TM = GetWorld()->GetTimerManager();

	if (!TM.IsTimerActive(HighlightTimerHandle))
	{
		return;
	}

	TM.ClearTimer(HighlightTimerHandle);

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

	SetHighlightIfPresent(CurrentTarget, false);

	CurrentTarget = NewTarget;

	SetHighlightIfPresent(CurrentTarget, true);

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