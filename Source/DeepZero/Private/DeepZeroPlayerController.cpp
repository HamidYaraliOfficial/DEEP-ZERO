#include "DeepZeroPlayerController.h"
#include "DZSubmarine.h"
#include "DZCoreSimulation.h"
ADeepZeroPlayerController::ADeepZeroPlayerController(){bShowMouseCursor=false;}
void ADeepZeroPlayerController::SetupInputComponent(){Super::SetupInputComponent();InputComponent->BindAction(TEXT("Interact"),IE_Pressed,this,&ADeepZeroPlayerController::Interact);InputComponent->BindAction(TEXT("SonarPing"),IE_Pressed,this,&ADeepZeroPlayerController::Ping);}
void ADeepZeroPlayerController::Interact(){if(GetPawn()&&GEngine)GEngine->AddOnScreenDebugMessage(-1,1.5f,FColor::Cyan,TEXT("Interaction ready: bind a Blueprint console or native IInteractable actor."));}
void ADeepZeroPlayerController::Ping(){if(ADZSubmarine*S=Cast<ADZSubmarine>(GetPawn()))if(S->Sonar)S->Sonar->Ping();}
