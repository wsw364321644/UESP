// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "UI/ExWrapBox.h"
#include "UI/ExWrapBoxSlot.h"

#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
// UExWrapBox

UExWrapBox::UExWrapBox(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = false;

	SExWrapBox::FArguments Defaults;
	Visibility = UWidget::ConvertRuntimeToSerializedVisibility(Defaults._Visibility.Get());

	WrapWidth = 500;
	bExplicitWrapWidth = false;
}

void UExWrapBox::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyWrapBox.Reset();
}

UClass* UExWrapBox::GetSlotClass() const
{
	return UExWrapBoxSlot::StaticClass();
}

void UExWrapBox::OnSlotAdded(UPanelSlot* InSlot)
{
	// Add the child to the live canvas if it already exists
	if ( MyWrapBox.IsValid() )
	{
		CastChecked<UExWrapBoxSlot>(InSlot)->BuildSlot(MyWrapBox.ToSharedRef());
	}
}

void UExWrapBox::OnSlotRemoved(UPanelSlot* InSlot)
{
	// Remove the widget from the live slot if it exists.
	if ( MyWrapBox.IsValid() )
	{
		TSharedPtr<SWidget> Widget = InSlot->Content->GetCachedWidget();
		if ( Widget.IsValid() )
		{
			MyWrapBox->RemoveSlot(Widget.ToSharedRef());
		}
	}
}

UExWrapBoxSlot* UExWrapBox::AddChildWrapBox(UWidget* Content)
{
	return Cast<UExWrapBoxSlot>(Super::AddChild(Content));
}

TSharedRef<SWidget> UExWrapBox::RebuildWidget()
{
	MyWrapBox = SNew(SExWrapBox)
		.UseAllottedWidth(!bExplicitWrapWidth)
		.PreferredWidth(WrapWidth)
		.GrowUp(bGrowUp)
		.GrowUnderOldLine(bGrowUnderOldLine);
	for ( UPanelSlot* PanelSlot : Slots )
	{
		if ( UExWrapBoxSlot* TypedSlot = Cast<UExWrapBoxSlot>(PanelSlot) )
		{
			TypedSlot->Parent = this;
			TypedSlot->BuildSlot(MyWrapBox.ToSharedRef());
		}
	}

	return MyWrapBox.ToSharedRef();
}

void UExWrapBox::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	MyWrapBox->SetInnerSlotPadding(InnerSlotPadding);
	MyWrapBox->SetUseAllottedWidth(!bExplicitWrapWidth);
	MyWrapBox->SetWrapWidth(WrapWidth);
}

void UExWrapBox::SetInnerSlotPadding(FVector2D InPadding)
{
	InnerSlotPadding = InPadding;

	if ( MyWrapBox.IsValid() )
	{
		MyWrapBox->SetInnerSlotPadding(InPadding);
	}
}

#if WITH_EDITOR

const FText UExWrapBox::GetPaletteCategory()
{
	return LOCTEXT("Panel", "Panel");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
