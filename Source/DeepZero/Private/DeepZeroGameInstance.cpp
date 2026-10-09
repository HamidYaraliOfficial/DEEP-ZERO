#include "DeepZeroGameInstance.h"
#include "DZCoreSimulation.h"
#include "Misc/Guid.h"
void UDeepZeroGameInstance::Init(){Super::Init();StartNewExpedition();if(UDZScheduleSubsystem*S=GetSubsystem<UDZScheduleSubsystem>()){S->AddWindow({TEXT("ROV Operations"),420,90,true});S->AddWindow({TEXT("Crew Shift A"),510,240,true});S->AddWindow({TEXT("Laboratory"),780,360,true});S->AddWindow({TEXT("Night Operations"),1260,120,true});}}
void UDeepZeroGameInstance::StartNewExpedition(){ExpeditionId=FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens);Day=1;if(UDZMissionSubsystem*M=GetSubsystem<UDZMissionSubsystem>())M->Generate(1000,1,.1f);}
