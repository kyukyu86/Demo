// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include <string>
#include "InputCoreTypes.h"

class UImage;
class UWidget;
class UTextBlock;
class URetainerBox;
class URichTextBlock;
class UWRTextBlock;
class UMaterialInstanceDynamic;
class AWRCharacter;
enum class ESlateVisibility : uint8;
enum class EWRInput : uint8;


class WR_API WRUIUtility
{
public:
	//====================================================================================
	//	Resource
	//====================================================================================
	static UObject* LoadImageObject(FString InImageFullPath);
	static bool SetResourceObject(UImage* const IN InImage, const FString& IN InTextureFileFullPath, const bool IN bInMatchSize = false);
	static bool SetResourceObject(UImage* const IN InImage, UObject* const IN InResourceObject, const bool IN bInMatchSize = false);

	//====================================================================================
	//	Safe
	//====================================================================================
	static bool SetTextSafe(UTextBlock* IN InTextBlock, const FString IN InText);
	static bool SetTextSafe(URichTextBlock* IN InTextBlock, const FString IN InText);
	static bool SetTextSafe(UWRTextBlock* IN InWRTextBlock, const FString& IN InText);
	static bool SetTextSafe(UWRTextBlock* IN InWRTextBlock, const FText& IN InText);
	static bool SetTextSafe(UWRTextBlock* IN InWRTextBlock, const FName& IN InText);
	static bool SetVisibilitySafe(UWidget* IN InWidget, const enum ESlateVisibility IN InVisibility);
	static bool SetImageTintColorSafe(UImage* IN InImage, const struct FSlateColor IN InSlateColor);
	static bool SetTextColorSafe(UTextBlock* IN InTextBlock, const struct FLinearColor IN InLinearColor);
	static bool SetTextColorSafe(UWRTextBlock* IN InWRTextBlock, const struct FLinearColor IN InLinearColor);

	static UMaterialInstanceDynamic* GetDynamicMaterial(UImage* const IN InImage);
	static UMaterialInstanceDynamic* GetDynamicMaterial(URetainerBox* const IN InRetainerBox);
	//static float GetAngle(const FVector3D&)

	static FText ConvertFNameToFText(const FName& IN InContent);
	static FString ConvertFNameToFString(const FName& IN InContent);
	static FName ConvertWStringToFName(const std::wstring& IN InContent);
	static FText ConvertWStringToFText(const std::wstring& IN InContent);
	static int32 ConvertFNameToInt(const FName& IN InNumContent);
	static EWRInput ConvertFKeyToEWRInput(const FKey& IN InKey);
	static FString ConvertNumberingToKUnit(const int32 IN InNumber);

	static void SetDOF(const float IN InValue);
	static void SlatePrepass(UWidget* IN InWidget);
	
};