#include "WRSecretDialogController.h"
#include "Table/Base/WRTableManager.h"
#include "Table/WRCustomTextTable.h"
#include "Struct/WRUIStruct.h"
#include "Utility/WRUIUtility.h"
#include "Define/WRUIDelegate.h"

WRSecretDialogController::WRSecretDialogController()
{

}

WRSecretDialogController::~WRSecretDialogController()
{

}

void WRSecretDialogController::Tick(const float IN InDeltaTime)
{
}

void WRSecretDialogController::OnShutdown()
{
}

void WRSecretDialogController::OnLoadLevelStart()
{
	
}

void WRSecretDialogController::OnLoadLevelProcessEnded()
{
	
}

void WRSecretDialogController::OnCmdAgreeToSecretDialog(const AID_t IN InPartnerAID, const CHAT_GROUP_ID_t IN InGroupID)
{
	//FWRCustomTextTable* CustomTextTable = WRTableManager::Get()->FindRow<FWRCustomTextTable>(44);
	//FTransform Transform(FRotator(0.f, 180.f, 0.f), FVector(250, 0, 0), FVector::OneVector);
	//FWRPopupInfo PopupInfo;
	//PopupInfo.eButtonFlag = EPopupButtonFlag::OK_CANCEL;
	//PopupInfo.Contents = WRUIUtility::ConvertFNameToFString(CustomTextTable->Localkey);
	//PopupInfo.Delegate_First = FOnPopupButtonDelegate::BindRaw(this, &WRSecretDialogController::OnPopUpOKForSecretDialog);
	//PopupInfo.Delegate_Second = FOnPopupButtonDelegate::CreateStatic(&IMMessageSender::RequestEndVoiceCommand);
	//WRPopupManager::Get()->OpenPopup(PopupInfo, Transform);
}

void WRSecretDialogController::OnAgreeToSecretDialog(const TArray<AID_t>& IN InAIDs, const CHAT_GROUP_ID_t IN InGroupID, bool InAgree)
{

}

void WRSecretDialogController::OnEndSecretDialog()
{

}

void WRSecretDialogController::OnEndSecretDialog(const CHAT_GROUP_ID_t IN InGroupID)
{

}

void WRSecretDialogController::OnPopUpOKForSecretDialog()
{

}

void WRSecretDialogController::OnPopCancelForSecretDialog()
{

}
