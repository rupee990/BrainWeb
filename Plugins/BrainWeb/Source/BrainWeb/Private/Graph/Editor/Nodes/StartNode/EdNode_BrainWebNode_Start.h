#pragma once

#include "CoreMinimal.h"
#include "Graph/Editor/Nodes/BaseNode/EdNode_BrainWebNode.h"
#include "EdNode_BrainWebNode_Start.generated.h"

class UEdNode_BrainWebEdge;
class UEdGraph_BrainWeb;
class SEdNode_BrainWebNode;

UCLASS(MinimalAPI)
class UEdNode_BrainWebNode_Start : public UEdNode_BrainWebNode
{
	GENERATED_BODY()

public:
	UEdNode_BrainWebNode_Start();
	virtual ~UEdNode_BrainWebNode_Start();

	virtual void AllocateDefaultPins() override;
	
	virtual UEdGraphPin* GetInputPin() const override;
	virtual UEdGraphPin* GetOutputPin() const override;
};
