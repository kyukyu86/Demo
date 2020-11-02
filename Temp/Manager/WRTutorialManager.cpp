// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRTutorialManager.h"
#include "WRUIManager.h"
#include "WRSFXManager.h"
#include "WRQuestManager.h"
#include "WRNoticeManager.h"
#include "WRCharacterManager.h"

#include "Table/Base/WRTableManager.h"

#include "Define/WRTypeDefine.h"
#include "UI/Panel/WRUIPanel_Tutorial.h"
#include "UI/Panel/WRUIPanel_Main.h"

#include "GameMode/WRGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/PostProcessVolume.h"
#include "EngineUtils.h"
#include "UI/Panel/WRUIPanel_Menu.h"
#include "WRItemManager.h"
#include "Table/WRItemTable.h"
#include "Actor/Character/WRCharacterMine.h"

#include "Localization/WRLocalizationProcessor.h"



//====================================================================================

void FWRTutorialPreConditionForm::SetInvokePreConditionForm(const FWRTutorialInvokePreCondition& IN InCondition)
{
	eCondition = InCondition.Type;
	nValue = InCondition.Value;
}

bool FWRTutorialPreConditionForm::CheckCondition()
{
	switch (eCondition)
	{
	case EWRTutorialPreConditionType::None:
	{
		return true;
	}
	break;
	case EWRTutorialPreConditionType::EquipMain:
	{
		WREquipItemBase* pEquippedMain = WRItemManager::Get()->FindEquippedMainWeapon();
		if (pEquippedMain != nullptr)
		{
			if (nValue == 0)
			{
				return true;
			}
			else
			{
				if (pEquippedMain->GetTID() == nValue)
					return true;
			}
		}
	}
	break;
	case EWRTutorialPreConditionType::EquipShield:
	{
		WREquipItemBase* pEquippedSub = WRItemManager::Get()->FindEquippedMainWeapon();
		if (pEquippedSub != nullptr)
		{
			if (pEquippedSub->GetSubType() == EWRItemSubType::Shield)
			{
				if (nValue == 0)
				{
					return true;
				}
				else
				{
					if (pEquippedSub->GetTID() == nValue)
						return true;
				}
			}			
		}
	}
	break;
	case EWRTutorialPreConditionType::EquipHook:
	{
		WREquipItemBase* pEquippedSub = WRItemManager::Get()->FindEquippedMainWeapon();
		if (pEquippedSub != nullptr)
		{
			if (pEquippedSub->GetSubType() == EWRItemSubType::Hook)
			{
				if (nValue == 0)
				{
					return true;
				}
				else
				{
					if (pEquippedSub->GetTID() == nValue)
						return true;
				}
			}
		}
	}
	break;
	}
	return false;
}

void FWRTutorialConditionForm::Initialize()
{
	bDone = false;
	nRepeatCount = 0;
}

//====================================================================================

void FWRTutorialConditionForm::SetInvokeConditionForm(const FWRTutorialInvokeCondition& IN InCondition)
{
	eCondition = InCondition.Type;
	eKey = InCondition.Key;
	nValue = InCondition.Value;
	nRepeatCount = 0;
	nRepeatCountMax = 0;
}

void FWRTutorialConditionForm::SetConditionForm(const FWRTutorialCondition& IN InCondition)
{
	eCondition = InCondition.Type;
	eKey = InCondition.Key;
	nValue = InCondition.Value;
	nRepeatCount = 0;
	nRepeatCountMax = InCondition.RepeatCount;

	if (nRepeatCountMax > 0)
	{
		FString strRepeatCount = MakeRepeatCountText();
		ParentForm->SetTutorialMainRepeatCount(strRepeatCount);
	}
}

bool FWRTutorialConditionForm::IsDone()
{
	return bDone;
}

void FWRTutorialConditionForm::OnSkipCondition()
{
	bDone = true;
	nRepeatCount = nRepeatCountMax;
}

void FWRTutorialConditionForm::OnCondition()
{
	if (nRepeatCountMax == 0)
	{
		bDone = true;
	}
	else
	{
		++nRepeatCount;
		if (nRepeatCount == nRepeatCountMax)
		{
			bDone = true;
		}
		else
		{
			FString strRepeatCount = MakeRepeatCountText();
			ParentForm->SetTutorialMainRepeatCount(strRepeatCount);
		}
	}
}

FString FWRTutorialConditionForm::MakeRepeatCountText()
{
	return FString::Printf(TEXT("%d / %d"), nRepeatCount, nRepeatCountMax);
}

