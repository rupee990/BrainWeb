#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

class UEdNode_InheritanceTreeNode;
class SEditableTextBlock;
class SMultiLineEditableTextBox;

class SEdNode_InheritanceTreeNode : public SGraphNode, public FNotifyHook
{
public:
	SLATE_BEGIN_ARGS(SEdNode_InheritanceTreeNode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdNode_InheritanceTreeNode* InNode);

	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual bool IsNameReadOnly() const override;

	virtual void CreateDetailsView();
	
	virtual void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);

	virtual void OnDataCommited(const FText& InText, ETextCommit::Type CommitInfo);

	virtual FSlateColor GetBorderBackgroundColor() const;
	virtual FSlateColor GetBackgroundColor() const;

	virtual EVisibility GetDragOverMarkerVisibility() const;

	virtual const FSlateBrush* GetNameIcon() const;

protected:
	TSharedPtr<SVerticalBox> Overlay;
	TSharedPtr<SBorder> NodeBody;
	TSharedPtr<SHorizontalBox> OutputPinBox;
	
	TSharedPtr<IDetailsView> DetailsView;
	TArray<UObject*> Objects;
	
	TSharedPtr<SMultiLineEditableTextBox> DescriptionBox;

	FText NodeText;
};
