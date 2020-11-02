// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Base/WRTableRow.h"
#include "Enum/EWRInputSystem.h"
#include "Base/WRTableManager.h"
#include "Enum/EWRCharacter.h"
#include "Manager/WRDialogueManager.h"
#include "WRVoiceRecognitionTable.generated.h"

/**
 * 
 */
USTRUCT()
struct FWRVoiceCommandData : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("VoiceRecognition_CommandData"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VoiceRecognitionTable")
	EWRVoiceWordType WordType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VoiceRecognitionTable")
	bool VolumeCheck;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VoiceRecognitionTable")
	bool UseWhileDriving;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VoiceRecognitionTable")
	int32 TargetTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VoiceRecognitionTable")
	FName NounWord;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VoiceRecognitionTable")
	FName VerbWord;
};

USTRUCT(BlueprintType)
struct FWRVoiceRevisionData : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("VoiceRecognition_RevisionData"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VoiceRecognitionTable")
		FName SimilarWord;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VoiceRecognitionTable")
		FName CorrectionWord;
	
	
};

USTRUCT(BlueprintType)
struct FWRVoiceEventDialogue : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("VoiceRecognition_EventDialogue"); }
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ENPCTable)
		EWRVoiceWordType WordType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ENPCTable)
		TArray<int32> Understand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ENPCTable)
		TArray<int32> Impossible;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ENPCTable)
		TArray<int32> VolumeImpossible;

	static FWRVoiceEventDialogue* GetVoiceEventDialogueData(const EWRVoiceWordType IN InVoiceWordType)
	{
		if (WRTableManager::Get() == nullptr)
			return nullptr;

		UDataTable* VoiceEventDialogueDataTable = WRTableManager::Get()->GetTable<FWRVoiceEventDialogue>();
		if (VoiceEventDialogueDataTable == nullptr)
			return nullptr;

		const TMap<FName, uint8*>& VoiceEventDialogueDataMap = VoiceEventDialogueDataTable->GetRowMap();

		for (auto& Data : VoiceEventDialogueDataMap)
		{
			FWRVoiceEventDialogue* VoiceEventDialogueData = reinterpret_cast<FWRVoiceEventDialogue*>(Data.Value);
			if (VoiceEventDialogueData == nullptr)
				continue;

			if (VoiceEventDialogueData->WordType != InVoiceWordType)
				continue;

			return VoiceEventDialogueData;
		}

		return nullptr;
	}

	static WRTableID GetRandUnderstandDialogueTID(const FWRVoiceEventDialogue* IN InVoiceEventDialogueTable)
	{
		if (InVoiceEventDialogueTable == nullptr)
			return INVALID_RECORD_TID;

		if (InVoiceEventDialogueTable->Understand.Num() == 0)
			return INVALID_RECORD_TID;

		int32 RandIndex = FMath::Rand() % InVoiceEventDialogueTable->Understand.Num();
		return InVoiceEventDialogueTable->Understand[RandIndex];
	}

	static WRTableID GetRandImpossibleDialogueTID(const FWRVoiceEventDialogue* IN InVoiceEventDialogueTable)
	{
		if (InVoiceEventDialogueTable == nullptr)
			return INVALID_RECORD_TID;

		if (InVoiceEventDialogueTable->Impossible.Num() == 0)
			return INVALID_RECORD_TID;

		int32 RandIndex = FMath::Rand() % InVoiceEventDialogueTable->Impossible.Num();
		return InVoiceEventDialogueTable->Impossible[RandIndex];
	}

	static WRTableID GetRandVolumeImposibbleDialogueTID(const FWRVoiceEventDialogue* IN InVoiceEventDialogueTable)
	{
		if (InVoiceEventDialogueTable == nullptr)
			return INVALID_RECORD_TID;

		if (InVoiceEventDialogueTable->VolumeImpossible.Num() == 0)
			return INVALID_RECORD_TID;

		int32 RandIndex = FMath::Rand() % InVoiceEventDialogueTable->VolumeImpossible.Num();
		return InVoiceEventDialogueTable->VolumeImpossible[RandIndex];
	}

	static void OpenRandUnderstandDialogue(const FWRVoiceEventDialogue* IN InVoiceEventDialogueTable)
	{
		if (WRDialogueManager::IsValid() == false)
			return;

		WRTableID DialogueTID = GetRandUnderstandDialogueTID(InVoiceEventDialogueTable);
		if (DialogueTID == INVALID_RECORD_TID)
			return;

		WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);		
	}

	static void OpenRandImpossibleDialogue(const FWRVoiceEventDialogue* IN InVoiceEventDialogueTable)
	{
		if (WRDialogueManager::IsValid() == false)
			return;

		WRTableID DialogueTID = GetRandImpossibleDialogueTID(InVoiceEventDialogueTable);
		if (DialogueTID == INVALID_RECORD_TID)
			return;

		WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);
	}

	static void OpenRandVolumeImposibbleDialogue(const FWRVoiceEventDialogue* IN InVoiceEventDialogueTable)
	{
		if (WRDialogueManager::IsValid() == false)
			return;

		WRTableID DialogueTID = GetRandVolumeImposibbleDialogueTID(InVoiceEventDialogueTable);
		if (DialogueTID == INVALID_RECORD_TID)
			return;

		WRDialogueManager::Get()->OpenDialogue(EWRDialogueType::FeedbackTalk, DialogueTID);
	}
};


