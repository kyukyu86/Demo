#include "WRNoticeManager.h"
#include "WRUIManager.h"
#include "UI/Panel/WRUIPanel_Main.h"
#include "UI/Panel/WRUIPanel_Notice.h"
#include "UI/Panel/WRUIPanel_StyleNotice.h"
#include "Table/WRCustomTextTable.h"
#include "Utility/WRUIUtility.h"
#include "Table/Base/WRTableManager.h"

#include "Localization/WRLocalizationProcessor.h"


WRNoticeManager::WRNoticeManager()
{
}


WRNoticeManager::~WRNoticeManager()
{
}

// [ 2019-6-14 : magedoga ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
void WRNoticeManager::OnInitialize()
{
        
}

// [ 2019-6-14 : magedoga ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
void WRNoticeManager::OnShutdown()
{
        
}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
void WRNoticeManager::OnLoadLevelStart()
{
        
}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
void WRNoticeManager::OnLoadLevelComplete()
{
        
}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
void WRNoticeManager::OnRenderStart()
{
        
}

void WRNoticeManager::AddStyleNotice(const FString& IN InNotice)
{
	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			UWRUIPanel_StyleNotice* Panel_Notice = Cast<UWRUIPanel_StyleNotice>(InCombinedPanelData.UIPanel);
			if (Panel_Notice)
				Panel_Notice->AddNotice(InNotice);
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::StyleNotice, EWRUIWidgetCreationBitFlag::AttachToCamera | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
}

void WRNoticeManager::AddNotice(const FString& IN InNotice)
{
	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			UWRUIPanel_Notice* Panel_Notice = Cast<UWRUIPanel_Notice>(InCombinedPanelData.UIPanel);
			if (Panel_Notice)
				Panel_Notice->AddNotice(InNotice);
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Notice, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
}

void WRNoticeManager::AddNotice(WRTableID InTID)
{
	FWRCustomTextTable* CustomTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(InTID);
	if (CustomTextTable)
	{
		FString LocalKey = WRUIUtility::ConvertFNameToFString(CustomTextTable->Localkey);
		AddNotice(WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::MiscStringTable, LocalKey).ToString());
	}
}

void WRNoticeManager::AddItem(const WRTableID IN InTableID, const int32 IN InCount /* = 1 */)
{
	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			UWRUIPanel_Main* Panel_Main = Cast<UWRUIPanel_Main>(InCombinedPanelData.UIPanel);
			if (Panel_Main)
				Panel_Main->AddItem(InTableID, InCount);
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Main, EWRUIWidgetCreationBitFlag::AttachToCamera | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
}
