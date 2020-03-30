#include "SEdNode_BrainWebNode.h"
//#include "GenericGraphEditorPCH.h"
#include "Colors_BrainWeb.h"
//#include "SLevelOfDetailBranchNode.h"
#include "SInlineEditableTextBlock.h"
#include "SCommentBubble.h"
#include "SlateOptMacros.h"
#include "SGraphPin.h"
#include "GraphEditorSettings.h"
#include "EdNode_BrainWebNode.h"
#include "SBoxPanel.h"
#include "BrainWebGraph.h"
#include "ScopedTransaction.h"
#include "SBox.h"
#include "SMultiLineEditableTextBox.h"

#define LOCTEXT_NAMESPACE "EdNode_BrainWeb"

//////////////////////////////////////////////////////////////////////////
class SGenericGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SGenericGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
	{
		this->SetCursor(EMouseCursor::Default);

		bShowLabel = true;

		GraphPinObj = InPin;
		check(GraphPinObj != nullptr);

		const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
		check(Schema);

		SBorder::Construct(SBorder::FArguments()
			.BorderImage(this, &SGenericGraphPin::GetPinBorder)
			.BorderBackgroundColor(this, &SGenericGraphPin::GetPinColor)
			.OnMouseButtonDown(this, &SGenericGraphPin::OnPinMouseDown)
			.Cursor(this, &SGenericGraphPin::GetPinCursor)
			.Padding(FMargin(10.0f))
		);
	}

protected:
	virtual FSlateColor GetPinColor() const override
	{
		return BrainWebColors::Pin::Default;
	}

	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override
	{
		return SNew(STextBlock);
	}

	const FSlateBrush* GetPinBorder() const
	{
		return FEditorStyle::GetBrush(TEXT("Graph.StateNode.Body"));
	}
};

//////////////////////////////////////////////////////////////////////////
void SEdNode_BrainWebNode::Construct(const FArguments& InArgs, UEdNode_BrainWebNode* InNode)
{
	GraphNode = InNode;
	UpdateGraphNode();
	InNode->SEdNode = this;
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SEdNode_BrainWebNode::UpdateGraphNode()
{
	const FMargin NodePadding = FMargin(2.0f);

	InputPins.Empty();
	OutputPins.Empty();

	UEdNode_BrainWebNode* EdNode_Node = Cast<UEdNode_BrainWebNode>(GraphNode);

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();
	OutputPinBox.Reset();

	TSharedPtr<SErrorText> ErrorText;
	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		[
            SNew(SBorder)
            .BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
			.Padding(0.0f)
			.BorderBackgroundColor(this, &SEdNode_BrainWebNode::GetBorderBackgroundColor)
			[
				SNew(SOverlay)
				// Pins and node details
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SAssignNew(RightNodeBox, SVerticalBox)
				]

				// STATE NAME AREA
				+ SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(10.0f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						 SAssignNew(NodeBody, SBorder)
						 .BorderImage(FEditorStyle::GetBrush("BTEditor.Graph.BTNode.Body"))
						 .BorderBackgroundColor(this, &SEdNode_BrainWebNode::GetBackgroundColor)
						 .HAlign(HAlign_Fill)
						 .VAlign(VAlign_Center)
						 .Visibility(EVisibility::SelfHitTestInvisible)
						 [
						     SNew(SOverlay)
						     + SOverlay::Slot()
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Fill)
							[
								SNew(SVerticalBox)
								+ SVerticalBox::Slot()
								.AutoHeight()
								[
									SNew(SHorizontalBox)
									+ SHorizontalBox::Slot()
									.AutoWidth()
									[
										// POPUP ERROR MESSAGE
										SAssignNew(ErrorText, SErrorText)
										.BackgroundColor(this, &SEdNode_BrainWebNode::GetErrorColor)
										.ToolTipText(this, &SEdNode_BrainWebNode::GetErrorMsgToolTip)
									]

									+ SHorizontalBox::Slot()
									.AutoWidth()
									[
										SNew(SHorizontalBox)
										+ SHorizontalBox::Slot()
										.Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
										[
                                            SNew(SVerticalBox)
                                            + SVerticalBox::Slot()
											.AutoHeight()
											[
												SAssignNew(InlineEditableText, SInlineEditableTextBlock)
												.Style(FEditorStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
												.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
												.OnVerifyTextChanged(this, &SEdNode_BrainWebNode::OnVerifyNameTextChanged)
												.OnTextCommitted(this, &SEdNode_BrainWebNode::OnNameTextCommited)
												.IsReadOnly(this, &SEdNode_BrainWebNode::IsNameReadOnly)
												.IsSelected(this, &SEdNode_BrainWebNode::IsSelectedExclusively)
											]
											+ SVerticalBox::Slot()
                                            .AutoHeight()
                                            [
												NodeTitle.ToSharedRef()
                                            ]
											+ SVerticalBox::Slot()
											.FillHeight(10.0f)
											.Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
											[
													SAssignNew(DescriptionBox, SMultiLineEditableTextBox)
													.Style(FEditorStyle::Get(), "Graph.StateNode.NodeMultiLineEditableText")
													.OnTextCommitted(this, &SEdNode_BrainWebNode::OnDataCommited)
													.Text(EdNode_Node->BrainWebNode->NodeData)
													.WrapTextAt(10.0f)
											]
										]
									]
								]
							]
						]
					]
				]
			]
		];

	// Create comment bubble
	TSharedPtr<SCommentBubble> CommentBubble;
	const FSlateColor CommentColor = GetDefault<UGraphEditorSettings>()->DefaultCommentNodeTitleColor;

	SAssignNew(CommentBubble, SCommentBubble)
		.GraphNode(GraphNode)
		.Text(this, &SGraphNode::GetNodeComment)
		.OnTextCommitted(this, &SGraphNode::OnCommentTextCommitted)
		.ColorAndOpacity(CommentColor)
		.AllowPinning(true)
		.EnableTitleBarBubble(true)
		.EnableBubbleCtrls(true)
		.GraphLOD(this, &SGraphNode::GetCurrentLOD)
		.IsGraphNodeHovered(this, &SGraphNode::IsHovered);

	GetOrAddSlot(ENodeZone::TopCenter)
		.SlotOffset(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetOffset))
		.SlotSize(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetSize))
		.AllowScaling(TAttribute<bool>(CommentBubble.Get(), &SCommentBubble::IsScalingAllowed))
		.VAlign(VAlign_Top)
		[
			CommentBubble.ToSharedRef()
		];

	ErrorReporting = ErrorText;
	ErrorReporting->SetError(ErrorMsg);
	CreatePinWidgets();
}

