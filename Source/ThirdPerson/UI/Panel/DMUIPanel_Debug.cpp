// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUIPanel_Debug.h"
#include "Components/TextBlock.h"

void UDMUIPanel_Debug::SetDebugText(FString IN InText)
{
	TextBlock_Debug->SetText(FText::FromString(InText));
}
