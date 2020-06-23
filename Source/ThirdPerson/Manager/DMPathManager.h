// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMUIManager.h"
#include "../Enum/DMPathEnum.h"



class THIRDPERSON_API DMPathManager
{
private:
	static DMPathManager* Instance;
public:
	static DMPathManager* Get()
	{
		if (Instance == nullptr)
			Instance = new DMPathManager;
		return Instance;
	}
	static void Release()
	{
		delete Instance;
	}

private:
	TMap<EDMPath, FString> PathList;

public:
	DMPathManager();
	~DMPathManager();

private:
	void AddPath(const EDMPath IN InType, const FString IN InPath);

public:
	FString GetPath(const EDMPath IN InType);
	FString GetUIPanelPath(const EDMPanelKind IN InKind);
	FString GetUISlotPath(const EDMSlotKind IN InKind);
};
