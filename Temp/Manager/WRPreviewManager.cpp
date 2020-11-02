#include "WRPreviewManager.h"
#include "WRUIManager.h"
#include "Actor/PreviewActor/WRPreviewStudio.h"
#include "UI/Slot/WRUISlot_Preview.h"





//====================================================================================

WRPreviewManager::WRPreviewManager()
{
}


WRPreviewManager::~WRPreviewManager()
{
}

void WRPreviewManager::ReleaseData()
{
	for (auto& EachAsync : AsyncPreviewList)
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(EachAsync.Key);
	}
	AsyncPreviewList.Empty();

	for (auto& Element : PreviewList)
	{
		FWRPreviewInfo& PreviewInfo = Element.Value;
		PreviewInfo.Release(true);
	}
}

// [ 2019-6-14 : magedoga ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
void WRPreviewManager::OnInitialize()
{
        
}

// [ 2019-6-14 : magedoga ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
void WRPreviewManager::OnShutdown()
{
	ReleaseData();
}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
void WRPreviewManager::OnLoadLevelStart()
{
	ReleaseData();
}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
void WRPreviewManager::OnLoadLevelComplete()
{
        
}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
void WRPreviewManager::OnRenderStart()
{
        
}

FString WRPreviewManager::SetPreview(FWRPreviewInfo& IN InPreviewInfo)
{
	if (InPreviewInfo.PreviewWidget->IsValidLowLevel() == false)
		return "";

	// Preview Actor Async Lambda
	auto AsyncSpawnPreviewActorComplete = FWRCompleteAsyncLoad::CreateLambda([&](UObject* InObject, FString InKey)
	{
		AWRPreviewStudio* CastedPreviewStudio = Cast<AWRPreviewStudio>(InObject);
		if (CastedPreviewStudio == nullptr)
			return;

		FWRPreviewInfo* PreviewInfo = AsyncPreviewList.Find(InKey);
		if (PreviewInfo == nullptr)
			return;

		PreviewInfo->PreviewStudio = CastedPreviewStudio;
		PreviewInfo->PreviewStudio->SetPreviewTarget(*PreviewInfo);

		// 모두 셋팅되면 보이도록 하자
		if (PreviewInfo->ManualVisible == false)
		{
			PreviewInfo->PreviewWidget->OnShow();
		}

		// Add Preview List
		PreviewList.Add(PreviewInfo->PreviewWidget, *PreviewInfo);

		// Remove Async List
		AsyncPreviewList.Remove(InKey);
	});

	FString PreviewActorPath = TEXT("/Game/Asset/UI/Preview/PreviewStudio_WB.PreviewStudio_WB_C");
	FString strAsyncKey = WRAsyncLoadUObjectManager::Get()->AsyncSpawnActor(PreviewActorPath, AsyncSpawnPreviewActorComplete);

	AsyncPreviewList.Add(strAsyncKey, InPreviewInfo);
	return strAsyncKey;
}

AWRPreviewStudio* WRPreviewManager::GetPreviewStudio(class UWRUISlot_Preview* IN InPreviewWidget)
{
	FWRPreviewInfo* PreviewInfo = PreviewList.Find(InPreviewWidget);
	if (PreviewInfo == nullptr)
		return nullptr;

	return PreviewInfo->PreviewStudio;
}

void WRPreviewManager::ReleasePreview(class UWRUISlot_Preview* IN InPreviewWidget, const bool IN InParentDestroy /* = false */)
{
	// Async Check
	for (auto& EachAsync : AsyncPreviewList)
	{
		if (EachAsync.Value.PreviewWidget == InPreviewWidget)
		{
			WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(EachAsync.Key);
			return;
		}
	}

	// Preview Check
	FWRPreviewInfo* PreviewInfo = PreviewList.Find(InPreviewWidget);
	if (PreviewInfo == nullptr)
		return;

	PreviewInfo->Release();
	PreviewList.Remove(InPreviewWidget);
}

bool WRPreviewManager::ReleasePreview(const FString IN InAsyncKey, const bool IN InParentDestroy /*= false*/)
{
	FWRPreviewInfo* AsyncPreviewInfo = AsyncPreviewList.Find(InAsyncKey);
	if (AsyncPreviewInfo == nullptr)
		return false;

	WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(InAsyncKey);
	AsyncPreviewList.Remove(InAsyncKey);
	return true;
}
