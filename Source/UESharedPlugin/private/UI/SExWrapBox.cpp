// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "UI/SExWrapBox.h"
#include "Layout/LayoutUtils.h"

SExWrapBox::SExWrapBox()
: Slots(this)
{
}

SExWrapBox::FSlot& SExWrapBox::Slot()
{
	return *( new SExWrapBox::FSlot() );
}

SExWrapBox::FSlot& SExWrapBox::AddSlot()
{
	SExWrapBox::FSlot* NewSlot = new SExWrapBox::FSlot();
	Slots.Add(NewSlot);
	return *NewSlot;
}

int32 SExWrapBox::RemoveSlot( const TSharedRef<SWidget>& SlotWidget )
{
	for (int32 SlotIdx = 0; SlotIdx < Slots.Num(); ++SlotIdx)
	{
		if ( SlotWidget == Slots[SlotIdx].GetWidget() )
		{
			Slots.RemoveAt(SlotIdx);
			return SlotIdx;
		}
	}

	return -1;
}

void SExWrapBox::Construct( const FArguments& InArgs )
{
	PreferredWidth = InArgs._PreferredWidth;
	InnerSlotPadding = InArgs._InnerSlotPadding;
	bUseAllottedWidth = InArgs._UseAllottedWidth;
	bGrowUnderOldLine = InArgs._GrowUnderOldLine;
	bGrowUp = InArgs._GrowUp;
	// Copy the children from the declaration to the widget
	for ( int32 ChildIndex=0; ChildIndex < InArgs.Slots.Num(); ++ChildIndex )
	{
		Slots.Add( InArgs.Slots[ChildIndex] );
	}
}

void SExWrapBox::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
	if (bUseAllottedWidth)
	{
		PreferredWidth = AllottedGeometry.GetLocalSize().X;
	}
	PreferredLength= AllottedGeometry.GetLocalSize().Y;
}

/*
 * Simple class for handling the somewhat complex state tracking for wrapping based on otherwise simple rules.
 * Singular static method in public interface simplifies use to a single function call by encapsulating and hiding
 * awkward inline helper object instantiation and method calls from user code. 
 */
class SExWrapBox::FChildArranger
{
public:
	struct FArrangementData
	{
		FVector2D SlotOffset;
		FVector2D SlotSize;
	};

	typedef TFunctionRef<void(const FSlot& Slot, const FArrangementData& ArrangementData)> FOnSlotArranged;

	static void Arrange(const SExWrapBox& WrapBox, const FOnSlotArranged& OnSlotArranged);

private:
	FChildArranger(const SExWrapBox& WrapBox, const FOnSlotArranged& OnSlotArranged);
	void Arrange();
	void FinalizeLine(const int32 IndexOfLastChildInCurrentLine);

	const SExWrapBox& WrapBox;
	const FOnSlotArranged& OnSlotArranged;
	FVector2D Offset;
	float MaximumHeightInCurrentLine;
	int32 IndexOfFirstChildInCurrentLine;
	TMap<int32, FArrangementData> OngoingArrangementDataMap;
};


SExWrapBox::FChildArranger::FChildArranger(const SExWrapBox& InWrapBox, const FOnSlotArranged& InOnSlotArranged)
	: WrapBox(InWrapBox)
	, OnSlotArranged(InOnSlotArranged)
	, Offset(FVector2D::ZeroVector)
	, MaximumHeightInCurrentLine(0.0f)
	, IndexOfFirstChildInCurrentLine(INDEX_NONE)
{
	OngoingArrangementDataMap.Reserve(WrapBox.Slots.Num());
}

