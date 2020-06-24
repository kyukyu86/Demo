// Fill out your copyright notice in the Description page of Project Settings.


#include "DMPreviewManager.h"
#include "../PreviewStudio/DMPreviewStudio.h"
#include "../UI/Slot/DMUISlot_Preview.h"
#include "DMAsyncLoadManager.h"
#include "DMPathManager.h"


DMPreviewManager::DMPreviewManager()
{
}

DMPreviewManager::~DMPreviewManager()
{
}

void DMPreviewManager::ReleaseData()
{
	for (auto& Element : PreviewList)
	{
		FDMPreviewInfo& PreviewInfo = Element.Value;
		PreviewInfo.Release(true);
	}
	AsyncPreviewList.Empty();
}

void DMPreviewManager::SetPreview(FDMPreviewInfo& IN InPreviewInfo)
{
	if (InPreviewInfo.PreviewWidget->IsValidLowLevel() == false)
		return;

	// Preview Actor Async Lambda
 	auto AsyncSpawnPreviewActorComplete = FDMCompleteAsyncLoad::CreateLambda([&](UObject* InObject, FString InKey)
 	{
 		ADMPreviewStudio* CastedPreviewStudio = Cast<ADMPreviewStudio>(InObject);
 		if (CastedPreviewStudio == nullptr)
 			return;
 
 		FDMPreviewInfo* PreviewInfo = AsyncPreviewList.Find(InKey);
 		if (PreviewInfo == nullptr)
 			return;
 
 		PreviewInfo->PreviewStudio = CastedPreviewStudio;
 		PreviewInfo->PreviewStudio->SetPreviewTarget(*PreviewInfo);
 
 		// 모두 셋팅되면 보이도록 하자
 		PreviewInfo->PreviewWidget->OnShow();
 
 		// Add Preview List
 		PreviewList.Add(PreviewInfo->PreviewWidget, *PreviewInfo);
 
 		// Remove Async List
 		AsyncPreviewList.Remove(InKey);
 	});
 
 	FString PreviewActorPath = DMPathManager::Get()->GetPath(EDMPath::Preview) + TEXT("DMPreviewStudio_BP.DMPreviewStudio_BP_C");
 	FString strAsyncKey = DMAsyncLoadManager::Get()->AsyncSpawnActor(PreviewActorPath, AsyncSpawnPreviewActorComplete);
 
 	AsyncPreviewList.Add(strAsyncKey, InPreviewInfo);
}

ADMPreviewStudio* DMPreviewManager::GetPreviewStudio(class UDMUISlot_Preview* IN InPreviewWidget)
{
	FDMPreviewInfo* PreviewInfo = PreviewList.Find(InPreviewWidget);
	if (PreviewInfo == nullptr)
		return nullptr;

	return PreviewInfo->PreviewStudio;
}

void DMPreviewManager::ReleasePreview(class UDMUISlot_Preview* IN InPreviewWidget, const bool IN InParentDestroy /* = false */)
{
	FDMPreviewInfo* PreviewInfo = PreviewList.Find(InPreviewWidget);
	if (PreviewInfo == nullptr)
		return;

	PreviewInfo->Release();
}