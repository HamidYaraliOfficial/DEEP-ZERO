#include "Misc/AutomationTest.h"
#include "DZCoreSimulation.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDZScheduleCrossMidnight,"DeepZero.Schedule.CrossMidnight",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FDZScheduleCrossMidnight::RunTest(const FString&){UDZScheduleSubsystem*S=NewObject<UDZScheduleSubsystem>();S->AddWindow({TEXT("Night"),1380,120,true});TestTrue(TEXT("Open before midnight"),S->Query(1410).Open);TestTrue(TEXT("Open after midnight"),S->Query(30).Open);return true;}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDZOceanPressure,"DeepZero.Ocean.Pressure",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FDZOceanPressure::RunTest(const FString&){UDZOceanSimulationComponent*O=NewObject<UDZOceanSimulationComponent>();const auto A=O->SampleAtDepth(0);const auto B=O->SampleAtDepth(5000);TestTrue(TEXT("Pressure rises"),B.PressureKPa>A.PressureKPa);return true;}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDZSaveMigration,"DeepZero.Save.Migration",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FDZSaveMigration::RunTest(const FString&){UDZSaveGame*S=NewObject<UDZSaveGame>();S->Version=1;S->DesiredDepth=0;TestTrue(TEXT("Migration works"),S->Migrate());TestEqual(TEXT("Version current"),S->Version,UDZSaveGame::CurrentVersion);return true;}
