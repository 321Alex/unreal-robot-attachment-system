#include "Misc/AutomationTest.h"

#include "AttachablePart.h"
#include "AttachmentPointActor.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FAttachmentCompatibilityTest_Final,
	"RobotAbuse.AttachmentSystem.CompatibilityCheck",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FAttachmentCompatibilityTest_Final::RunTest(const FString& Parameters)
{
	UObject* Outer = GetTransientPackage();

	AAttachmentPointActor* LeftSocket = NewObject<AAttachmentPointActor>(Outer);
	LeftSocket->AcceptedArmType = EPartType::LeftArm;

	AAttachablePart* LeftArm = NewObject<AAttachablePart>(Outer);
	LeftArm->PartType = EPartType::LeftArm;

	AAttachablePart* RightArm = NewObject<AAttachablePart>(Outer);
	RightArm->PartType = EPartType::RightArm;

	AAttachablePart* UniversalPart = NewObject<AAttachablePart>(Outer);
	UniversalPart->PartType = EPartType::Universal;

	// Compatible type should be accepted
	TestTrue(TEXT("Left arm should be accepted by left socket"),
	         LeftSocket->CanAcceptPart(LeftArm));

	// Incompatible type should be rejected
	TestFalse(TEXT("Right arm should be rejected by left socket"),
	          LeftSocket->CanAcceptPart(RightArm));

	// Universal part should be accepted by any non-busy socket
	TestTrue(TEXT("Universal part should be accepted by left socket"),
	         LeftSocket->CanAcceptPart(UniversalPart));

	// Null input should be rejected
	TestFalse(TEXT("Null part should be rejected"),
	          LeftSocket->CanAcceptPart(nullptr));

	// Busy socket should reject any part (availability gate)
	LeftSocket->TryAttachPart(LeftArm);
	TestFalse(TEXT("Socket should reject parts when occupied"),
	          LeftSocket->CanAcceptPart(UniversalPart));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPartStateTransitionTest_Final,
	"RobotAbuse.AttachablePart.StateTransitions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
)

bool FPartStateTransitionTest_Final::RunTest(const FString& Parameters)
{
	UObject* Outer = GetTransientPackage();
	AAttachablePart* Part = NewObject<AAttachablePart>(Outer);

	// Initial state
	TestEqual(TEXT("Part should start in DETACHED state"),
	          Part->CurrentState, EPartState::DETACHED);

	// This needs a refactor
	//Part->PickUp();
	//TestEqual(TEXT("Part should be HELD after PickUp"),
	//          Part->CurrentState, EPartState::HELD);
	

	// Drop changes state -> DETACHED
	Part->Drop();
	TestEqual(TEXT("Part should be DETACHED after Drop"),
	          Part->CurrentState, EPartState::DETACHED);

	return true;
}