void SExWrapBox::FChildArranger::Arrange()
{
	int32 ChildIndex;
	if (WrapBox.bGrowUp) {
		Offset.Y = WrapBox.PreferredLength;
	}
	for (ChildIndex = 0; ChildIndex < WrapBox.Slots.Num(); ++ChildIndex)
	{
		const FSlot& Slot = WrapBox.Slots[ChildIndex];
		const TSharedRef<SWidget>& Widget = Slot.GetWidget();

		/*
		* Simple utility lambda for determining if the current child index is the first child of the current line.
		*/
		const auto& IsFirstChildInCurrentLine = [&]() -> bool
		{
			return ChildIndex == IndexOfFirstChildInCurrentLine;
		};

		// Skip collapsed widgets.
		if (Widget->GetVisibility() == EVisibility::Collapsed)
		{
			continue;
		}

		FArrangementData& ArrangementData = OngoingArrangementDataMap.Add(ChildIndex, FArrangementData());

		// If there is no first child in the current line, we must be the first child.
		if (IndexOfFirstChildInCurrentLine == INDEX_NONE)
		{
			IndexOfFirstChildInCurrentLine = ChildIndex;
		}

		/*
		* Simple utility lambda for beginning a new line with the current child, updating it's offset for the new line.
		*/
		const auto& BeginNewLine = [&]()
		{
			FinalizeLine(ChildIndex - 1);

			// Starting a new line.
			IndexOfFirstChildInCurrentLine = ChildIndex;

			// Update child's offset to new X and Y values for new line.
			ArrangementData.SlotOffset.X = Offset.X;
			ArrangementData.SlotOffset.Y = Offset.Y;
		};

		// Rule: If this child is not the first child in the line, "inner slot padding" needs to be injected left of it.
		if (!IsFirstChildInCurrentLine())
		{
			Offset.X += WrapBox.InnerSlotPadding.X;
		}

		const FVector2D DesiredSizeOfSlot = Slot.SlotPadding.Get().GetDesiredSize() + Widget->GetDesiredSize();

		// Populate arrangement data with default size and offset at right end of current line.
		ArrangementData.SlotOffset.X = Offset.X;
		ArrangementData.SlotOffset.Y = Offset.Y;
		ArrangementData.SlotSize.X = DesiredSizeOfSlot.X;
		ArrangementData.SlotSize.Y = DesiredSizeOfSlot.Y;

		const float RightBoundOfChild = ArrangementData.SlotOffset.X + ArrangementData.SlotSize.X;

		// Rule: If required due to a wrapping width under specified threshold, start a new line and allocate all of it to this child.
		if (Slot.SlotFillLineWhenWidthLessThan.IsSet() && WrapBox.PreferredWidth.Get() < Slot.SlotFillLineWhenWidthLessThan.GetValue())
		{
			// Begin a new line if the current one isn't empty, because we demand a whole line to ourselves.
			if (!IsFirstChildInCurrentLine())
			{
				BeginNewLine();
			}

			// Fill width of rest of wrap box.
			ArrangementData.SlotSize.X = WrapBox.PreferredWidth.Get() - Offset.X;
		}
		// Rule: If the end of a child would go beyond the width to wrap at, it should move to a new line.
		else if (RightBoundOfChild > WrapBox.PreferredWidth.Get())
		{
			// Begin a new line if the current one isn't empty, because we demand a new line.
			if (!IsFirstChildInCurrentLine())
			{
				BeginNewLine();
			}
		}

		// Update current line maximum height.
		MaximumHeightInCurrentLine = FMath::Max(MaximumHeightInCurrentLine, ArrangementData.SlotSize.Y);

		// Update offset to right bound of child.
		Offset.X = ArrangementData.SlotOffset.X + ArrangementData.SlotSize.X;
	}

	// Attempt to finalize the final line if there are any children in it.
	if (IndexOfFirstChildInCurrentLine != INDEX_NONE)
	{
		FinalizeLine(ChildIndex - 1);
	}
	for (int32 tChildIndex = 0; tChildIndex < ChildIndex; ++tChildIndex)
	{
		const FSlot& Slot = WrapBox.Slots[tChildIndex];
		const TSharedRef<SWidget>& Widget = Slot.GetWidget();

		// Skip collapsed widgets.
		if (Widget->GetVisibility() == EVisibility::Collapsed)
		{
			continue;
		}
		FArrangementData& ArrangementData = OngoingArrangementDataMap[tChildIndex];
		OnSlotArranged(Slot, ArrangementData);
	}
}

