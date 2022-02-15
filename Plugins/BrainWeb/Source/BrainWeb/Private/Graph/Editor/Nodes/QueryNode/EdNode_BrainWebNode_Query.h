#pragma once

#include "CoreMinimal.h"
#include "Graph/Editor/Nodes/BaseNode/EdNode_BrainWebNode.h"
#include "EdNode_BrainWebNode_Query.generated.h"

class UEdNode_BrainWebEdge;
class UEdGraph_BrainWeb;
class SEdNode_BrainWebNode;

UCLASS(MinimalAPI)
class UEdNode_BrainWebNode_Query : public UEdNode_BrainWebNode
{

	GENERATED_BODY()

public:
	UEdNode_BrainWebNode_Query();
	virtual ~UEdNode_BrainWebNode_Query();

	virtual void SetGenericGraphNode(UBrainWebNode* InNode) override;

	virtual void AllocateDefaultPins() override;
	
	UFUNCTION()
	virtual void GeneratePins() override;
	
	UEdGraphPin* GetOutputPinFromName(const FName& PinName);
	
	virtual void PostInitProperties() override;
};


