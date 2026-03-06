#include "RobotSpectatorPawn.h"

#include "RobotInteractionComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerController.h"

void ARobotSpectatorPawn::BeginPlay()
{
    Super::BeginPlay();
    
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->bShowMouseCursor = true;
        PC->bEnableClickEvents = true;
        PC->bEnableMouseOverEvents = true;
    }
}

void ARobotSpectatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);

    Interaction = FindComponentByClass<URobotInteractionComponent>();
    if (!ensureMsgf(Interaction, TEXT("RobotSpectatorPawn requires a RobotInteractionComponent (add it in the Blueprint).")))
    {
        return;
    }

    PlayerInputComponent->BindAction(
        "MouseClick",
        IE_Pressed,
        Interaction.Get(),
        &URobotInteractionComponent::OnMouseClick
    );
}

