#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "PartTypes.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPartTypeCompatibilityTest,
	"RobotAbuse.AttachmentSystem.PartTypeCompatibility",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::SmokeFilter
)

bool FPartTypeCompatibilityTest::RunTest(const FString& Parameters)
{
	AddInfo(TEXT("Checking direct part/socket compatibility."));
	TestTrue(TEXT("Left arm should be accepted by left socket"),
	         ArePartTypesCompatible(EPartType::LeftArm, EPartType::LeftArm));

	AddInfo(TEXT("Checking mismatched part/socket rejection."));
	TestFalse(TEXT("Right arm should be rejected by left socket"),
	          ArePartTypesCompatible(EPartType::LeftArm, EPartType::RightArm));

	AddInfo(TEXT("Checking universal part compatibility."));
	TestTrue(TEXT("Universal part should be accepted by left socket"),
	         ArePartTypesCompatible(EPartType::LeftArm, EPartType::Universal));

	AddInfo(TEXT("Checking universal socket compatibility."));
	TestTrue(TEXT("Universal socket should accept right leg"),
	         ArePartTypesCompatible(EPartType::Universal, EPartType::RightLeg));

	return true;
}

#endif
