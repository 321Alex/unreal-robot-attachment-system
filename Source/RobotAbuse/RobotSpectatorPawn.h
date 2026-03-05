#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "RobotSpectatorPawn.generated.h"

class URobotInteractionComponent;

UCLASS()
class ROBOTABUSE_API ARobotSpectatorPawn : public ASpectatorPawn
{
    GENERATED_BODY()
    
    UPROPERTY(Transient)
    TObjectPtr<URobotInteractionComponent> Interaction = nullptr;

public:
    ARobotSpectatorPawn();
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};