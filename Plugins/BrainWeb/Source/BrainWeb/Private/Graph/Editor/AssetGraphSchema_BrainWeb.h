#pragma once

#include "CoreMinimal.h"
#include "BrainWebGraph.h"
#include "BrainWebNode.h"
#include "BrainWebEdge.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraph/EdGraph.h"
#include "AssetGraphSchema_BrainWeb.generated.h"

class UEdNode_BrainWebNode;
class UEdNode_BrainWebEdge;
class UAutoLayoutStrategy;

/** Action to add a node to the graph */
USTRUCT()
struct FAssetSchemaAction_BrainWeb_NewNode : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

public:
	FAssetSchemaAction_BrainWeb_NewNode(): NodeTemplate(nullptr) {}

	FAssetSchemaAction_BrainWeb_NewNode(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping), NodeTemplate(nullptr) {}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	UEdNode_BrainWebNode* NodeTemplate;
};

USTRUCT()
struct FAssetSchemaAction_BrainWeb_NewEdge : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();

public:
	FAssetSchemaAction_BrainWeb_NewEdge(): NodeTemplate(nullptr){}

	FAssetSchemaAction_BrainWeb_NewEdge(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping), NodeTemplate(nullptr) {}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	UEdNode_BrainWebEdge* NodeTemplate;
};

UCLASS(MinimalAPI)
class UAssetGraphSchema_BrainWeb : public UEdGraphSchema
{
	GENERATED_BODY()

public:
	void GetBreakLinkToSubMenuActions(class FMenuBuilder& MenuBuilder, class UEdGraphPin* InGraphPin);

	virtual EGraphType GetGraphType(const UEdGraph* TestEdGraph) const override;

 	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;

 	virtual void GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, class FMenuBuilder* MenuBuilder, bool bIsDebugging) const override;

 	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;

	virtual bool CreateAutomaticConversionNodeAndConnections(UEdGraphPin* A, UEdGraphPin* B) const override;

	virtual class FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const override;

 	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;

 	virtual void BreakNodeLinks(UEdGraphNode& TargetNode) const override;

 	virtual void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const override;

	virtual void BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const override;

	virtual UEdGraphPin* DropPinOnNode(UEdGraphNode* InTargetNode, const FName& InSourcePinName, const FEdGraphPinType& InSourcePinType, EEdGraphPinDirection InSourcePinDirection) const override;

	virtual bool SupportsDropPinOnNode(UEdGraphNode* InTargetNode, const FEdGraphPinType& InSourcePinType, EEdGraphPinDirection InSourcePinDirection, FText& OutErrorMessage) const override;

	virtual bool IsCacheVisualizationOutOfDate(int32 InVisualizationCacheID) const override;

	virtual int32 GetCurrentVisualizationCacheID() const override;

	virtual void ForceVisualizationCacheClear() const override;

	virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;

private:
	static int32 CurrentCacheRefreshID;
};

