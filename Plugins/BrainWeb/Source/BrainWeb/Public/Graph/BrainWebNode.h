#pragma once

#include "CoreMinimal.h"
#include "SubclassOf.h"
#include "TextProperty.h"
#include "BrainWebNode.generated.h"

class UBrainWebGraph;
class UBrainWebEdge;

UCLASS(Blueprintable)
class BRAINWEB_API UBrainWebNode : public UObject
{
	GENERATED_BODY()

public:
	UBrainWebNode();
	virtual ~UBrainWebNode();

	UPROPERTY(VisibleDefaultsOnly, Category = "BrainWebNode")
	UBrainWebGraph* Graph;

	UPROPERTY(BlueprintReadOnly, Category = "BrainWebNode")
	TArray<UBrainWebNode*> ParentNodes;

	UPROPERTY(BlueprintReadOnly, Category = "BrainWebNode")
	TArray<UBrainWebNode*> ChildrenNodes;

	UPROPERTY(BlueprintReadOnly, Category = "BrainWebNode")
	TMap<UBrainWebNode*, UBrainWebEdge*> Edges;

	UFUNCTION(BlueprintCallable, Category = "BrainWebNode")
	virtual UBrainWebEdge* GetEdge(UBrainWebNode* ChildNode);

	UFUNCTION(BlueprintCallable, Category = "BrainWebNode")
	bool IsLeafNode() const;

	UFUNCTION(BlueprintCallable, Category = "BrainWebNode")
	UBrainWebGraph* GetGraph() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MissionNode")
	FText GetDescription() const;
	virtual FText GetDescription_Implementation() const;

	FText& GetDataText()
	{
		return NodeData;
	}

	//////////////////////////////////////////////////////////////////////////
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "BrainWebNode_Editor")
	FText NodeTitle;

	UPROPERTY(VisibleDefaultsOnly, Category = "BrainWebNode_Editor")
	TSubclassOf<UBrainWebGraph> CompatibleGraphType;

	UPROPERTY(EditDefaultsOnly, Category = "BrainWebNode_Editor")
	FLinearColor BackgroundColor;

	UPROPERTY(EditDefaultsOnly, Category = "BrainWebNode_Editor")
	FText ContextMenuName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BrainWebNode_Editor")
	FText NodeData;

#endif

#if WITH_EDITOR
	virtual FLinearColor GetBackgroundColor() const;

	virtual FText GetNodeTitle() const;

	virtual void SetNodeTitle(const FText& NewTitle);

	virtual bool CanCreateConnection(UBrainWebNode* Other, FText& ErrorMessage);
#endif
};
