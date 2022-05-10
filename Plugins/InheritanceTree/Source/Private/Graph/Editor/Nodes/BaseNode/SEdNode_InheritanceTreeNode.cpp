#include "SEdNode_InheritanceTreeNode.h"
#include "SCommentBubble.h"
#include "SlateOptMacros.h"
#include "SGraphPin.h"
#include "GraphEditorSettings.h"

#include "Graph/InheritanceTreeGraph.h"
#include "ScopedTransaction.h"
#include "Graph/Editor/Nodes/BaseNode/EdNode_InheritanceTreeNode.h"
#include "Graph/Editor/Style/Colors_InheritanceTree.h"
#include "PropertyEditor/Private/SDetailsView.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"


#define LOCTEXT_NAMESPACE "EdNode_InheritanceTree"

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
		return InheritanceTreeColors::Pin::Default;
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
void SEdNode_InheritanceTreeNode::Construct(const FArguments& InArgs, UEdNode_InheritanceTreeNode* InNode)
{
	GraphNode = InNode;
	UpdateGraphNode();
	InNode->SEdNode = this;
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SEdNode_InheritanceTreeNode::UpdateGraphNode()
{
	const FMargin NodePadding = FMargin(2.0f);

	InputPins.Empty();
	OutputPins.Empty();

	UEdNode_InheritanceTreeNode* EdNode_Node = Cast<UEdNode_InheritanceTreeNode>(GraphNode);

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();
	OutputPinBox.Reset();

	TSharedPtr<SErrorText> ErrorText;
	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);

	Objects.Empty();
	Objects.Add(GraphNode);
	
	CreateDetailsView();

	Overlay = SNew(SVerticalBox)
	+ SVerticalBox::Slot()
   	.AutoHeight()
   	[
   	    NodeTitle.ToSharedRef()
   	]
	+ SVerticalBox::Slot()
	.FillHeight(10.0f)
	.AutoHeight()
	[
		DetailsView->AsShared()
	];

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		[
            SNew(SBorder)
            .BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
			.Padding(0.0f)
			.BorderBackgroundColor(this, &SEdNode_InheritanceTreeNode::GetBorderBackgroundColor)
			[
				SNew(SOverlay)
				// Pins and node details
				+ SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Bottom)
				[
					SAssignNew(RightNodeBox, SVerticalBox)
				]

				+ SOverlay::Slot()
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Top)
                [
                    SAssignNew(LeftNodeBox, SVerticalBox)
                ]

				// STATE NAME AREA
				+ SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(1.0f, 5.0f, 1.0f, 5.0f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						 SAssignNew(NodeBody, SBorder)
						 .BorderImage(FEditorStyle::GetBrush("BTEditor.Graph.BTNode.Body"))
						 .BorderBackgroundColor(this, &SEdNode_InheritanceTreeNode::GetBackgroundColor)
						 .HAlign(HAlign_Fill)
						 .VAlign(VAlign_Center)
						 .Visibility(EVisibility::SelfHitTestInvisible)
						 [
						 	SNew(SOverlay)
							+SOverlay::Slot()
							.HAlign(HAlign_Left)
                 			.VAlign(VAlign_Center)
                 			[
                 			    SNew(SBorder)
                 			    .BorderImage( FEditorStyle::GetBrush("Graph.Node.ColorSpill") )
                 			    // The extra margin on the right
                 			    // is for making the color spill stretch well past the node title
                 			    .Padding( FMargin(10,5,30,3) )
                 			    .BorderBackgroundColor( this, &SEdNode_InheritanceTreeNode::GetBackgroundColor )
                 			    [
                 			        SNew(SVerticalBox)
                 			        +SVerticalBox::Slot()
                 			            .AutoHeight()
                 			        [
                 			            SAssignNew(InlineEditableText, SInlineEditableTextBlock)
                 			            .Style( FEditorStyle::Get(), "Graph.Node.NodeTitleInlineEditableText" )
                 			            .Text( NodeTitle.Get(), &SNodeTitle::GetHeadTitle )
                 			            .OnVerifyTextChanged(this, &SEdNode_InheritanceTreeNode::OnVerifyNameTextChanged)
                 			            .OnTextCommitted(this, &SEdNode_InheritanceTreeNode::OnNameTextCommited)
                 			            .IsReadOnly( this, &SEdNode_InheritanceTreeNode::IsNameReadOnly )
                 			            .IsSelected(this, &SEdNode_InheritanceTreeNode::IsSelectedExclusively)
                 			        ]
                 			        +SVerticalBox::Slot()
                 			            .AutoHeight()
                 			        [
                 			            NodeTitle.ToSharedRef()
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

	CreatePinWidgets();
}

void SEdNode_InheritanceTreeNode::CreatePinWidgets()
{
	UEdNode_InheritanceTreeNode* StateNode = CastChecked<UEdNode_InheritanceTreeNode>(GraphNode);

	TArray<UEdGraphPin*> Result;
	TArray<UEdGraphPin*> Copy = StateNode->Pins;

	Result.Init(nullptr, StateNode->Pins.Num());

	for(int32 i = 0; i < Copy.Num(); i++)
	{
		FString PinName = Copy[i]->PinName.ToString();
		if(PinName.Contains("In"))
		{
			Result[i] = Copy[i];
			Copy.RemoveAt(i);
			i--;
		}
		else if(PinName.Contains("Out"))
		{
			int32 Idx = PinName.Len() > 3 ? FCString::Atoi(*PinName.RightChop(4)) : StateNode->Pins.Num() > 1 ? 1 : 0;
			Result[Idx] = Copy[i];
			Copy.RemoveAt(i);
			i--;
		}
	}
	
	for (UEdGraphPin* MyPin : Result)
	{
		if(MyPin)
		{
			if (!MyPin->bHidden)
			{
				TSharedPtr<SGraphPin> NewPin = SNew(SGenericGraphPin, MyPin);

				AddPin(NewPin.ToSharedRef());
			}
		}
	}
}

void SEdNode_InheritanceTreeNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
    PinToAdd->SetOwner(SharedThis(this));
	if(PinToAdd->GetDirection() == EGPD_Output)
	{
		RightNodeBox->AddSlot()
      .HAlign(HAlign_Fill)
      .VAlign(VAlign_Fill)
      .FillHeight(1.0f)
      [
          PinToAdd
      ];
	OutputPins.Add(PinToAdd);
	}
	else
	{
		LeftNodeBox->AddSlot()
      .HAlign(HAlign_Fill)
      .VAlign(VAlign_Fill)
      .FillHeight(1.0f)
      [
          PinToAdd
      ];
		InputPins.Add(PinToAdd);
	}
}

bool SEdNode_InheritanceTreeNode::IsNameReadOnly() const
{
	return true;
}

void SEdNode_InheritanceTreeNode::CreateDetailsView()
{
	FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs DetailsViewArgs(
        /*bUpdateFromSelection=*/ false,
        /*bLockable=*/ false,
        /*bAllowSearch=*/ false,
        FDetailsViewArgs::HideNameArea,
        /*bHideSelectionTip=*/ true,
        /*InNotifyHook=*/ this,
        /*InSearchInitialKeyFocus=*/ false,
        /*InViewIdentifier=*/ NAME_None);

	DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Automatic;
	DetailsViewArgs.bShowOptions = false;
	DetailsViewArgs.bAllowMultipleTopLevelObjects = true;

	DetailsView = EditModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetObjects(Objects);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SEdNode_InheritanceTreeNode::OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	SGraphNode::OnNameTextCommited(InText, CommitInfo);

	UEdNode_InheritanceTreeNode* MyNode = CastChecked<UEdNode_InheritanceTreeNode>(GraphNode);

	if (MyNode != nullptr && MyNode->InheritanceTreeNode != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("InheritanceTreeEditorRenameNode", "Brain Web Editor: Rename Node"));
		MyNode->Modify();
		MyNode->InheritanceTreeNode->Modify();
		MyNode->InheritanceTreeNode->SetNodeTitle(InText);
		UpdateGraphNode();
	}
}


void SEdNode_InheritanceTreeNode::OnDataCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	UEdNode_InheritanceTreeNode* MyNode = CastChecked<UEdNode_InheritanceTreeNode>(GraphNode);

	if (MyNode != nullptr && MyNode->InheritanceTreeNode != nullptr)
	{
		//MyNode->InheritanceTreeNode-> = InText;
	}
}

FSlateColor SEdNode_InheritanceTreeNode::GetBorderBackgroundColor() const
{
	UEdNode_InheritanceTreeNode* MyNode = CastChecked<UEdNode_InheritanceTreeNode>(GraphNode);
	return MyNode ? MyNode->GetBackgroundColor() : InheritanceTreeColors::NodeBorder::HighlightAbortRange0;
}

FSlateColor SEdNode_InheritanceTreeNode::GetBackgroundColor() const
{
	return InheritanceTreeColors::NodeBody::Default;
}

EVisibility SEdNode_InheritanceTreeNode::GetDragOverMarkerVisibility() const
{
	return EVisibility::Visible;
}

const FSlateBrush* SEdNode_InheritanceTreeNode::GetNameIcon() const
{
	return FEditorStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Icon"));
}

#undef LOCTEXT_NAMESPACE