bool FWRTutorialConditionForm::CheckCondition(const EWRTutorialConditionType IN InConditionType, void* IN InValue, const bool IN InShowAlert)
{
	if (bDone)
		return true;

	switch (eCondition)
	{
	case EWRTutorialConditionType::FastAttack:
	case EWRTutorialConditionType::StrongAttack:
	{
		// 조건이 약공격일 때 키입력값으로 R1이 오지 않으면 입력경고
		if (eCondition == EWRTutorialConditionType::FastAttack)
		{
			if (InConditionType == EWRTutorialConditionType::KeyInput)
			{
				EWRInput Key = *(EWRInput*)InValue;
				if (Key != EWRInput::R1 
					&& Key != EWRInput::JoystickPull	// [ 2019-11-18 : kyu ] 자이로로 Pull, Push가 너무 자주 와서 예외처리
					&& Key != EWRInput::JoystickPush)
				{
					if (InShowAlert)
						WRTutorialManager::Get()->OpenTutorialAlert();
				}
			}
		}
		// 조건이 강공격일 때 키입력값으로 R2가 오지 않으면 입력경고
		else if (eCondition == EWRTutorialConditionType::StrongAttack)
		{
			if (InConditionType == EWRTutorialConditionType::KeyInput)
			{
				EWRInput Key = *(EWRInput*)InValue;
				if (Key != EWRInput::R2
					&& Key != EWRInput::JoystickPull	// [ 2019-11-18 : kyu ] 자이로로 Pull, Push가 너무 자주 와서 예외처리
					&& Key != EWRInput::JoystickPush)
				{
					if (InShowAlert)
						WRTutorialManager::Get()->OpenTutorialAlert();
				}
			}
		}
		
		if (InConditionType != eCondition)
			return false; // 다른 타입

		OnCondition();
	}
	break;
	case EWRTutorialConditionType::DropItem:
	case EWRTutorialConditionType::Equip:
	{
		if (InConditionType != eCondition)
			return false; // 다른 타입

		int32 ItemID = *(int32*)InValue;
		if (nValue == ItemID)
			OnCondition();
	}
	break;
	case EWRTutorialConditionType::DropMain:
	{
		if (InConditionType != eCondition)
			return false; // 다른 타입

		int32 ItemID = *(int32*)InValue;
		if (nValue == 0) // 조건값이 0이면 서브타입으로만 구분
		{
			FWRItemTableBase* DropItemTable = WRTableManager::Get()->FindRowInItemTable(ItemID);

			if (DropItemTable != nullptr)
			{
				if (DropItemTable->MainType == EWRItemMainType::Weapon)
				{
					OnCondition();
				}
			}
		}
		else
		{
			if (nValue == ItemID)
				OnCondition();
		}
	}
	break;
	case EWRTutorialConditionType::DropShield:
	case EWRTutorialConditionType::DropHook:
	{
		if (InConditionType != eCondition)
			return false; // 다른 타입

		int32 ItemID = *(int32*)InValue;
		if (nValue == 0) // 조건값이 0이면 서브타입으로만 구분
		{
			FWRItemTableBase* DropItemTable = WRTableManager::Get()->FindRowInItemTable(ItemID);

			if (DropItemTable != nullptr)
			{
				if (eCondition == EWRTutorialConditionType::DropShield && DropItemTable->SubType == EWRItemSubType::Shield)
				{
					OnCondition();
				}
				else if (eCondition == EWRTutorialConditionType::DropHook && DropItemTable->SubType == EWRItemSubType::Hook)
				{
					OnCondition();
				}
			}
		}
		else
		{
			if (nValue == ItemID)
				OnCondition();
		}
	}
	break;
	case EWRTutorialConditionType::MonsterKill:
	case EWRTutorialConditionType::Targeting:
	{
		if (InConditionType != eCondition)
			return false; // 다른 타입
		
		int32 TargetID = *(int32*)InValue;
		if (nValue == TargetID)
			OnCondition();
	}
	break;
	case EWRTutorialConditionType::SkillUse:
	{
		if (InConditionType != eCondition)
			return false; // 다른 타입
		
		int32 SkillID = *(int32*)InValue;
		if (nValue == SkillID)
			OnCondition();
	}
	break;
	case EWRTutorialConditionType::MoveToTrigger:
	{
		if (InConditionType != eCondition)
			return false; // 다른 타입

		int32 TriggerID = *(int32*)InValue;
		if (nValue == TriggerID)
			OnCondition();
	}
	break;
	case EWRTutorialConditionType::KeyInput:
	{
		if (InConditionType != eCondition)
			return false; // 다른 타입

		EWRInput Key = *(EWRInput*)InValue;
		if (eKey == Key)
		{
			OnCondition();
		}
		else
		{
			// [ 2019-11-18 : kyu ] 일반적으로는 키입력에 대해서 키입력 경고를 출력한다.
			// [ 2019-11-18 : kyu ] 자이로로 Pull, Push가 너무 자주 와서 예외처리
			if (Key != EWRInput::JoystickPull
				&& Key != EWRInput::JoystickPush)
			{
				if (InShowAlert)
					WRTutorialManager::Get()->OpenTutorialAlert();
			}
		}
	}
	break;

	case EWRTutorialConditionType::BoardingVehicle:
	{
		if (InConditionType != eCondition)
			return false; // 다른 타입

		OnCondition();
	}
	break;
	}

	if (bDone)
		return true;

	return false;
}

//====================================================================================
//
//====================================================================================

bool FWRTutorialForm::MakeTutorialForm(const WRTableID IN InTutorialTID, const bool IN InFromMenu /* = false */)
{
	if (InTutorialTID == INVALID_TABLE_TID)
	{
		ensureMsgf(false, TEXT("Invalid Tutorial TID"));
		return false;
	}

	TutorialTable = WRTableManager::Get()->FindRow<FWRTutorialTable>(InTutorialTID);
	if (TutorialTable == nullptr)
	{
		ensureMsgf(false, TEXT("Invalid Tutorial TID"));
		return false;
	}

	// 튜토리얼의 발생 전제 조건Form 작성
	FWRTutorialInvokePreCondition& InvokePreCondition = TutorialTable->InvokePreCondition;
	if (InvokePreCondition.Type != EWRTutorialPreConditionType::None)
	{
		InvokePreConditionForm.SetInvokePreConditionForm(InvokePreCondition);
		InvokePreConditionForm.SetParentForm(this);
	}
	// 튜토리얼의 발생 조건Form 작성
	for (FWRTutorialInvokeCondition InvokeCondition : TutorialTable->InvokeCondition)
	{
		if (InvokeCondition.Type != EWRTutorialConditionType::None)
		{
			FWRTutorialConditionForm NewConditionForm;
			NewConditionForm.SetInvokeConditionForm(InvokeCondition);
			NewConditionForm.SetParentForm(this);
			InvokeConditionForms.Add(NewConditionForm);
		}
	}

	// 튜토리얼의 조건Form 작성
	for (FWRTutorialCondition Condition : TutorialTable->Condition)
	{
		if (Condition.Type != EWRTutorialConditionType::None)
		{
			FWRTutorialConditionForm NewConditionForm;
			NewConditionForm.SetConditionForm(Condition);
			NewConditionForm.SetParentForm(this);
			ConditionForms.Add(NewConditionForm);
		}
	}

	SetState(EWRTutorialState::Init);
	bFromMenu = InFromMenu;

	return true;
}

