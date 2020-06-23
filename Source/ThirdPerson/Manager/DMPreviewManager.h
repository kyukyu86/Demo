// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PreviewStudio/DMPreviewStruct.h"
#include "Base/DMBaseManager.h"

/**
 * 
 */
class THIRDPERSON_API DMPreviewManager : public DMBaseManager<DMPreviewManager>
{
private:
	TMap<class UDMUISlot_Preview*, FDMPreviewInfo>	PreviewList;
	TMap<FString, FDMPreviewInfo>	AsyncPreviewList;

private:
	void ReleaseData();

public:
	DMPreviewManager();
	~DMPreviewManager();

// 	virtual void OnInitialize()			override;
// 	virtual void OnShutdown()			override;
// 	virtual void OnLoadLevelStart()		override;
// 	virtual void OnLoadLevelComplete() 	override;
// 	virtual void OnRenderStart() 		override;

	void SetPreview(FDMPreviewInfo& IN InPreviewInfo);

	class ADMPreviewStudio* GetPreviewStudio(class UDMUISlot_Preview* IN InPreviewWidget);

	void ReleasePreview(class UDMUISlot_Preview* IN InPreviewWidget, const bool IN InParentDestroy = false);
};