void SEdNode_BrainWebNode::CreatePinWidgets()
{
	UEdNode_BrainWebNode* StateNode = CastChecked<UEdNode_BrainWebNode>(GraphNode);

	for (int32 PinIdx = 0; PinIdx < StateNode->Pins.Num(); PinIdx++)
	{
		UEdGraphPin* MyPin = StateNode->Pins[PinIdx];
		if (!MyPin->bHidden)
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SGenericGraphPin, MyPin);

			AddPin(NewPin.ToSharedRef());
		}
	}
}

void SEdNode_BrainWebNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
    PinToAdd->SetOwner(SharedThis(this));
    RightNodeBox->AddSlot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        .FillHeight(1.0f)
        [
            PinToAdd
        ];
    OutputPins.Add(PinToAdd);
}

bool SEdNode_BrainWebNode::IsNameReadOnly() const
{
	UEdNode_BrainWebNode* EdNode_Node = Cast<UEdNode_BrainWebNode>(GraphNode);
	check(EdNode_Node != nullptr);

	UBrainWebGraph* Graph = EdNode_Node->BrainWebNode->Graph;
	check(Graph != nullptr);

	return !Graph->bCanRenameNode || SGraphNode::IsNameReadOnly();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SEdNode_BrainWebNode::OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	SGraphNode::OnNameTextCommited(InText, CommitInfo);

	UEdNode_BrainWebNode* MyNode = CastChecked<UEdNode_BrainWebNode>(GraphNode);

	if (MyNode != nullptr && MyNode->BrainWebNode != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("BrainWebEditorRenameNode", "Brain Web Editor: Rename Node"));
		MyNode->Modify();
		MyNode->BrainWebNode->Modify();
		MyNode->BrainWebNode->SetNodeTitle(InText);
		UpdateGraphNode();
	}
}


void SEdNode_BrainWebNode::OnDataCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	UEdNode_BrainWebNode* MyNode = CastChecked<UEdNode_BrainWebNode>(GraphNode);

	if (MyNode != nullptr && MyNode->BrainWebNode != nullptr)
	{
		MyNode->BrainWebNode->NodeData = InText;
	}
}

FSlateColor SEdNode_BrainWebNode::GetBorderBackgroundColor() const
{
	UEdNode_BrainWebNode* MyNode = CastChecked<UEdNode_BrainWebNode>(GraphNode);
	return MyNode ? MyNode->GetBackgroundColor() : BrainWebColors::NodeBorder::HighlightAbortRange0;
}

FSlateColor SEdNode_BrainWebNode::GetBackgroundColor() const
{
	return BrainWebColors::NodeBody::Default;
}

EVisibility SEdNode_BrainWebNode::GetDragOverMarkerVisibility() const
{
	return EVisibility::Visible;
}

const FSlateBrush* SEdNode_BrainWebNode::GetNameIcon() const
{
	return FEditorStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Icon"));
}

#undef LOCTEXT_NAMESPACE
