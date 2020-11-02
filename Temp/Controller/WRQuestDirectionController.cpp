#include "WRQuestDirectionController.h"
#include "Manager/WRUIManager.h"
#include "Manager/WRQuestManager.h"
#include "Manager/WRDialogueManager.h"
#include "UI/Panel/WRUIPanel_QuestBoard.h"
#include "Enum/EWRUIEnum.h"

WRQuestDirectionController::WRQuestDirectionController()
{

}

WRQuestDirectionController::~WRQuestDirectionController()
{

}

void WRQuestDirectionController::OnShutdown()
{
	Clear();
}

void WRQuestDirectionController::OnLoadLevelStart()
{
	Clear();
}

void WRQuestDirectionController::OnLoadLevelProcessEnded()
{
	
}

void WRQuestDirectionController::AddQuestDirectionData(const FWRQuestDirectionData& IN InQuestDirectionData)
{
	if (WaitQuestDirectionType != FWRQuestDirectionData::EWRQuestDirectionType::None)
	{
		QuestDirectionDataQueue.Enqueue(new FWRQuestDirectionData(InQuestDirectionData));
		return;
	}

	ProgressQuestDirection(&InQuestDirectionData);
}

void WRQuestDirectionController::AddQuestDirectionData(const FWRDialogueDirectionData& IN InDialogueDirectionData)
{
	if (WaitQuestDirectionType != FWRQuestDirectionData::EWRQuestDirectionType::None)
	{
		QuestDirectionDataQueue.Enqueue(new FWRDialogueDirectionData(InDialogueDirectionData));
		return;
	}

	ProgressQuestDirection(&InDialogueDirectionData);
}

void WRQuestDirectionController::Clear()
{
	FWRQuestDirectionData* QuestDirectionData = nullptr;
	
	for (; QuestDirectionDataQueue.Dequeue(QuestDirectionData); )
	{
		if(QuestDirectionData == nullptr)
			continue;

		delete QuestDirectionData;
	}	

	QuestDirectionDataQueue.Empty();
}

void WRQuestDirectionController::OnQuestDirectionFinished(const FWRQuestDirectionData::EWRQuestDirectionType IN InQuestDirectionType)
{
	if (InQuestDirectionType != WaitQuestDirectionType)
	{
		ensureMsgf(false, TEXT("WaitQuestDirectionType Invalid"));
		return;
	}

	WaitQuestDirectionType = FWRQuestDirectionData::None;

	if (QuestDirectionDataQueue.IsEmpty())
		return;

	DequeueQuestDirectionData();
}

void WRQuestDirectionController::ProgressQuestDirection(const FWRQuestDirectionData* IN InQuestDirectionData)
{
	if (InQuestDirectionData == nullptr)
		return;

	bool bSuccess = false;
	if (InQuestDirectionData->Type == FWRQuestDirectionData::EWRQuestDirectionType::Dialogue)
	{
		bSuccess = ProgressDialogueDirection(InQuestDirectionData);
	}
	else if (InQuestDirectionData->Type == FWRQuestDirectionData::EWRQuestDirectionType::QuestBoard)
	{
		bSuccess = ProgressQuestBoardDirection(InQuestDirectionData);
	}

	if(bSuccess && InQuestDirectionData->bWaitDirectionToFinish)
		WaitQuestDirectionType = InQuestDirectionData->Type;
}

bool WRQuestDirectionController::ProgressDialogueDirection(const FWRQuestDirectionData* IN InQuestDirectionData)
{
	if (InQuestDirectionData == nullptr)
		return false;

	if (InQuestDirectionData->Type != FWRQuestDirectionData::EWRQuestDirectionType::Dialogue)
		return false;

	const FWRDialogueDirectionData* DialogueDirectionData = static_cast<const FWRDialogueDirectionData*>(InQuestDirectionData);
	if (DialogueDirectionData == nullptr)
		return false;


	WRDialogueManager::Get()->OpenDialogue(DialogueDirectionData->DialogueType, DialogueDirectionData->DialogueTID, FSimpleDelegate::CreateRaw(this, &WRQuestDirectionController::OnQuestDirectionFinished, FWRQuestDirectionData::EWRQuestDirectionType::Dialogue));

	return true;
}

bool WRQuestDirectionController::ProgressQuestBoardDirection(const FWRQuestDirectionData* IN InQuestDirectionData)
{
	if (InQuestDirectionData == nullptr)
		return false;

	if (InQuestDirectionData->Type != FWRQuestDirectionData::EWRQuestDirectionType::QuestBoard)
		return false;

	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			UWRUIPanel_QuestBoard* Panel_QuestBoard = Cast<UWRUIPanel_QuestBoard>(InCombinedPanelData.UIPanel);
			if (Panel_QuestBoard)
				Panel_QuestBoard->DisplayQuestBoard();
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::QuestBoard, EWRUIWidgetCreationBitFlag::AttachToCamera | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);

	return true;
}

void WRQuestDirectionController::DequeueQuestDirectionData()
{
	FWRQuestDirectionData* QuestDirectionData = nullptr;

	for (; QuestDirectionDataQueue.Dequeue(QuestDirectionData); )
	{
		if (QuestDirectionData == nullptr)
			continue;

		if (QuestDirectionData->bWaitDirectionToFinish)
			break;

		ProgressQuestDirection(QuestDirectionData);
		delete QuestDirectionData;
		QuestDirectionData = nullptr;
	}

	if (QuestDirectionData != nullptr)
	{
		ProgressQuestDirection(QuestDirectionData);
		delete QuestDirectionData;
		QuestDirectionData = nullptr;
	}
}
