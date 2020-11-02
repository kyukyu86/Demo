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

// [ 2019-6-14 : magedoga ] �Ŵ��� ���� ������ Initialize ȣ�� �� ȣ��Ǵ� �̺�Ʈ
void WRPreviewManager::OnInitialize()
{
        
}

// [ 2019-6-14 : magedoga ] ���� ���� ������ Shutdown ȣ�� �� ȣ��Ǵ� �̺�Ʈ
void WRPreviewManager::OnShutdown()
{
	ReleaseData();
}

// [ 2019-6-14 : magedoga ] ���� ���� �� ���� ���� �� ȣ��Ǵ� �̺�Ʈ
void WRPreviewManager::OnLoadLevelStart()
{
	ReleaseData();
}

// [ 2019-6-14 : magedoga ] ���� ���� �� ���� ���� �Ϸ� �� ȣ��Ǵ� �̺�Ʈ
void WRPreviewManager::OnLoadLevelComplete()
{
        
}

// [ 2019-6-14 : magedoga ] ���� ���� �� �������� ���۵� �� ȣ��Ǵ� �̺�Ʈ
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

		// ��� ���õǸ� ���̵��� ����
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
