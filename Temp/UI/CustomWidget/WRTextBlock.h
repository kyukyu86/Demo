// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ProgressBar.h"
#include "TextWidgetTypes.h"
#include "SRichTextBlock.h"
#include "STextBlock.h"
#include "RichTextBlock.h"
#include "SlateTypes.h"
#include "NoExportTypes.h"
#include "SlateColor.h"
#include "WRTextBlock.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EWRTextStyleRowName : uint8 {MAX,};

DECLARE_DELEGATE(FWROnTypingEffectCompleted)

UCLASS()
class WR_API UWRTextBlock : public URichTextBlock
{
	GENERATED_BODY()
	
private:
	
public:
	void Tick_TypingEffect(const float IN InDeltaTime);		// ENgine ���� ƽ�� �ƴ� �� ���ó���� ȣ�� ������ϴ°�(TypingEffect�� ���� ����Ѵ�.)
	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	FText GetText() const;

	virtual void SetJustification(ETextJustify::Type InJustification) override;	
	virtual void SetText(const FText& InText) override;
	void SetText(const FString& InText);
	void SetText(const FName& InText);
	
	void SetColorAndOpacity(const FSlateColor& IN InColorAndOpacity);

	UFUNCTION(BlueprintCallable)
	void SetIsRichTextBlock(const bool InbRichTextBlock);

	UFUNCTION(BlueprintCallable)
	void SetFontRawName(const FName InName);

	void PlayTypingEffect(const float IN InSpeed = 0.2f, FWROnTypingEffectCompleted IN InDelegate = nullptr);
private:
	void UpdateTextBlockStyle();
	void UpdateTextStyleEnum();
	void SetTextBlockStyle(FTextBlockStyle IN InTextBlockStyle);
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bRichTextBlock;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FName FontRowName = "Default";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bFontSizeOverride = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 1, ClampMax = 1000, AllowPrivateAccess = "true"))
		int32 FontSize = 24;
	/*UPROPERTY(EditAnywhere)
		EWRTextStyleRowName DefaultFontName;*/
		

	TSharedPtr<STextBlock> MyTextBlock;
	bool bTypingEffect = false;

	float TypingEffectSpeed = 0.2f;	// �ش� �ӵ��� ���� �ϳ� �߰� . ��)TypingEffectSpeed = 0.3f -> 0.3�ʿ� �ϳ��� ���� �ϳ� �߰�
	float TypingEffectTime;
	FString TypingEffectContents;
	FWROnTypingEffectCompleted TypingEffectCompletedDelegate;
};
