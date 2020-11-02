// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUIPanel.h"
#include "Enum/EWRCharacter.h"
#include "WRUIPanel_InputIcon.generated.h"

UCLASS()
class WR_API UWRUIPanel_InputIcon : public UWRUIPanel
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void SetInputKey(const EWRInput IN InInput);
	void SetContent(const FText& IN InContent);
public:
	UPROPERTY(EditAnywhere)
		class UTexture2D* Texture_Triangle;
	UPROPERTY(EditAnywhere)
		class UTexture2D* Texture_Circle;
	UPROPERTY(EditAnywhere)
		class UTexture2D* Texture_Square;
	UPROPERTY(EditAnywhere)
		class UTexture2D* Texture_Cross;

private:
	class UTextBlock* TextBlock_Content;
	class UImage* Image_Icon;
};
