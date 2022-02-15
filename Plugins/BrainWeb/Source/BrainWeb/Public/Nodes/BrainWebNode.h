#pragma once

#include "CoreMinimal.h"
#include "BrainWebNode.generated.h"

UENUM()
enum ENodeType
{
	Query,
	Message,
	Start,
	End,
};

class UBrainWebGraph;
class UBrainWebEdge;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExecute);

UCLASS(Blueprintable, Abstract)
class BRAINWEB_API UBrainWebNode : public UObject
{
	GENERATED_BODY()

public:
	UBrainWebNode();
	virtual ~UBrainWebNode();

	UFUNCTION()
	virtual void Execute();
	
	UPROPERTY(BlueprintReadOnly, Category = "BrainWebNode")
	UBrainWebGraph* Graph;

	UPROPERTY(BlueprintReadOnly, Category = "BrainWebNode")
	TArray<UBrainWebNode*> ParentNodes;

	UPROPERTY(BlueprintReadOnly, Category = "BrainWebNode")
	TArray<UBrainWebNode*> ChildrenNodes;

	UFUNCTION(BlueprintCallable)
	UBrainWebNode* GetParentNode(int32 Id);

	UFUNCTION(BlueprintCallable)
	virtual UBrainWebNode* GetChildNode(int32 Id);
	
	UFUNCTION(BlueprintCallable, Category = "BrainWebNode")
	bool IsLeafNode() const;

	UFUNCTION(BlueprintCallable, Category = "BrainWebNode")
	UBrainWebGraph* GetGraph() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MissionNode")
	FText GetDescription() const;
	virtual FText GetDescription_Implementation() const;

	UPROPERTY(BlueprintAssignable)
	FOnExecute OnExecute;
	
	//////////////////////////////////////////////////////////////////////////
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "BrainWebNode_Editor")
	FText NodeTitle;
	
	UPROPERTY(EditDefaultsOnly, Category = "BrainWebNode_Editor")
	FLinearColor BackgroundColor;
	
	UPROPERTY(BlueprintReadOnly, Category = "BrainWebNode_Editor")
	TSubclassOf<UBrainWebGraph> CompatibleGraphType;

	UPROPERTY(BlueprintReadOnly, Category = "BrainWebNode_Editor")
	FText ContextMenuName;
#endif

#if WITH_EDITOR
	virtual FLinearColor GetBackgroundColor() const;

	virtual FText GetNodeTitle() const;

	virtual void SetNodeTitle(const FText& NewTitle);

	virtual bool CanCreateConnection(UBrainWebNode* Other, FText& ErrorMessage);
#endif

	void SetSelectedOutput(int32 NewSelection){SelectedOutput = NewSelection; };
	int32 GetSelectedOutput() const {return SelectedOutput;};
	
	UPROPERTY()
	int32 SelectedOutput = -1;;
};