void FWRTutorialForm::OnTick(float InDeltaTime)
{
	if (eState == EWRTutorialState::Opening)
	{
		fElapsedOpeningTime += InDeltaTime;
		if (fElapsedOpeningTime >= 0.5f)
		{
			SetState(EWRTutorialState::Doing);
			fElapsedOpeningTime = 0.f;
		}
		return;
	}

	// Main : UI 뜨는 시간
	if (bTickPopupTime)
	{
		fElapsedPopupTime += InDeltaTime;
		if (fElapsedPopupTime >= ((float)TutorialTable->PopupHoldingTime / 1000.f))
		{
			OnClosePopup();
		}
		else
		{
			if (TutorialTable->TutorialPanel == EWRTutorialPanel::Main)
			{
				// 유지시간 동안 퍼센테이지 올림
				float fRate = fElapsedPopupTime / ((float)TutorialTable->PopupHoldingTime / 1000.f);
				SetTutorialMainProgressBar(fRate);
			}			
		}
	}

	// Sub : 미동작 제한시간
	if (bTickLimitTime)
	{
		fElapsedLimitTime += InDeltaTime;
		if (fElapsedLimitTime >= ((float)TutorialTable->LimitTime / 1000.f))
		{
			bTickPopupTime = false;
			fElapsedPopupTime = 0.f;

			bTickLimitTime = false;
			fElapsedLimitTime = 0.f;

			// + 미동작 제한시간 UI 닫음
			WRTutorialManager::Get()->CloseTutorialSub();

			// + 미동작 알림
			if (TutorialTable->ShowPopup)
			{
				// + 팝업이 있으면 팝업을 다시 띄움
				OnOpenPopup(true);
			}
			else
			{
				if (TutorialTable->LimitReturnTID != INVALID_TABLE_TID)
				{
					// + 팝업이 없으면 특정 튜토리얼 TID 로 이동
					WRTutorialManager::Get()->OnForceNewTutorial(TutorialTable->LimitReturnTID, bFromMenu);
				}
				else
				{
					ensureMsgf(false, TEXT("Not a ShowPopup & Invalid LimitReturnTID : OnLimitTime"));
				}
			}
		}
		else
		{
			// + Skip UI Limit Time Progress
			float fRate = fElapsedLimitTime / ((float)TutorialTable->LimitTime / 1000.f);
			SetTutorialSubProgressBar(fRate);
		}
	}
}

void FWRTutorialForm::OnStartTutorial()
{
	if (TutorialTable == nullptr)
	{
		ensureMsgf(false, TEXT("Tutorial Start Failed"));
		return;
	}

	SetState(EWRTutorialState::Opening);
	
	fElapsedPopupTime = 0.f;
	fElapsedLimitTime = 0.f;

	// + Popup UI 띄움
	if (TutorialTable->ShowPopup)
	{
		OnOpenPopup();
	}
	else
	{
		bTickPopupTime = false;
		bTickLimitTime = true; // UI를 쓰지 않으면 바로 제한시간 동작
	}

	// + 시작 사운드
	if (TutorialTable->StartSound.IsEmpty() == false)
	{
		WRSFXManager::Get()->Spawn2D(TutorialTable->StartSound);
	}

	// [ 2020-2-26 : kyu ] 메인 튜토리얼에서만 동작하는 부분들
	if (GetTutorialPanelType() == EWRTutorialPanel::Main)
	{
		// + Skip UI 띄움
		if (TutorialTable->Skip)
		{
			WRTutorialManager::Get()->OpenTutorialSub(TutorialTable->Skip);
		}
		else
		{
			WRTutorialManager::Get()->CloseTutorialSub();
		}

		// + 이동 불가
		WRTutorialManager::Get()->SetTutorialMovable(TutorialTable->Movable);

		// + Blur
		WRTutorialManager::Get()->SetBlur(true);
	}	
}

void FWRTutorialForm::OnReservedTutorial()
{
	SetState(EWRTutorialState::Reserved);
}

void FWRTutorialForm::OnCompleteTutorial()
{
	if (IsFromMenu() == false)
		SetState(EWRTutorialState::Complete);

	// + 종료 사운드
	if (TutorialTable->CompleteSound.IsEmpty() == false)
	{
		WRSFXManager::Get()->Spawn2D(TutorialTable->CompleteSound);
	}
}

bool FWRTutorialForm::OnSkipTutorial()
{
	// 스킵되지 않는 튜토리얼이면 스킵 불가
	if (TutorialTable->Skip == false)
		return false;

	// 완료 설정하고
	bDone = true;
	// 조건들 스킵하고
	for (FWRTutorialConditionForm ConditionForm : ConditionForms)
	{
		ConditionForm.OnSkipCondition();
	}
	// 해제될 것들 해제
	ReleaseTutorial();

	return true;
}

void FWRTutorialForm::ReleaseTutorial()
{
	// 튜토리얼 종료시 해제될 것들

	if (WRTutorialManager::IsValid() == false)
		return;

	// + 이동 불가
	WRTutorialManager::Get()->SetTutorialMovable(true);

	// 생성됐던 UI 끄고
	WRTutorialManager::Get()->CloseTutorialMain(TutorialTable);		// Tutorial Popup
	WRTutorialManager::Get()->CloseTutorialSub();					// Tutorial LimitTime or Skip

	// 기타 변수값 초기화
	bTickPopupTime = false;
	fElapsedPopupTime = 0.f;
	bTickLimitTime = false;
	fElapsedLimitTime = 0.f;
}

void FWRTutorialForm::Initialize()
{
	bDone = false;
	for (FWRTutorialConditionForm ConditionForm : ConditionForms)
	{
		ConditionForm.Initialize();
	}
}

bool FWRTutorialForm::OnInvokeReadyEvent(const EWRTutorialConditionType IN InConditionType, void* IN InValue)
{
	if (TutorialTable == nullptr)
		return false;

	if (InvokeConditionForms.Num() == 0)
		return false;

	// 전제 조건 체크
	if (InvokePreConditionForm.CheckCondition() == false)
		return false;

	// 각 조건 체크
	bool bCheckAllDone = true;
	for (int32 i = 0; i < InvokeConditionForms.Num(); ++i)
	{
		FWRTutorialConditionForm& ConditionForm = InvokeConditionForms[i];
		if (ConditionForm.IsDone())
			continue;

		// 조건에 만족하는 행위를 하면 해당 조건 체크
		if (ConditionForm.CheckCondition(InConditionType, InValue, false) == false)
		{
			bCheckAllDone = false;
		}
	}

	// 모든 조건 만족시 액션
	if (bCheckAllDone)
	{
		OnInvokeReadyEventAction();
	}
	return bCheckAllDone;
}

void FWRTutorialForm::OnInvokeReadyEventAction()
{
	// + LimitTime 초기화
	fElapsedLimitTime = 0.f;
}

