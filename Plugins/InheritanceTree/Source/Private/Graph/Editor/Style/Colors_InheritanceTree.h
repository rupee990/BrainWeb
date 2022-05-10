#pragma once

#include "CoreMinimal.h"

namespace InheritanceTreeColors
{
	namespace NodeBody
	{
		const FLinearColor Default(0.1f, 0.1f, 0.1f);
		const FLinearColor Root(0.5f, 0.5f, 0.5f, 0.1f);
		const FLinearColor Error(1.0f, 0.0f, 0.0f);
	}

	namespace NodeBorder
	{
		const FLinearColor HighlightAbortRange0(0.0f, 0.22f, 0.4f);
	}

	namespace Pin
	{
		const FLinearColor Diff(0.9f, 0.2f, 0.15f);
		const FLinearColor Hover(1.0f, 0.7f, 0.0f);
		const FLinearColor Default(0.02f, 0.02f, 0.02f);
		const FLinearColor SingleNode(0.02f, 0.02f, 0.02f);
	}
	
	namespace Connection
	{
		const FLinearColor Default(1.0f, 1.0f, 1.0f);
	}
}
