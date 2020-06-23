// Fill out your copyright notice in the Description page of Project Settings.


#include "DMPathManager.h"
#include "../Util/DMEnumUtil.h"


DMPathManager::DMPathManager()
{
	AddPath(EDMPath::UIPanel, "/Game/StarterContent/Blueprints/Assets/UI/Panel/");
	AddPath(EDMPath::UISlot, "/Game/StarterContent/Blueprints/Assets/UI/Slot/");
}

DMPathManager::~DMPathManager()
{
}

void DMPathManager::AddPath(const EDMPath IN InType, const FString IN InPath)
{
	PathList.Add(InType, InPath);
}

FString DMPathManager::GetPath(const EDMPath IN InType)
{
	return PathList.FindRef(InType);
}

FString DMPathManager::GetUIPanelPath(const EDMPanelKind IN InKind)
{
	FString strPath = GetPath(EDMPath::UIPanel);
	FString strPanelName = DMEnumUtil::EnumToString<EDMPanelKind>("EDMPanelKind", InKind);
	strPath += FString::Printf(TEXT("Panel_%s.Panel_%s_C"), *strPanelName, *strPanelName);
	return strPath;
}

FString DMPathManager::GetUISlotPath(const EDMSlotKind IN InKind)
{
	FString strPath = GetPath(EDMPath::UISlot);
	FString strSlotName = DMEnumUtil::EnumToString<EDMSlotKind>("EDMSlotKind", InKind);
	strPath += FString::Printf(TEXT("Slot_%s.Slot_%s_C"), *strSlotName, *strSlotName);
	return strPath;
}
