#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "Nodes/InheritanceTreeNode.h"
#include "EdNode_InheritanceTreeNode.generated.h"

class UEdNode_InheritanceTreeEdge;
class UEdGraph_InheritanceTree;
class SEdNode_InheritanceTreeNode;

UCLASS()
class UEdNode_InheritanceTreeNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UEdNode_InheritanceTreeNode();
	virtual ~UEdNode_InheritanceTreeNode();

	UPROPERTY(BlueprintReadOnly, Instanced, Category = "InheritanceTree")
	UInheritanceTreeNode* InheritanceTreeNode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ClassInfo)
	TSubclassOf<UObject> Class;

	virtual void SetGenericGraphNode(UInheritanceTreeNode* InNode);
	UEdGraph_InheritanceTree* GetGenericGraphEdGraph();

	SEdNode_InheritanceTreeNode* SEdNode;

	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PrepareForCopying() override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;

	virtual void GeneratePins();

	virtual FLinearColor GetBackgroundColor() const;
	virtual UEdGraphPin* GetInputPin() const;
	virtual UEdGraphPin* GetOutputPin() const;

#if WITH_EDITOR
	virtual void PostEditUndo() override;
#endif

};