void FWRTutorialForm::OnEvent(const EWRTutorialConditionType IN InConditionType, void* IN InValue)
{
	if (TutorialTable == nullptr)
		return;

	if (eState != EWRTutorialState::Doing)
		return;

	// 키입력이 왔을 때 키입력으로 Main창이 꺼지는 경우
	if (bAnyKeyInputToClosePopup && InConditionType == EWRTutorialConditionType::KeyInput)
	{
		OnClosePopup();
		return;
	}

	// 각 조건 체크
	bool bCheckAllDone = true;
	for (int32 i = 0; i < ConditionForms.Num(); ++i)
	{
		FWRTutorialConditionForm& ConditionForm = ConditionForms[i];
		if (ConditionForm.IsDone())
			continue;

		// 조건에 만족하는 행위를 하면 해당 조건 체크
		if (ConditionForm.CheckCondition(InConditionType, InValue, true) == false)
		{
			bCheckAllDone = false;
		}
	}

	// 모든 조건 만족시 액션
	if (bCheckAllDone)
	{
		OnEventAction();
	}
}

void FWRTutorialForm::OnEventAction()
{
	// + LimitTime 초기화
	fElapsedLimitTime = 0.f;
}

#ifdef DEF_ASYNC_LOAD_TUTORIAL_KYU
void FWRTutorialForm::OnOpenPopupCompleteToAsync()
{
	// UI 유지시간이 없고 스킵이 안될 경우 아무 입력이 올때까지 떠있어야한다
	if (TutorialTable->PopupHoldingTime == 0 && TutorialTable->Skip == false)
	{
		bAnyKeyInputToClosePopup = true;
	}
	else if (TutorialTable->PopupHoldingTime != 0)
	{
		bTickPopupTime = true;	// UI 유지시간이 있으면 유지시간 동안만 떠있기
	}
	fElapsedPopupTime = 0.f;

	bTickLimitTime = false;		// 제한시간은 UI 꺼지는 시간에 맞춤
	fElapsedLimitTime = 0.f;

	// [ 2019-11-15 : kyu ] 다시 보여주는 경우 셋팅을 초기화 해야한다
	if (bReOpen)
	{
		// + Skip UI 띄움
		if (TutorialTable->Skip)
		{
			WRTutorialManager::Get()->OpenTutorialSub(TutorialTable->Skip);
		}
		else
		{
			WRTutorialManager::Get()->CloseTutorialSub();
		}
	}
}

void FWRTutorialForm::OnOpenPopup(const bool IN InAgain /* = false */)
{
	bReOpen = InAgain;

	// + UI 생성
	switch (TutorialTable->PopupType)
	{
	case EWRTutorialType::ImageAndDescription:
	{
		WRTutorialManager::Get()->OpenTutorialMain(TutorialTable);
	}
	break;
	}
}
#else//DEF_ASYNC_LOAD_TUTORIAL_KYU
void FWRTutorialForm::OnOpenPopup(const bool IN InAgain /* = false */)
{
	// UI 유지시간이 없고 스킵이 안될 경우 아무 입력이 올때까지 떠있어야한다
	if (TutorialTable->PopupHoldingTime == 0 && TutorialTable->Skip == false)
	{
		bAnyKeyInputToClosePopup = true;
	}
	else if (TutorialTable->PopupHoldingTime != 0)
	{
		bTickPopupTime = true;	// UI 유지시간이 있으면 유지시간 동안만 떠있기
	}
	fElapsedPopupTime = 0.f;

	bTickLimitTime = false;		// 제한시간은 UI 꺼지는 시간에 맞춤
	fElapsedLimitTime = 0.f;

	// + UI 생성
	switch (TutorialTable->PopupType)
	{
	case EWRTutorialType::ImageAndDescription:
	{
		WRTutorialManager::Get()->OpenTutorialMain(TutorialTable);
	}
	break;
	}

	// [ 2019-11-15 : kyu ] 다시 보여주는 경우 셋팅을 초기화 해야한다
	if (InAgain)
	{
		// + Skip UI 띄움
		if (TutorialTable->Skip)
		{
			WRTutorialManager::Get()->OpenTutorialSub(TutorialTable->Skip);
		}
		else
		{
			WRTutorialManager::Get()->CloseTutorialSub();
		}
	}
}
#endif//DEF_ASYNC_LOAD_TUTORIAL_KYU

void FWRTutorialForm::OnClosePopup()
{
	bAnyKeyInputToClosePopup = false;
	bTickPopupTime = false;
	fElapsedPopupTime = 0.f;

	// + 유지시간 종료. UI 닫음
	WRTutorialManager::Get()->CloseTutorialMain(TutorialTable);

	if (TutorialTable->TutorialPanel == EWRTutorialPanel::Main && TutorialTable->LimitTime != 0.f)
	{
		// 미동작 제한시간 시작
		bTickLimitTime = true;
		fElapsedLimitTime = 0.f;

		// + 미동작 제한시간 UI 띄움
		WRTutorialManager::Get()->OpenTutorialSub(TutorialTable->Skip);
	}
	else
	{
		// UI 뜨는 시간 종료 후 미동작 제한시간이 없으면 바로 끝
		bDone = true;
	}
}

void FWRTutorialForm::SetTutorialMainProgressBar(float IN InRate)
{
	WRTutorialManager::Get()->SetTutorialProgressBar(GetTutorialTable(), InRate);
}

void FWRTutorialForm::SetTutorialMainRepeatCount(FString& IN InText)
{
	WRTutorialManager::Get()->SetTutorialRepeatCount(GetTutorialTable(), InText);
}

void FWRTutorialForm::SetTutorialSubProgressBar(float IN InRate)
{
	if (WRUIManager::IsValid() == false)
		return;

	if (GetTutorialPanelType() != EWRTutorialPanel::Main)
		return;

	UWRUIPanel_Main* Panel_Main = WRUIManager::Get()->FindUI<UWRUIPanel_Main>(EWRUIPanelKind::Main);
	if (Panel_Main)
	{
		Panel_Main->SetTutorialSubProgressBar(InRate);
	}
}

WRTableID FWRTutorialForm::GetCurTID()
{
	if (TutorialTable != nullptr)
	{
		return FCString::Atoi(*TutorialTable->GetRowName().ToString());
	}
	return INVALID_TABLE_TID;
}

WRTableID FWRTutorialForm::GetNextTID()
{
	if (TutorialTable != nullptr)
	{
		return TutorialTable->NextTID;
	}
	return INVALID_TABLE_TID;
}

bool FWRTutorialForm::IsSame(const WRTableID IN InTutorialTID)
{
	if (TutorialTable == nullptr)
		return false;

	if (GetCurTID() == InTutorialTID)
		return true;

	return false;
}

