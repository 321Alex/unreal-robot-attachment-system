#include "RobotSpectatorPawn.h"
#include "Components/InputComponent.h"
#include "RobotInteractionComponent.h"

ARobotSpectatorPawn::ARobotSpectatorPawn()
{
    PrimaryActorTick.bCanEverTick = false;
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
