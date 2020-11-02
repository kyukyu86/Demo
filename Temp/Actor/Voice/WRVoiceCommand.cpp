#include "WRVoiceCommand.h"
#include "Table/WRVoiceRecognitionTable.h"

WRVoiceCommand::WRVoiceCommand()
{
}


WRVoiceCommand::~WRVoiceCommand()
{

}

void WRVoiceCommand::Init(const EWRVoiceContentType InVoiceContentType, TArray<FWRSubCommandData>& subCommand, FWRVoiceEventDelegate callback)
{
	SubCommandList.Append(subCommand);
	VoiceEventCallback = callback;
	VoiceContentType = InVoiceContentType;

	if (InVoiceContentType == EWRVoiceContentType::Default)
	{
		VoiceContentDelayTime = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_DefaultDelayTime);
	}
	else if (InVoiceContentType == EWRVoiceContentType::SelectDialogue)
	{
		VoiceContentDelayTime = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_SelectDialogueTime);
	}
	else
	{
		VoiceContentDelayTime = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_ContentDelayTime);
	}

	SilenceHoldingTime = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_SilenceHoldingTime);
	VoiceEventCallback = callback;
	VoiceContentType = InVoiceContentType;
	NoiseCheckTime = WRConsoleManager::Get()->GetFloat(EWRConsole::Voice_NoiseVolumeCheckTime);

	SetVoiceRecording(EWRVoiceRecordingStateType::DELAY);
	VoiceRecordingUseType = EWRVoiceRecordingUseType::COMMAND;
}

void WRVoiceCommand::InitValue()
{
	SilenceHoldingTimer = -1.0f;
}


void WRVoiceCommand::Begin()
{
	InitValue();

	if (WRSFXManager::IsValid())
		WRSFXManager::Get()->Spawn2D(TEXT("SFX/VoiceRecognition_Start_Cue.VoiceRecognition_Start_Cue"));

	FString Text = FString::Printf(TEXT("### StartRecording ### %s"), *FDateTime::Now().ToString());
	WRInputSystemManager::Get()->DisplayAndSaveLog(Text);

	FString tmp1 = FString::Printf(TEXT("[ Set NoiseVoulume : %.2f ]"), NoiseVoulume);
	WRInputSystemManager::Get()->DisplayAndSaveLog(tmp1);

	SetVoiceRecording(EWRVoiceRecordingStateType::RECORDING);
}

void WRVoiceCommand::Tick(float DeltaTime, EVoiceCaptureState::Type CaptureState, TArray<uint8>& OutVoiceBuffer, uint32& OutAvailableVoiceData, float& VoiceCaptureFinalVolume)
{
	if (VoiceRecordingState == EWRVoiceRecordingStateType::VOICESENDING)
		return;

	WRVoiceBase::Tick(DeltaTime, CaptureState, OutVoiceBuffer, OutAvailableVoiceData, VoiceCaptureFinalVolume);

	if (bWaitRecording)
		return;

	if (SilenceHoldingTimer >= SilenceHoldingTime)
	{
		FString Text = FString::Printf(TEXT("### SilenceHolding VoiceCaptureStop ### [ start : %s ] [ end : %s ]"), *SilenceStartDateTime, *FDateTime::Now().ToString());
		WRInputSystemManager::Get()->DisplayAndSaveLog(Text);

		End();
	}

}

void WRVoiceCommand::End(bool bCancel)
{
	if(WRSFXManager::IsValid())
		WRSFXManager::Get()->Spawn2D(TEXT("SFX/VoiceRecognition_End_Cue.VoiceRecognition_End_Cue"));

	VoiceRecordingUseType = EWRVoiceRecordingUseType::NONE;

	if (bCancel)
	{
		bFinish = true;
		FailedVoiceCapture();
		return;
	}

	if (!ConvertVoiceCaptureBufferToWav())
	{
		bFinish = true;
		FailedVoiceCapture(EWRVoiceWordType::EmptyVoice);
		return;
	}
}


void WRVoiceCommand::FailedVoiceCapture(EWRVoiceWordType InEventType)
{
	SetVoiceRecording(EWRVoiceRecordingStateType::NONE);

	if (VoiceContentType == EWRVoiceContentType::Default)
	{
		WRInputSystemManager::Get()->SetInputVoiceEvent(InEventType, INVALID_TABLE_TID);
	}
	else
	{
		if (VoiceEventCallback.IsBound())
		{
			VoiceEventCallback.ExecuteIfBound(INVALID_TABLE_TID);
			//SetVoiceRecording(EWRVoiceRecordingStateType::NONE);
		}
	}
}

void WRVoiceCommand::RequestGoogleSTT(FString pcmdata)
{
	SetVoiceRecording(EWRVoiceRecordingStateType::VOICESENDING);

	if (GoogleSTTRequstDelegate.IsBound())
	{
		GoogleSTTRequstDelegate.ExecuteIfBound(pcmdata, GoogleSTTRequstCnt);
	}
}