bool FWRTutorialForm::IsDone()
{
	// 조건이 별도로 없는 경우 시간(UI 유지시간, 미동작 제한시간)에 의해 체크된다
	if (ConditionForms.Num() == 0)
	{
		return bDone;
	}
	else
	{
		// 조건이 있으면 조건에 의해 체크된다.
		for (FWRTutorialConditionForm ConditionForm : ConditionForms)
		{
			if (ConditionForm.IsDone() == false)
				return false;
		}
	}
	return true;
}

bool FWRTutorialForm::IsConditionComplete(int32 IN InConditionIndex)
{
	if (ConditionForms.IsValidIndex(InConditionIndex) == false)
		return false;

	return ConditionForms[InConditionIndex].IsDone();
}

bool FWRTutorialForm::IsAbleToSkip()
{
	return TutorialTable->Skip;
}

FString FWRTutorialForm::GetCompleteText()
{
	if (TutorialTable->CompleteText.IsEmpty())
	{
		return TEXT("");
	}
	return WRLocalizationProcessor::GetLocalizedString(EWRStringTableName::TutorialStringTable, TutorialTable->CompleteText).ToString();
}

EWRTutorialPanel FWRTutorialForm::GetTutorialPanelType()
{
	return TutorialTable->TutorialPanel;
}

//====================================================================================
//
//====================================================================================

WRTutorialManager::WRTutorialManager()
{
	
}
WRTutorialManager::~WRTutorialManager()
{

}

// [ 2019-6-14 : magedoga ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
void WRTutorialManager::OnInitialize()
{
	InitializeTutorials();
}

// [ 2019-6-14 : magedoga ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
void WRTutorialManager::OnShutdown()
{

}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
void WRTutorialManager::OnLoadLevelStart()
{
	// [ 2020-4-28 : kyu ] 레벨변경 시 예약된것 + 현재 튜토리얼을 삭제함
	ReservedTutorials.Empty();
	
	if (CurTutorial != nullptr)
	{
		CurTutorial->ReleaseTutorial(); // 현재 튜토리얼 강제 종료 (연관된 UI 등 해제)
		CurTutorial = nullptr;
	}
}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
void WRTutorialManager::OnLoadLevelComplete()
{
        
}

// [ 2019-6-14 : magedoga ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
void WRTutorialManager::OnRenderStart()
{

}

// [ 2020-3-31 : kyu ] 레벨 변경 완료 + 연출 완료 시점에 호출되는 이벤트
void WRTutorialManager::EndLoadLevelProcess()
{
	// 월드 진입시 진입연출 끝나고 떠야함
	AWRGameModeBase* GameModeBase = Cast<AWRGameModeBase>(UGameplayStatics::GetGameMode(UWRGameInstance::GetGameInstance()));
	if (GameModeBase == nullptr)
		return;

	if (GameModeBase->GetGameModeType() == EWRGameMode::Field)
	{
		bStartOnMap = true;

		TryToTutorialStart(); // 튜토리얼 시작 시도
	}
}

bool WRTutorialManager::OnTick(float InDeltaTime)
{
	if (CurTutorial == nullptr)
		return true;

	if (CurTutorial->IsDone() == false)
	{
		CurTutorial->OnTick(InDeltaTime);
	}
	else
	{
		OnTutorialComplete();
	}
	return true;
}

void WRTutorialManager::InitializeTutorials()
{
	UDataTable* pData = WRTableManager::Get()->GetTable<FWRTutorialTable>();
	if (pData == nullptr)
		return;

	TArray<FWRTutorialTable*> TempArray;
	pData->GetAllRows(FString(""), TempArray);
	if (TempArray.Num() == 0)
		return;

	for (int32 i = 0; i < TempArray.Num(); ++i)
	{
		if (TempArray.IsValidIndex(i) == false)
			break;

		FWRTutorialTable* pTable = TempArray[i];
		FWRTutorialForm* pNewTutorial = new FWRTutorialForm;
		if (pNewTutorial == nullptr)
		{
			ensureMsgf(false, TEXT("New TutorialForm Allocate Failed"));
			return;
		}

		if (pNewTutorial->MakeTutorialForm(pTable->GetTableID(), false))
		{
			InitTutorials.AddHead(pNewTutorial);
		}
	}
}

void WRTutorialManager::RemoveAllTutorials()
{
	TDoubleLinkedList<FWRTutorialForm*>::TDoubleLinkedListNode* ListNode = InitTutorials.GetHead();
	for (; ListNode != nullptr; ListNode = ListNode->GetNextNode())
	{
		FWRTutorialForm* DataToDelete = ListNode->GetValue();
		if (DataToDelete != nullptr)
		{
			delete DataToDelete;
			DataToDelete = nullptr;
		}
	}
	InitTutorials.Empty();
	ReservedTutorials.Empty();
	CompletedTutorials.Empty();

	CurTutorial = nullptr;
}

void WRTutorialManager::ClearAllTutorials()
{
	RemoveAllTutorials();
	InitializeTutorials();
}

//====================================================================================
//
//====================================================================================

void WRTutorialManager::OnNewTutorial(const WRTableID IN InTutorialTID, const bool IN InFromMenu /* = false */)
{
	if (InTutorialTID == INVALID_TABLE_TID)
		return;

	if (InFromMenu == false)
	{
		if (IsReservedTutorial(InTutorialTID))
			return; // 이미 예약된 튜로리얼은 패스

		if (IsCompletedTutorial(InTutorialTID))
			return; // 이미 완료된 튜로리얼은 패스
	}	

	AddTutorial(InTutorialTID, false, InFromMenu); // 추가(Tail)

	if (bStartOnMap)
	{
		TryToTutorialStart(); // 튜토리얼 시작 시도
	}
}

void WRTutorialManager::OnForceNewTutorial(const WRTableID IN InTutorialTID, const bool IN InFromMenu /* = false */)
{
	if (InTutorialTID == INVALID_TABLE_TID)
		return;

	// 이미 예약된 튜토리얼 삭제 (연결된 모든 튜토리얼 포함)
	RemoveAllLinkedReservedTutorial(InTutorialTID);
	// 이미 완료된 튜토리얼 삭제 (연결된 모든 튜토리얼 포함)
	RemoveAllLinkedCompletedTutorial(InTutorialTID);

	AddTutorial(InTutorialTID, true, InFromMenu); // 강제 추가(Head)

	TryToTutorialStart(true); // 튜토리얼 강제 시작
}

