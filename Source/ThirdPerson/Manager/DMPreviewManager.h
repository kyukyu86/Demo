// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PreviewStudio/DMPreviewStruct.h"
#include "Singleton/DMSingleton.h"

/**
 * 
 */
class THIRDPERSON_API DMPreviewManager : public DMSingleton<DMPreviewManager>
{
private:
	TMap<class UDMUISlot_Preview*, FDMPreviewInfo>	PreviewList;
	TMap<FString, FDMPreviewInfo>	AsyncPreviewList;

private:
	void ReleaseData();

public:
	DMPreviewManager();
	virtual ~DMPreviewManager();

	virtual void OnInit() override;
	virtual void OnShutdown() override;

	void SetPreview(FDMPreviewInfo& IN InPreviewInfo);

	class ADMPreviewStudio* GetPreviewStudio(class UDMUISlot_Preview* IN InPreviewWidget);

	void ReleasePreview(class UDMUISlot_Preview* IN InPreviewWidget, const bool IN InParentDestroy = false);
	bool ReleasePreview(const FString IN InAsyncKey, const bool IN InParentDestroy = false);
};
