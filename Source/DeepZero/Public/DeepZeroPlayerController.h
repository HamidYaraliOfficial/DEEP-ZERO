#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DeepZeroPlayerController.generated.h"
UCLASS() class DEEPZERO_API ADeepZeroPlayerController:public APlayerController{GENERATED_BODY() public:ADeepZeroPlayerController();protected:virtual void SetupInputComponent()override;private:void Interact();void Ping();};
