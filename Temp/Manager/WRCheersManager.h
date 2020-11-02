// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "SingletonClass/WRSingleton.h"
#include "CoreMinimal.h"
#include "Define/WRTypeDefine.h"
#include "Enum/EWRCharacter.h"

enum class EWRCheersState : uint8
{
	None,
	CheersPossible,
	Suggest,
	AnswerWait,
	AnswerYes,
	AnswerNo,
};

class WR_API WRCheersManager : public WRSingleton<WRCheersManager>
{
	CDECLARE_MANAGER(WRCheersManager);
	
private:
	WRCheersManager();
	virtual ~WRCheersManager();

private:
	// Cheers
	bool bHideCheersContents = false;
	AID_t CheersSuggesterAID = INVALID_AID;
	TMap<AID_t, EWRCheersState> CheersAnswerAIDs;
	EWRCheersState MyAnswerState = EWRCheersState::None;
	bool bUseCheersSuggestTime = false;
	float fCheersSuggestElapsedTime = 0.f;

	// [ 2020-4-17 : kyu ] 건배 가능한 사람들에게 아이콘 띄우기
	TArray<AID_t> CheersPossiblePlayers;
	float fUpdateForCheersDeltaTime = 0.f;

public:
	//====================================================================================
	// [ 2019-6-14 : magedoga ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
	virtual void OnInitialize()			override;

	// [ 2019-6-14 : magedoga ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
	virtual void OnShutdown()			override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
	virtual void OnLoadLevelStart()		override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
	virtual void OnRenderStart() 		override;

	virtual bool OnTick(float InDeltaTime) override;
	//====================================================================================

	// Input
	bool CheckSocialInput(EWRInput IN InInput);

	//====================================================================================
	// [ 2020-3-13 : kyu ] Cheers
private:
	void AddCheersAnswerAID(const AID_t IN InAID);
	void RemoveCheersAnswerAID(const AID_t IN InAID);
	void RemoveCheersAnswer(const AID_t IN InAID);
	void ChangeCheersAnswerState(const AID_t IN InAID, const EWRCheersState IN InState, const bool IN InUpdateIcon = true);
	void ClearCheersInfo(const bool IN InEachCharacterRelease = false);
	// UI
	void OpenCheersIcon(const AID_t IN InAID, const EWRCheersState IN InState);	// 상태 아이콘 켬
	void CloseCheersIcon(const AID_t IN InAID);									// 상태 아이콘 끔
	void OpenCheersUI_Suggest();	// UI : 건배 제안 보냄
	void OpenCheersUI_Suggested();	// UI : 건배 제안 받음
	void OpenCheersUI_Impossible();	// UI : 건배 요청 불가
	// Etc
	void StartSuggestTime();
	void UpdateSuggestTime(const float IN InDeltaTime);	
	bool IsValidDrinker(const AID_t IN InAID, const bool IN InUseSuggest);
	bool IsValidDrinker(class AWRCharacter* IN InChar, const bool IN InUseSuggest);
	bool IsAllCheersAnswerNo();
	void OnStartCheers();			// for UI exception
	void OnEndCheers();				// for UI exception
public:	
	bool IsSuggestTime() { return bUseCheersSuggestTime; }
	void RotateComplete(const AID_t IN InSuggesterAID, const AID_t IN InAnswerAID);
	void UpdateForCheersPossiblePlayers(float fDeltaTime);
	// UI
	void OpenCheersUI_Guide();		// UI : 술잔 가이드	
	bool AttemptToOpenCheersContents();		// for UI exception
	bool PauseCheersContents();				// for UI exception
	// Throw Out
	void OnUnEquipBottle();
	// Req
	void Req_SuggestCheers();					// 건배 제안
	void Req_AnswerCheers(bool IN InAnswer);	// 건배 응답
	void Req_BeginCheers();						// 건배 시작
	void Req_CancelCheers();					// 건배 취소
	// Ack
	void Ack_SuggestCheers(const AID_t IN InAnswerAID);
	void Ack_AnswerCheers(const AID_t IN InSuggestAID, bool IN InAnswer);
	void Ack_BeginCheers(const AID_t IN InAnswerAID);
	void Ack_CancelCheers(const AID_t IN InTargetAID);
	// Inf
	void Inf_SuggestCheers(const AID_t IN InSuggestAID, uint8 IN InSuggesterCondition, const AID_t IN InAnswerAID, uint8 IN InAnswerCondition);
	void Inf_AnswerCheers(const AID_t IN InSuggestAID, uint8 IN InSuggesterCondition, const AID_t IN InAnswerAID, uint8 IN InAnswerCondition, const bool IN InAccept);
	void Inf_BeginCheers(const AID_t IN InSuggestAID, uint8 IN InSuggesterCondition, const AID_t IN InAnswerAID, uint8 IN InAnswerCondition);
	void Inf_CancelCheers(const AID_t IN InCancellerAID, uint8 IN InCancellerCondition, const AID_t IN InTargetAID, uint8 IN InTargetCondition, const AID_t IN InSuggesterAID);

	//====================================================================================


};