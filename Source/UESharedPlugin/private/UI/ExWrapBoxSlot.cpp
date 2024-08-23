// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "UI/ExWrapBoxSlot.h"
#include "Components/Widget.h"

/////////////////////////////////////////////////////
// UExWrapBoxSlot

UExWrapBoxSlot::UExWrapBoxSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Slot(NULL)
{
	HorizontalAlignment = HAlign_Fill;
	VerticalAlignment = VAlign_Fill;

	bFillEmptySpace = false;
	FillSpanWhenLessThan = 0;
}

void UExWrapBoxSlot::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	Slot = NULL;
}

void UExWrapBoxSlot::BuildSlot(TSharedRef<SExWrapBox> WrapBox)
{
	Slot = &WrapBox->AddSlot()
		.Padding(Padding)
		.HAlign(HorizontalAlignment)
		.VAlign(VerticalAlignment)
		.FillEmptySpace(bFillEmptySpace)
		.FillLineWhenWidthLessThan(FillSpanWhenLessThan == 0 ? TOptional<float>() : TOptional<float>(FillSpanWhenLessThan))
		[
			Content == NULL ? SNullWidget::NullWidget : Content->TakeWidget()
		];
}

void UExWrapBoxSlot::SetPadding(FMargin InPadding)
{
	Padding = InPadding;
	if ( Slot )
	{
		Slot->Padding(InPadding);
	}
}

void UExWrapBoxSlot::SetFillEmptySpace(bool InbFillEmptySpace)
{
	bFillEmptySpace = InbFillEmptySpace;
	if ( Slot )
	{
		Slot->FillEmptySpace(InbFillEmptySpace);
	}
}

void UExWrapBoxSlot::SetFillSpanWhenLessThan(float InFillSpanWhenLessThan)
{
	FillSpanWhenLessThan = InFillSpanWhenLessThan;
	if ( Slot )
	{
		Slot->FillLineWhenWidthLessThan(InFillSpanWhenLessThan == 0 ? TOptional<float>() : TOptional<float>(InFillSpanWhenLessThan));
	}
}

void UExWrapBoxSlot::SetHorizontalAlignment(EHorizontalAlignment InHorizontalAlignment)
{
	HorizontalAlignment = InHorizontalAlignment;
	if ( Slot )
	{
		Slot->HAlign(InHorizontalAlignment);
	}
}

void UExWrapBoxSlot::SetVerticalAlignment(EVerticalAlignment InVerticalAlignment)
{
	VerticalAlignment = InVerticalAlignment;
	if ( Slot )
	{
		Slot->VAlign(InVerticalAlignment);
	}
}

void UExWrapBoxSlot::SynchronizeProperties()
{
	SetPadding(Padding);
	SetFillEmptySpace(bFillEmptySpace);
	SetFillSpanWhenLessThan(FillSpanWhenLessThan);
	SetHorizontalAlignment(HorizontalAlignment);
	SetVerticalAlignment(VerticalAlignment);
}
