// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DMUIManager.h"
#include "../Enum/DMPathEnum.h"
#include "Singleton/DMSingleton.h"



class THIRDPERSON_API DMPathManager : public DMSingleton<DMPathManager>
{
private:
	TMap<EDMPath, FString> PathList;

private:
	void AddPath(const EDMPath IN InType, const FString IN InPath);

public:
	DMPathManager();
	virtual ~DMPathManager();

	virtual void OnInit() override;
	virtual void OnShutdown() override;

	FString GetPath(const EDMPath IN InType);
	FString GetUIPanelPath(const EDMPanelKind IN InKind);
	FString GetUISlotPath(const EDMSlotKind IN InKind);
};
