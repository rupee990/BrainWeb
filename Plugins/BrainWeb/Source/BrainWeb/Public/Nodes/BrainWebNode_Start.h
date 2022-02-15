#pragma once

#include "CoreMinimal.h"

#include "BrainWebNode.h"
#include "BrainWebNode_Start.generated.h"

class UBrainWebGraph;
class UBrainWebEdge;

UCLASS(Blueprintable)
class BRAINWEB_API UBrainWebNode_Start : public UBrainWebNode
{
	GENERATED_BODY()

public:
	UBrainWebNode_Start();
	virtual ~UBrainWebNode_Start();

	virtual void Execute() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpeechIndex;
};
