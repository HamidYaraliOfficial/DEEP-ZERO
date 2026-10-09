#include "DeepZeroGameMode.h"
#include "DZSubmarine.h"
#include "DZCoreSimulation.h"
ADeepZeroGameMode::ADeepZeroGameMode(){DefaultPawnClass=ADZSubmarine::StaticClass();}
void ADeepZeroGameMode::StartPlay(){Super::StartPlay();if(UDZHorrorDirectorSubsystem*D=GetWorld()->GetSubsystem<UDZHorrorDirectorSubsystem>())D->SetContext(.75f,.9f,.2f,.1f,0);if(UDZMissionSubsystem*M=GetGameInstance()->GetSubsystem<UDZMissionSubsystem>())M->Generate(1000,1,.1f);}