void WRTutorialManager::OnInvokeReadyEvent(const EWRTutorialConditionType IN InConditionType, int32 IN InValue)
{
	if (bStartOnMap == false)
		return;

	for (FWRTutorialForm* pTutorial : InitTutorials)
	{
		if (pTutorial == nullptr)
			continue;

		if (IsReservedTutorial(pTutorial->GetCurTID()))
			continue;

		if (IsCompletedTutorial(pTutorial->GetCurTID()))
			continue;

		if (pTutorial->GetState() != EWRTutorialState::Init)
			continue;

		if (pTutorial->OnInvokeReadyEvent(InConditionType, &InValue))
		{
			OnNewTutorial(pTutorial->GetCurTID());
			return;
		}
	}
}

void WRTutorialManager::OnInvokeReadyEvent(EWRInput IN InKey)
{
	if (bStartOnMap == false)
		return;

	for (FWRTutorialForm* pTutorial : InitTutorials)
	{
		if (pTutorial == nullptr)
			continue;

		if (IsReservedTutorial(pTutorial->GetCurTID()))
			continue;

		if (IsCompletedTutorial(pTutorial->GetCurTID()))
			continue;

		if (pTutorial->GetState() != EWRTutorialState::Init)
			continue;

		if (pTutorial->OnInvokeReadyEvent(EWRTutorialConditionType::KeyInput, &InKey))
		{
			OnNewTutorial(pTutorial->GetCurTID());
			return;
		}
	}
}

void WRTutorialManager::OnEvent(const EWRTutorialConditionType IN InConditionType)
{
//	OnInvokeReadyEvent();

	if (CurTutorial == nullptr)
		return;

	if (CurTutorial->IsDone())
		return;

	CurTutorial->OnEvent(InConditionType, nullptr);
	// OnEvent 이후 Done될 수 있다
	if (CurTutorial->IsDone())
	{
		OnTutorialComplete();
	}
}

void WRTutorialManager::OnEvent(const EWRTutorialConditionType IN InConditionType, int32 IN InValue)
{
	OnInvokeReadyEvent(InConditionType, InValue);

	if (CurTutorial == nullptr)
		return;

	if (CurTutorial->IsDone())
		return;

	CurTutorial->OnEvent(InConditionType, &InValue);
	if (CurTutorial->IsDone())
	{
		OnTutorialComplete();
	}
}

void WRTutorialManager::OnEvent(EWRInput IN InKey)
{
	OnInvokeReadyEvent(InKey);

	if (CurTutorial == nullptr)
		return;

	if (CurTutorial->IsDone())
		return;

	CurTutorial->OnEvent(EWRTutorialConditionType::KeyInput, &InKey);
	if (CurTutorial->IsDone())
	{
		OnTutorialComplete();
	}
}

void WRTutorialManager::OnSkip()
{
	if (CurTutorial == nullptr)
		return;

	if (CurTutorial->IsDone())
		return;

	if (CurTutorial->OnSkipTutorial())
	{
		// 이미 ReservedList에서는 빠졌기 때문에 바로 CompletedList로 넣는다
		OnTutorialComplete();
	}
}

bool WRTutorialManager::IsReservedTutorial(const WRTableID IN InTutorialTID)
{
	for (FWRTutorialForm* ReserveTutorial : ReservedTutorials)
	{
		if (ReserveTutorial != nullptr && ReserveTutorial->IsSame(InTutorialTID))
			return true;
	}
	return false;
}

bool WRTutorialManager::IsCompletedTutorial(const WRTableID IN InTutorialTID)
{
	for (FWRTutorialForm* CompleteTutorial : CompletedTutorials)
	{
		if (CompleteTutorial != nullptr && CompleteTutorial->IsSame(InTutorialTID))
			return true;
	}
	return false;
}

bool WRTutorialManager::IsDuringTutorial()
{
	if (CurTutorial == nullptr)
		return false;

	if (CurTutorial->IsDone())
		return false;

	return true;
}

void WRTutorialManager::AddTutorial(const WRTableID IN InTutorialTID, const bool IN InForceHead /* = false */, const bool IN InFromMenu /* = false */)
{
	if (InTutorialTID == INVALID_TABLE_TID)
		return;

	FWRTutorialForm* FoundForm = GetTutorialForm(InTutorialTID);
	if (FoundForm == nullptr)
		return;

	// [ 2020-4-23 : kyu ] 메뉴창에서 다시 볼 때
	if (InFromMenu)
	{
		FoundForm->Initialize();
	}

	FoundForm->SetFromMenu(InFromMenu);
	FoundForm->OnReservedTutorial();

	if (InForceHead)
		ReservedTutorials.AddHead(FoundForm);
	else
		ReservedTutorials.AddTail(FoundForm);
}

void WRTutorialManager::TryToTutorialStart(const bool IN InForceHead /* = false */)
{
	if (InForceHead)
	{
		if (CurTutorial != nullptr)
		{
			CurTutorial->ReleaseTutorial(); // 현재 튜토리얼 강제 종료 (연관된 UI 등 해제)
			CurTutorial = nullptr;
		}
	}
	else
	{
		// 이미 진행중이면 패스
		if (CurTutorial != nullptr && CurTutorial->IsDone() == false)
			return;
	}

	CurTutorial = GetNextTutorial();
	if (CurTutorial != nullptr)
	{
		CurTutorial->OnStartTutorial();
	}
	else
	{
		// + Blur
		SetBlur(false);
	}
}