USTRUCT(BlueprintType)
struct FWRVoiceSubCommandTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("VoiceRecognition_SubCommandData"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = VoiceSubCommandTable)
		FName NounWord;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = VoiceSubCommandTable)
		FName VerbWord;
};


USTRUCT(BlueprintType)
struct FWRRecommendedVoiceCommandTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("VoiceRecognition_Recommend"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = RecommendedVoiceTable)
		FName ContentsText;
};

USTRUCT(BlueprintType)
struct FWRDefaultRecommendedVoiceCommandTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("VoiceRecognition_DefaultRecommend"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = RecommendedVoiceTable)
		FName ContentsText;
};


UENUM(BlueprintType)
enum class EWRVideoChatSubCommandConditionType : uint8
{
	None = 0,

	VideoChatList,
	Calling,
	Receiving,

	MAX
};

UENUM(BlueprintType)
enum class EWRVideoChatSubCommandAction : uint8
{
	None = 0,

	ShortcutNumber,
	NickName,
	Reject,
	Calling,
	DirectCall,

	MAX
};

USTRUCT(BlueprintType)
struct FWRVideoChatVoiceSubCommandTable : public FWRTableRow
{
	GENERATED_USTRUCT_BODY()
public:
	static FString GetFileName() { return TEXT("VoiceRecognition_VideoChatSubCommand"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		EWRVideoChatSubCommandConditionType ConditionType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int32 VoiceSubCommandTID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		EWRVideoChatSubCommandAction ActionType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FName ActionValue;

	static const FWRVideoChatVoiceSubCommandTable* FindVideoChatVoiceSubCommandTable(const WRTableID IN InVoiceSubCommandTID)
	{
		UDataTable* DataTable = WRTableManager::Get()->GetTable<FWRVideoChatVoiceSubCommandTable>();
		if (DataTable == nullptr)
			return nullptr;

		const TMap<FName, uint8*>& CurrencyData = DataTable->GetRowMap();

		for (auto& iter : CurrencyData)
		{
			FWRVideoChatVoiceSubCommandTable* VideoChatVoiceSubCommandTable = reinterpret_cast<FWRVideoChatVoiceSubCommandTable*>(iter.Value);
			if (VideoChatVoiceSubCommandTable == nullptr)
				continue;

			if(VideoChatVoiceSubCommandTable->VoiceSubCommandTID != InVoiceSubCommandTID)
				continue;

			return VideoChatVoiceSubCommandTable;
		}

		return nullptr;
	}
};