void SExWrapBox::FChildArranger::FinalizeLine(const int32 IndexOfLastChildInCurrentLine)
{
	// Iterate backwards through children in this line. Iterate backwards because the last uncollapsed child may wish to fill the remaining empty space of the line.
	bool IsLastUncollapsedChild = true;

	// Set initial state for new line.
	Offset.X = 0.0f;
	// Since this is the initial state for a new line, this only happens after the first line, so the inner slot vertical padding should always be added.
	if (WrapBox.bGrowUp) {
		if (WrapBox.bGrowUnderOldLine) {
			for (int32 ChildIndex = 0; ChildIndex <=IndexOfLastChildInCurrentLine; ++ChildIndex)
			{
				const FSlot& Slot = WrapBox.Slots[ChildIndex];
				const TSharedRef<SWidget>& Widget = Slot.GetWidget();

				// Skip collapsed widgets.
				if (Widget->GetVisibility() == EVisibility::Collapsed)
				{
					continue;
				}
				FArrangementData& ArrangementData = OngoingArrangementDataMap[ChildIndex];
				ArrangementData.SlotOffset.Y -= MaximumHeightInCurrentLine + WrapBox.InnerSlotPadding.Y;
			}
			for (int32 ChildIndex = IndexOfLastChildInCurrentLine; ChildIndex >= IndexOfFirstChildInCurrentLine; --ChildIndex)
			{
				const FSlot& Slot = WrapBox.Slots[ChildIndex];
				const TSharedRef<SWidget>& Widget = Slot.GetWidget();

				// Skip collapsed widgets.
				if (Widget->GetVisibility() == EVisibility::Collapsed)
				{
					continue;
				}
				FArrangementData& ArrangementData = OngoingArrangementDataMap[ChildIndex];
				ArrangementData.SlotOffset.Y -= MaximumHeightInCurrentLine;
			}
		}
		else {
			Offset.Y -= MaximumHeightInCurrentLine + WrapBox.InnerSlotPadding.Y;
			for (int32 ChildIndex = IndexOfLastChildInCurrentLine; ChildIndex >= IndexOfFirstChildInCurrentLine; --ChildIndex)
			{
				const FSlot& Slot = WrapBox.Slots[ChildIndex];
				const TSharedRef<SWidget>& Widget = Slot.GetWidget();

				// Skip collapsed widgets.
				if (Widget->GetVisibility() == EVisibility::Collapsed)
				{
					continue;
				}
				FArrangementData& ArrangementData = OngoingArrangementDataMap[ChildIndex];
				ArrangementData.SlotOffset.Y -= MaximumHeightInCurrentLine;
			}
		}
	}else {
		if (WrapBox.bGrowUnderOldLine) {
			Offset.Y += MaximumHeightInCurrentLine + WrapBox.InnerSlotPadding.Y;
		}
		else {
			for (int32 ChildIndex =0 ; ChildIndex <IndexOfFirstChildInCurrentLine; ++ChildIndex)
			{
				const FSlot& Slot = WrapBox.Slots[ChildIndex];
				const TSharedRef<SWidget>& Widget = Slot.GetWidget();

				// Skip collapsed widgets.
				if (Widget->GetVisibility() == EVisibility::Collapsed)
				{
					continue;
				}
				FArrangementData& ArrangementData = OngoingArrangementDataMap[ChildIndex];
				ArrangementData.SlotOffset.Y += MaximumHeightInCurrentLine + WrapBox.InnerSlotPadding.Y;
			}
		}
	}
	

	for (int32 ChildIndex = IndexOfLastChildInCurrentLine; ChildIndex >= IndexOfFirstChildInCurrentLine; --ChildIndex)
	{
		const FSlot& Slot = WrapBox.Slots[ChildIndex];
		const TSharedRef<SWidget>& Widget = Slot.GetWidget();

		// Skip collapsed widgets.
		if (Widget->GetVisibility() == EVisibility::Collapsed)
		{
			continue;
		}

		FArrangementData& ArrangementData = OngoingArrangementDataMap[ChildIndex];

		// Rule: The last uncollapsed child in a line may request to fill the remaining empty space in the line.
		if (IsLastUncollapsedChild && Slot.bSlotFillEmptySpace)
		{
			ArrangementData.SlotSize.X = WrapBox.PreferredWidth.Get() - ArrangementData.SlotOffset.X;
		}
		IsLastUncollapsedChild = false;
	}

	
	MaximumHeightInCurrentLine = 0.0f;
	IndexOfFirstChildInCurrentLine = INDEX_NONE;
}

void SExWrapBox::FChildArranger::Arrange(const SExWrapBox& WrapBox, const FOnSlotArranged& OnSlotArranged)
{
	FChildArranger(WrapBox, OnSlotArranged).Arrange();
}

void SExWrapBox::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	FChildArranger::Arrange(*this, [&](const FSlot& Slot, const FChildArranger::FArrangementData& ArrangementData)
	{
		// Calculate offset and size in slot using alignment.
		const AlignmentArrangeResult XResult = AlignChild<Orient_Horizontal>(ArrangementData.SlotSize.X, Slot, Slot.SlotPadding.Get());
		const AlignmentArrangeResult YResult = AlignChild<Orient_Vertical>(ArrangementData.SlotSize.Y, Slot, Slot.SlotPadding.Get());

		// Note: Alignment offset is relative to slot offset.
		const FVector2D PostAlignmentOffset = ArrangementData.SlotOffset + FVector2D(XResult.Offset, YResult.Offset);
		const FVector2D PostAlignmentSize = FVector2D(XResult.Size, YResult.Size);

		ArrangedChildren.AddWidget(AllottedGeometry.MakeChild(Slot.GetWidget(), PostAlignmentOffset, PostAlignmentSize));
	});
}

void SExWrapBox::ClearChildren()
{
	Slots.Empty();
}

FVector2D SExWrapBox::ComputeDesiredSize( float ) const
{
	FVector2D MyDesiredSize = FVector2D::ZeroVector;

	FChildArranger::Arrange(*this, [&](const FSlot& Slot, const FChildArranger::FArrangementData& ArrangementData)
	{
		// Increase desired size to the maximum X and Y positions of any child widget.
		MyDesiredSize.X = FMath::Max(MyDesiredSize.X, ArrangementData.SlotOffset.X + ArrangementData.SlotSize.X);
		MyDesiredSize.Y = FMath::Max(MyDesiredSize.Y, ArrangementData.SlotOffset.Y + ArrangementData.SlotSize.Y);
	});

	return MyDesiredSize;
}

FChildren* SExWrapBox::GetChildren()
{
	return &Slots;	
}

void SExWrapBox::SetInnerSlotPadding(FVector2D InInnerSlotPadding)
{
	InnerSlotPadding = InInnerSlotPadding;
}

void SExWrapBox::SetWrapWidth(const TAttribute<float>& InWrapWidth)
{
	PreferredWidth = InWrapWidth;
}

void SExWrapBox::SetUseAllottedWidth(bool bInUseAllottedWidth)
{
	bUseAllottedWidth = bInUseAllottedWidth;
}