void WRTutorialManager::OnTutorialComplete()
{
	// to Quest
	WRQuestManager::Get()->OnTutorialCompleted(CurTutorial->GetCurTID());

	// 이미 ReservedList에서는 빠졌기 때문에 바로 CompletedList로 넣는다
	CurTutorial->OnCompleteTutorial();
	// Menu에서 보는것이 아닐때만 완료목록에 추가
	if (CurTutorial->IsFromMenu() == false)
	{
		CompletedTutorials.AddTail(CurTutorial);
	}
	CurTutorial->ReleaseTutorial();

	// 튜토리얼이 끝났으면 다음 튜토리얼 셋팅
	WRTableID NextTID = CurTutorial->GetNextTID();
	if (NextTID != INVALID_TABLE_TID)
	{
		bool bFromMenu = CurTutorial->IsFromMenu();
		CurTutorial = nullptr;
		OnForceNewTutorial(NextTID, bFromMenu);
	}
	else
	{
		FString CompleteText = CurTutorial->GetCompleteText();
		if (CompleteText.IsEmpty() == false)
		{
			WRNoticeManager::Get()->AddNotice(CompleteText);
		}
		// 메뉴창에서 열린것이면 닫고 메뉴창을 다시 켠다
		if (CurTutorial->IsFromMenu())
		{
			UWRUIPanel_Menu* CastedPanel = WRUIManager::Get()->FindUI<UWRUIPanel_Menu>(EWRUIPanelKind::Menu);
			if (CastedPanel != nullptr)
			{
				CastedPanel->SetVisibilityUI(true);
			}
		}
		CurTutorial = nullptr;
		TryToTutorialStart();
	}
}

FWRTutorialForm* WRTutorialManager::GetTutorialForm(const WRTableID IN InTutorialTID)
{
	for (FWRTutorialForm* Tutorial : InitTutorials)
	{
		// 강제 이동하려는 튜토리얼부터 NextTID를 넘겨 삭제한다 (재귀)
		WRTableID TID = Tutorial->GetCurTID();
		if (InTutorialTID == TID)
		{
			return Tutorial;
		}
	}
	return nullptr;
}

FWRTutorialForm* WRTutorialManager::GetNextTutorial()
{
	if (ReservedTutorials.Num() == 0)
		return nullptr;

	TDoubleLinkedList<FWRTutorialForm*>::TDoubleLinkedListNode* pHead = ReservedTutorials.GetHead();
	if (pHead == nullptr)
		return nullptr;

	FWRTutorialForm* pNextTutorial = pHead->GetValue();
	ReservedTutorials.RemoveNode(pHead);
	return pNextTutorial;
}

void WRTutorialManager::RemoveAllLinkedReservedTutorial(const WRTableID IN InTutorialTID)
{
	if (InTutorialTID == INVALID_TABLE_TID)
		return;

	WRTableID NextTID = INVALID_TABLE_TID;
	for (FWRTutorialForm* Tutorial : ReservedTutorials)
	{
		// 강제 이동하려는 튜토리얼부터 NextTID를 넘겨 삭제한다 (재귀)
		WRTableID ReservedTID = Tutorial->GetCurTID();
		if (ReservedTID == InTutorialTID)
		{
			NextTID = Tutorial->GetTutorialTable()->NextTID;
			ReservedTutorials.RemoveNode(Tutorial);
			break;
		}
	}
	RemoveAllLinkedReservedTutorial(NextTID);
}

void WRTutorialManager::RemoveAllLinkedCompletedTutorial(const WRTableID IN InTutorialTID)
{
	if (InTutorialTID == INVALID_TABLE_TID)
		return;

	WRTableID NextTID = INVALID_TABLE_TID;
	for (FWRTutorialForm* Tutorial : CompletedTutorials)
	{
		// 강제 이동하려는 튜토리얼부터 NextTID를 넘겨 삭제한다 (재귀)
		WRTableID TID = Tutorial->GetCurTID();
		if (TID == InTutorialTID)
		{
			NextTID = Tutorial->GetTutorialTable()->NextTID;
			ReservedTutorials.RemoveNode(Tutorial);
			break;
		}
	}
	RemoveAllLinkedCompletedTutorial(NextTID);
}

//====================================================================================
//
//====================================================================================

void WRTutorialManager::OpenTutorialMain(FWRTutorialTable* IN InTable)
{
	if (WRUIManager::IsValid() == false)
		return;

	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			//InCombinedPanelData.WidgetComponentBase->SetPivot(FVector2D(0.5f, 0.5f));
			UWRUIPanel_Tutorial* Panel = Cast<UWRUIPanel_Tutorial>(InCombinedPanelData.UIPanel);
			if (Panel)
				Panel->SetInformation(InTable);

#ifdef DEF_ASYNC_LOAD_TUTORIAL_KYU
			// 비동기 로드
			WRTutorialManager::Get()->OpenTutorialMainCompleteToAsync();
#else//DEF_ASYNC_LOAD_TUTORIAL_KYU
#endif//DEF_ASYNC_LOAD_TUTORIAL_KYU
		}
	});

	if (InTable->TutorialPanel == EWRTutorialPanel::Main)
	{
		if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::Tutorial))
		{
			UWRUIPanel_Tutorial* pFoundWidget = WRUIManager::Get()->FindUI<UWRUIPanel_Tutorial>(EWRUIPanelKind::Tutorial);
			if (pFoundWidget != nullptr)
			{
				pFoundWidget->SetInformation(InTable);

#ifdef DEF_ASYNC_LOAD_TUTORIAL_KYU
				// 비동기 로드
				OpenTutorialMainCompleteToAsync();
#else//DEF_ASYNC_LOAD_TUTORIAL_KYU
#endif//DEF_ASYNC_LOAD_TUTORIAL_KYU
			}
		}
		else
		{
			FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Tutorial, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, WRCharacterManager::Get()->GetMyCharacter());
			WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
// 			FVector vLocation = FVector(250, 0, 0);
// 			WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::Tutorial,
// 				Cast<AWRCharacter>(WRCharacterManager::Get()->GetMyCharacter()),
// 				FTransform(FRotator(0.f, 180.f, 0.f), vLocation, FVector::OneVector),
// 				EWRUIWidgetCreationBitFlag::AttachToCamera | EWRUIWidgetCreationBitFlag::IgnoreDepth,
// 				&Delegate);
		}
	}
	else if (InTable->TutorialPanel == EWRTutorialPanel::Sub)
	{
		if (WRUIManager::Get()->IsWidgetOpened(EWRUIPanelKind::TutorialMini))
		{
			UWRUIPanel_Tutorial* pFoundWidget = WRUIManager::Get()->FindUI<UWRUIPanel_Tutorial>(EWRUIPanelKind::TutorialMini);
			if (pFoundWidget != nullptr)
			{
				pFoundWidget->SetInformation(InTable);

#ifdef DEF_ASYNC_LOAD_TUTORIAL_KYU
				// 비동기 로드
				OpenTutorialMainCompleteToAsync();
#else//DEF_ASYNC_LOAD_TUTORIAL_KYU
#endif//DEF_ASYNC_LOAD_TUTORIAL_KYU
			}
		}
		else
		{
			FWRWidgetInfo WidgetInfo(EWRUIPanelKind::TutorialMini, EWRUIWidgetCreationBitFlag::AttachToCameraAll | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, WRCharacterManager::Get()->GetMyCharacter());
			WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
// 			FVector vLocation = WRUIManager::Get()->GetWidgetPosition(EWRUIPanelKind::TutorialMini);
// 			WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::TutorialMini,
// 				Cast<AWRCharacter>(WRCharacterManager::Get()->GetMyCharacter()),
// 				FTransform(FRotator(0.f, 180.f, 0.f), vLocation, FVector::OneVector),
// 				EWRUIWidgetCreationBitFlag::AttachToCamera | EWRUIWidgetCreationBitFlag::IgnoreDepth,
// 				&Delegate);
		}
	}
}

