#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph_InheritanceTree.generated.h"

class UInheritanceTreeGraph;
class UInheritanceTreeNode;
class UEdNode_InheritanceTreeNode;

UCLASS()
class UEdGraph_InheritanceTree : public UEdGraph
{
	GENERATED_BODY()

public:
	UEdGraph_InheritanceTree();
	virtual ~UEdGraph_InheritanceTree();

	virtual void RebuildGenericGraph();

	UInheritanceTreeGraph* GetGenericGraph() const;

	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
	virtual void PostEditUndo() override;

	UPROPERTY(Transient)
	TMap<UObject*, UEdGraphNode*> NodeMap;

protected:
	void Clear();

	void SortNodes(UInheritanceTreeNode* RootNode);
};
