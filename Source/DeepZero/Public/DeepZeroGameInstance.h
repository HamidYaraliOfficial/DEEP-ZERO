#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DeepZeroGameInstance.generated.h"
UCLASS() class DEEPZERO_API UDeepZeroGameInstance:public UGameInstance{GENERATED_BODY() public:virtual void Init()override;UFUNCTION(BlueprintCallable)void StartNewExpedition();UPROPERTY(BlueprintReadOnly)FString ExpeditionId;UPROPERTY(BlueprintReadOnly)int32 Day=1;};