#ifdef DEF_ASYNC_LOAD_TUTORIAL_KYU
void WRTutorialManager::OpenTutorialMainCompleteToAsync()
{
	if (CurTutorial == nullptr)
		return;

	CurTutorial->OnOpenPopupCompleteToAsync();
}
#else//DEF_ASYNC_LOAD_TUTORIAL_KYU
#endif//DEF_ASYNC_LOAD_TUTORIAL_KYU

void WRTutorialManager::CloseTutorialMain(FWRTutorialTable* IN InTable)
{
	if (WRUIManager::IsValid() == false)
		return;

	EWRUIPanelKind PanelKind = EWRUIPanelKind::Tutorial;
	if (InTable->TutorialPanel == EWRTutorialPanel::Sub)
		PanelKind = EWRUIPanelKind::TutorialMini;

	UWRUIPanel_Tutorial* Panel = WRUIManager::Get()->FindUI<UWRUIPanel_Tutorial>(PanelKind);
	if (Panel)
	{
		Panel->CloseTutorialPanel();
	}
}

void WRTutorialManager::SetTutorialProgressBar(FWRTutorialTable* IN InTable, float IN InRate)
{
	if (WRUIManager::IsValid() == false)
		return;

	EWRUIPanelKind PanelKind = EWRUIPanelKind::Tutorial;
	if (InTable->TutorialPanel == EWRTutorialPanel::Sub)
		PanelKind = EWRUIPanelKind::TutorialMini;

	UWRUIPanel_Tutorial* Panel = WRUIManager::Get()->FindUI<UWRUIPanel_Tutorial>(PanelKind);
	if (Panel)
	{
		Panel->SetProgressBar(InRate);
	}
}

void WRTutorialManager::SetTutorialRepeatCount(FWRTutorialTable* IN InTable, FString& IN InText)
{
	if (WRUIManager::IsValid() == false)
		return;

	EWRUIPanelKind PanelKind = EWRUIPanelKind::Tutorial;
	if (InTable->TutorialPanel == EWRTutorialPanel::Sub)
		PanelKind = EWRUIPanelKind::TutorialMini;

	UWRUIPanel_Tutorial* Panel = WRUIManager::Get()->FindUI<UWRUIPanel_Tutorial>(PanelKind);
	if (Panel)
	{
		Panel->SetRepeatCount(InText);
	}
}

void WRTutorialManager::OpenTutorialSub(const bool IN InShowSkip)
{
	if (WRUIManager::IsValid() == false)
		return;

	UWRUIPanel_Main* Panel_Main = WRUIManager::Get()->FindUI<UWRUIPanel_Main>(EWRUIPanelKind::Main);
	if (Panel_Main)
	{
		Panel_Main->OpenTutorialSub(InShowSkip);
	}
}

void WRTutorialManager::CloseTutorialSub()
{
	if (WRUIManager::IsValid() == false)
		return;

	UWRUIPanel_Main* Panel_Main = WRUIManager::Get()->FindUI<UWRUIPanel_Main>(EWRUIPanelKind::Main);
	if (Panel_Main)
	{
		Panel_Main->CloseTutorialSub();
	}
}

void WRTutorialManager::OpenTutorialAlert(const float IN InTime /* = 0.f */)
{
	if (WRUIManager::IsValid() == false)
		return;

	UWRUIPanel_Main* Panel_Main = WRUIManager::Get()->FindUI<UWRUIPanel_Main>(EWRUIPanelKind::Main);
	if (Panel_Main)
	{
		Panel_Main->OpenTutorialAlert(InTime);
	}
}

void WRTutorialManager::CloseTutorialAlert()
{
	if (WRUIManager::IsValid() == false)
		return;

	UWRUIPanel_Main* Panel_Main = WRUIManager::Get()->FindUI<UWRUIPanel_Main>(EWRUIPanelKind::Main);
	if (Panel_Main)
	{
		Panel_Main->CloseTutorialAlert();
	}
}

void WRTutorialManager::SetBlur(const bool IN InSet)
{
	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	for (TActorIterator<APostProcessVolume> Iter(World); Iter; ++Iter)
	{
		APostProcessVolume* PostProcessVolume = *Iter;
		if (PostProcessVolume == nullptr)
			continue;

		FPostProcessVolumeProperties PostProcessVolumeProperties = PostProcessVolume->GetProperties();
		FPostProcessSettings* Settings = const_cast<FPostProcessSettings*>(PostProcessVolumeProperties.Settings);
		if (Settings)
		{
			if (InSet)
			{
				Settings->DepthOfFieldFocalRegion = DEF_BLUR_FOCAL_REGION;
				Settings->LensFlareBokehSize = DEF_BLUR_MAX_BOKEH_SIZE;
			}
			else
			{
				Settings->DepthOfFieldFocalRegion = DEF_BLUR_DEFAULT_FOCAL_REGION;
				Settings->LensFlareBokehSize = DEF_BLUR_DEFAULT_MAX_BOKEH_SIZE;
			}
		}
	}
}

void WRTutorialManager::SetTutorialMovable(const bool IN InSet)
{
	bMovableBlock = !InSet;
}

bool WRTutorialManager::IsTutorialMovableBlock()
{
	if (IsDuringTutorial() == false)
	{
		bMovableBlock = false;
		return false;
	}

	if (bMovableBlock)
	{
		//OpenTutorialAlert(); // [ 2019-12-4 : kyu ] 입력방지상태에서 입력이 들어온경우 처리됨
	}

	return bMovableBlock;
}