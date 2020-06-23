// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMUIManager.h"
#include "../Enum/DMPathEnum.h"
#include "Base/DMBaseManager.h"



class THIRDPERSON_API DMPathManager : public DMBaseManager<DMPathManager>
{
private:
	TMap<EDMPath, FString> PathList;

private:
	void AddPath(const EDMPath IN InType, const FString IN InPath);

public:
	DMPathManager();
	~DMPathManager();

	FString GetPath(const EDMPath IN InType);
	FString GetUIPanelPath(const EDMPanelKind IN InKind);
	FString GetUISlotPath(const EDMSlotKind IN InKind);
};
