// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Widgets/SWidget.h"
#include "SExWrapBox.h"
#include "Components/PanelWidget.h"
#include "ExWrapBox.generated.h"

class UWrapBoxSlot;

/**
 * Arranges widgets left-to-right.  When the widgets exceed the Width it will place widgets on the next line.
 * 
 * * Many Children
 * * Flows
 * * Wraps
 */
UCLASS()
class UESHAREDPLUGIN_API UExWrapBox : public UPanelWidget
{
	GENERATED_UCLASS_BODY()

public:
	/** The inner slot padding goes between slots sharing borders */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Content Layout")
	FVector2D InnerSlotPadding;

	/** When this width is exceeded, elements will start appearing on the next line. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content Layout", meta=(EditCondition = "bExplicitWrapWidth"))
	float WrapWidth;

	/** Use explicit wrap width whenever possible. It greatly simplifies layout calculations and reduces likelihood of "wiggling UI" */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content Layout")
	bool bExplicitWrapWidth;

	/** Use explicit wrap width whenever possible. It greatly simplifies layout calculations and reduces likelihood of "wiggling UI" */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content Layout")
		bool bGrowUp;

	/** Use explicit wrap width whenever possible. It greatly simplifies layout calculations and reduces likelihood of "wiggling UI" */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Content Layout")
		bool bGrowUnderOldLine;

	/** Sets the inner slot padding goes between slots sharing borders */
	UFUNCTION(BlueprintCallable, Category="Content Layout")
	void SetInnerSlotPadding(FVector2D InPadding);


public:

	/**  */
	UFUNCTION(BlueprintCallable, Category="Panel")
	UExWrapBoxSlot* AddChildWrapBox(UWidget* Content);

#if WITH_EDITOR
	// UWidget interface
	virtual const FText GetPaletteCategory() override;
	// End UWidget interface
#endif

protected:

	// UPanelWidget
	virtual UClass* GetSlotClass() const override;
	virtual void OnSlotAdded(UPanelSlot* Slot) override;
	virtual void OnSlotRemoved(UPanelSlot* Slot) override;
	// End UPanelWidget

	// UWidget interface
	virtual void SynchronizeProperties() override;
	// End of UWidget interface

	// UVisual interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	// End of UVisual interface

protected:

	TSharedPtr<class SExWrapBox> MyWrapBox;

protected:
	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface
};
