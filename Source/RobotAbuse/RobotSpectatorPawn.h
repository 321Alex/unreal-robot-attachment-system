#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "RobotSpectatorPawn.generated.h"

class URobotInteractionComponent;
class UInputComponent;

UCLASS()
class ROBOTABUSE_API ARobotSpectatorPawn : public ASpectatorPawn
{
    GENERATED_BODY()

public:
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(Transient)
    TObjectPtr<URobotInteractionComponent> Interaction = nullptr;
};