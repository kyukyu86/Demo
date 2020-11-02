#pragma once

#include "PlatformFilemanager.h"

#include "Enum/EWRTriggerEnum.h"

#include "../Node/WRTriggerBaseNode.h"
#include "../Node/Trigger/WRTriggerNodeBase.h"
#include "../Node/Action/WRTriggerActionNodeBase.h"
#include "../Node/Transition/WRTriggerTransitionNodeBase.h"

#include "../Node/Transition/WRTriggerTransitionNode_Active.h"
#include "../Node/Transition/WRTriggerTransitionNode_Deactive.h"
#include "../Node/Transition/WRTriggerTransitionNode_Enter.h"
#include "../Node/Transition/WRTriggerTransitionNode_Exit.h"
#include "../Node/Transition/WRTriggerTransitionNode_Etc.h"
#include "../Node/Trigger/WRTriggerNode_None.h"
#include "../Node/Trigger/WRTriggerNode_Box.h"
#include "../Node/Trigger/WRTriggerNode_Sphere.h"
#include "../Node/Special/WRTriggerSpecialNode_Watch.h"
#include "../Node/Trigger/WRTriggerNode_MainAreaBox.h"
#include "../Node/Trigger/WRTriggerNode_SubAreaBox.h"
#include <JsonReader.h>
#include <FileHelper.h>
#include "../Node/Transition/WRTriggerTransitionNode_Loop.h"

class WR_API WRTriggerParseHelper
{
public:

	/**
	* [2017-1-4 : magedoga]
	*  @ FileName : Path + Name
	*  @ InJsonString : Json String Buffer
	*/
	static TSharedRef<TJsonReader<TCHAR>> CreateJsonReader(const FString& FileName)
	{
		FString JsonString;
		ReadJsonString(FileName, JsonString);
		return FJsonStringReader::Create(JsonString);
	}

	static bool TryGetNodeFromJsonObject(TSharedPtr<FJsonObject>& InJsonObject, const FString& InFieldName, const TSharedPtr<FJsonObject>* OutJsonObject)
	{
		return InJsonObject->TryGetObjectField(InFieldName, OutJsonObject);
	}

	static bool TriggerFileExist(const FString& FileName)
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		return PlatformFile.FileExists(*FileName);
	}


private:
	static bool ReadJsonString(const FString& FileName, FString& OutJsonString)
	{
		if (TriggerFileExist(FileName) == false)
		{
			UE_LOG(WRTriggerLog, Error, TEXT("[WRTriggerParseHelper] "));
			return false;
		}

		FFileHelper::LoadFileToString(OutJsonString, *FileName);
		return true;
	}
};



class WR_API WRTriggerNodeFactory
{
public:
	static WRTriggerBaseNode* CreateNodeFromJsonObject(const TSharedPtr<FJsonObject>* InJsonObject/*, FWRTriggerNodeCreateResult& OutResult*/)
	{
		if (InJsonObject == nullptr)
			return nullptr;

		// [2017-1-9 : magedoga] Parse NodeType
		FString Buffer;
		if ((*InJsonObject)->TryGetStringField(TEXT("nodetype"), Buffer) == false)
			return nullptr;

		// [2017-1-9 : magedoga] Node Type
		EWRTriggerNodeType::en NodeType = EWRTriggerNodeType::GetEnum(Buffer);
		if (EWRTriggerNodeType::IsVaild(NodeType) == false)
			return nullptr;


		// [2017-1-9 : magedoga] CreateNode by NodeKind
		WRTriggerBaseNode* ResultNode = nullptr;

		// ActionNode
		if (EWRTriggerNodeType::IsActionNode(NodeType) == true)
		{
			ResultNode = new WRTriggerActionNodeBase((*InJsonObject)->Values);
		}
		// TransitionNode
		else if (EWRTriggerNodeType::IsTransitionNode(NodeType) == true)
		{
			// Transition의 종류에 맞게 생성
			switch (NodeType)
			{
				case EWRTriggerNodeType::Transition_Active:
				{ResultNode = new WRTriggerTransitionNode_Active(); } break;
				case EWRTriggerNodeType::Transition_Deactive:
				{ResultNode = new WRTriggerTransitionNode_Deactive(); } break;
				case EWRTriggerNodeType::Transition_Enter:
				{ResultNode = new WRTriggerTransitionNode_Enter(); } break;
				case EWRTriggerNodeType::Transition_Exit:
				{ResultNode = new WRTriggerTransitionNode_Exit(); } break;
				case EWRTriggerNodeType::Transition_Etc:
				{ResultNode = new WRTriggerTransitionNode_Etc(); } break;
				case EWRTriggerNodeType::Transition_Loop:
				{ResultNode = new WRTriggerTransitionNode_Loop(); } break;
// 				case EWRTriggerNodeType::Transition_Converter:
// 				{ResultNode = new WRTriggerTransitionNode_Converter(); } break;
			}
		}
		// TriggerNode
		else  if (EWRTriggerNodeType::IsTriggerNode(NodeType) == true)
		{
			// TriggerNode의 종류에 맞게 생성
			switch (NodeType)
			{
				case EWRTriggerNodeType::Trigger_None:
				{ResultNode = new WRTriggerNode_None(); } break;
				case EWRTriggerNodeType::Trigger_Box:
				{ResultNode = new WRTriggerNode_Box(); } break;
				case EWRTriggerNodeType::Trigger_Sphere:
				{ResultNode = new WRTriggerNode_Sphere(); } break;
				case EWRTriggerNodeType::Trigger_MainAreaBox:
				{ResultNode = new WRTriggerNode_MainAreaBox(); } break;
				case EWRTriggerNodeType::Trigger_SubAreaBox:
				{ResultNode = new WRTriggerNode_SubAreaBox(); } break;
			}
		}
		// SpecialNode
		else
		{
			switch (NodeType)
			{
				case EWRTriggerNodeType::Special_Watch:
				{ResultNode = new WRTriggerSpecialNode_Watch(); } break;
			}
		}

		if (ResultNode != nullptr)
		{
			ResultNode->ImportDataFromJson(InJsonObject);
		}

		return ResultNode;
	}

};