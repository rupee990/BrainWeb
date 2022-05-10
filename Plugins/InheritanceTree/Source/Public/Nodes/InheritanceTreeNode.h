#pragma once

#include "CoreMinimal.h"
#include "InheritanceTreeNode.generated.h"

class UInheritanceTreeGraph;
class UInheritanceTreeEdge;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExecute);

UCLASS(Blueprintable)
class INHERITANCETREE_API UInheritanceTreeNode : public UObject
{
	GENERATED_BODY()

public:
	UInheritanceTreeNode();
	virtual ~UInheritanceTreeNode();

	UFUNCTION()
	virtual void Execute();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UClass* Class;
	
	UPROPERTY(BlueprintReadOnly, Category = "InheritanceTreeNode")
	UInheritanceTreeGraph* Graph;

	UPROPERTY(BlueprintReadOnly, Category = "InheritanceTreeNode")
	TArray<UInheritanceTreeNode*> ParentNodes;

	UPROPERTY(BlueprintReadOnly, Category = "InheritanceTreeNode")
	TArray<UInheritanceTreeNode*> ChildrenNodes;

	UFUNCTION(BlueprintCallable)
	UInheritanceTreeNode* GetParentNode(int32 Id);

	UFUNCTION(BlueprintCallable)
	virtual UInheritanceTreeNode* GetChildNode(int32 Id);
	
	UFUNCTION(BlueprintCallable, Category = "InheritanceTreeNode")
	bool IsLeafNode() const;

	UFUNCTION(BlueprintCallable, Category = "InheritanceTreeNode")
	UInheritanceTreeGraph* GetGraph() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MissionNode")
	FText GetDescription() const;
	virtual FText GetDescription_Implementation() const;

	UPROPERTY(BlueprintAssignable)
	FOnExecute OnExecute;

	FLinearColor NodeColor = FLinearColor::Blue;
	
	//////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
	virtual FLinearColor GetBackgroundColor() const;

	virtual FText GetNodeTitle() const;

	virtual void SetNodeTitle(const FText& NewTitle);

	virtual bool CanCreateConnection(UInheritanceTreeNode* Other, FText& ErrorMessage);
#endif

	void SetSelectedOutput(int32 NewSelection){SelectedOutput = NewSelection; };
	int32 GetSelectedOutput() const {return SelectedOutput;};
	
	UPROPERTY()
	int32 SelectedOutput = -1;;
};