void WRVoiceCommand::OnResponseGoogleSTT(FString transcript)
{
	VoiceRecognition(transcript);

	bFinish = true;
}


void WRVoiceCommand::VoiceRecognition(FString transcript)
{
	if (transcript.IsEmpty() && SubCommandList.Num() <= 0)
	{
		WRInputSystemManager::Get()->SetInputVoiceEvent(EWRVoiceWordType::Unknown, INVALID_TABLE_TID);
		return;
	}
	else if (transcript.IsEmpty())
	{
		if (VoiceEventCallback.IsBound())
		{
			VoiceEventCallback.ExecuteIfBound(INVALID_TABLE_TID);
			WRVoiceBase::SetVoiceRecording(EWRVoiceRecordingStateType::NONE);
			return;
		}
	}

	FString Order = transcript;
	Order.RemoveSpacesInline();

	FString Text;

	WRTableID findTid = 0;

	UDataTable* VoiceRevisionTable = WRTableManager::Get()->GetTable<FWRVoiceRevisionData>();
	if (VoiceRevisionTable == nullptr)
	{
		Text = FString::Printf(TEXT("[FVoiceRecognition::Run] VoiceRevisionTable == nullptr"));
		WRInputSystemManager::Get()->DisplayAndSaveLog(Text);
	}

	const TMap<FName, uint8*>& VoiceRevisionData = VoiceRevisionTable->GetRowMap();
	for (auto& Data : VoiceRevisionData)
	{
		FWRVoiceRevisionData* RevisionData = reinterpret_cast<FWRVoiceRevisionData*>(Data.Value);
		if (RevisionData == nullptr)
		{
			continue;
		}

		FString orgOrder = Order;
		FString SimilarWord = RevisionData->SimilarWord.ToString();
		SimilarWord.RemoveSpacesInline();
		FString CorrectionWord = RevisionData->CorrectionWord.ToString();
		CorrectionWord.RemoveSpacesInline();

		if (Order.ReplaceInline(*SimilarWord, *CorrectionWord) > 0)
		{
			Text = FString::Printf(TEXT("[ TID : %s ] [ orgOrder : %s ] [ SimilarWord : %s => CorrectionWord : %s ] [ reusltOrder : %s ]"),
				*Data.Key.ToString(), *orgOrder, *RevisionData->SimilarWord.ToString(), *RevisionData->CorrectionWord.ToString(), *Order);
			WRInputSystemManager::Get()->DisplayAndSaveLog(Text);
		}
	}

	if (SubCommandList.Num() > 0)
	{
		for (auto& subCommand : SubCommandList)
		{
			FString nounWord = subCommand.NounWord;
			nounWord.RemoveSpacesInline();
			FString verbWord = subCommand.VerbWord;
			verbWord.RemoveSpacesInline();

			if (FName(*verbWord) == NAME_None && Order.Contains(nounWord))
			{
				findTid = subCommand.Tid;
				break;
			}
			else if (Order.Contains(nounWord) && Order.Contains(verbWord))
			{
				findTid = subCommand.Tid;
				break;
			}
		}

		if (VoiceEventCallback.IsBound())
		{
			VoiceEventCallback.ExecuteIfBound(findTid);
		}

		SetVoiceRecording(EWRVoiceRecordingStateType::NONE);

		return;
	}
	else
	{
		UDataTable* VoiceCommandTable = WRTableManager::Get()->GetTable<FWRVoiceCommandData>();
		if (VoiceCommandTable == nullptr)
		{
			Text = FString::Printf(TEXT("[FVoiceRecognition::Run] VoiceCommandTable == nullptr"));
			WRInputSystemManager::Get()->DisplayAndSaveLog(Text);
		}

		const TMap<FName, uint8*>& VoiceCommandData = VoiceCommandTable->GetRowMap();
		for (auto& iter : VoiceCommandData)
		{
			FWRVoiceCommandData* CommandData = reinterpret_cast<FWRVoiceCommandData*>(iter.Value);
			if (CommandData == nullptr)
			{
				continue;
			}

			if (Order.Contains(CommandData->NounWord.ToString()))
			{
				if (CommandData->VerbWord == NAME_None || Order.Contains(CommandData->VerbWord.ToString()))
				{
					int32 Key = Order.Find(CommandData->NounWord.ToString());
					EWRVoiceWordType Value = CommandData->WordType;
					int32 TID = FCString::Atoi(*(iter.Key.ToString()));
					Text = FString::Printf(TEXT("NounWord : %s, WordType : %d"),
						*CommandData->NounWord.ToString(), (uint8)CommandData->WordType);
					WRInputSystemManager::Get()->DisplayAndSaveLog(Text);

					WRInputSystemManager::Get()->SetInputVoiceEvent(CommandData->WordType, TID);
					return;
				}
			}
		}
	}

	WRInputSystemManager::Get()->SetInputVoiceEvent(EWRVoiceWordType::Unknown, INVALID_TABLE_TID);
}
