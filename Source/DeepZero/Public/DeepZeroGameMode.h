#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DeepZeroGameMode.generated.h"
UCLASS() class DEEPZERO_API ADeepZeroGameMode:public AGameModeBase{GENERATED_BODY() public:ADeepZeroGameMode();virtual void StartPlay()override;};
