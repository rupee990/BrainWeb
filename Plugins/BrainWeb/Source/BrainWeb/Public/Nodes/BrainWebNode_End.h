#pragma once

#include "CoreMinimal.h"

#include "BrainWebNode.h"
#include "BrainWebNode_End.generated.h"

class UBrainWebGraph;
class UBrainWebEdge;

UCLASS(Blueprintable)
class BRAINWEB_API UBrainWebNode_End : public UBrainWebNode
{
	GENERATED_BODY()

public:
	UBrainWebNode_End();
	virtual ~UBrainWebNode_End();

	virtual void Execute() override;
};
