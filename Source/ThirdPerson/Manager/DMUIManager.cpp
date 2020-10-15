// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUIManager.h"
#include "DMPathManager.h"
#include "../GameInstance/DMGameInstance.h"
#include "../Data/CustomData/DMWidgetTable.h"

#define DEF_WIDGET_TABLE_PATH TEXT("/Game/Data/UI/WidgetTable.WidgetTable")


//====================================================================================
//	WidgetInfo
//====================================================================================

FDMOpenWidgetInfo::FDMOpenWidgetInfo(EDMPanelKind IN InPanelKind, FTransform IN InTransform /*= FTransform::Identity*/, EDMWidgetComponentFlag IN InAdditionalFlags /*= EDMWidgetComponentFlag::None*/)
{
	if (DMUIManager::Get())
	{
		const FDMWidgetTable* FoundWidgetData = DMUIManager::Get()->GetWidgetCreationTable(InPanelKind);
		if (FoundWidgetData != nullptr)
		{
			PanelKind = InPanelKind;
			bIs3DWidget = FoundWidgetData->bIs3DWidget;
			WidgetComponentFlags |= (EDMWidgetComponentFlag)FoundWidgetData->Flags;
			WidgetComponentFlags |= InAdditionalFlags;
			Transform.SetLocation(InTransform.GetLocation() + FoundWidgetData->OffsetTransform.GetLocation());
			Transform.SetRotation(InTransform.GetRotation() + FoundWidgetData->OffsetTransform.GetRotation());
			Transform.SetScale3D(InTransform.GetScale3D() * FoundWidgetData->OffsetTransform.GetScale3D());
		}
	}
}

FDMOpenWidgetInfo::FDMOpenWidgetInfo(FString IN InWidgetPath, FTransform IN InStandardTransform /*= FTransform::Identity*/, FTransform IN InAddTransform /*= FTransform::Identity*/, bool IN InIs3DWidget /*= false*/, EDMWidgetComponentFlag IN InAdditionalFlags /*= EDMWidgetComponentFlag::None*/)
{
	WidgetPath = InWidgetPath;
	bIs3DWidget = InIs3DWidget;
	WidgetComponentFlags = InAdditionalFlags;

	Transform.SetLocation(InStandardTransform.GetLocation() + InAddTransform.GetLocation());
	Transform.SetRotation(InStandardTransform.GetRotation() + InAddTransform.GetRotation());
	Transform.SetScale3D(InStandardTransform.GetScale3D() * InAddTransform.GetScale3D());
}

//====================================================================================
//	Manager
//====================================================================================

void DMUIManager::OnInit()
{
	LoadWidgetTable();
}

void DMUIManager::OnShutdown()
{
	CloseWidgetAll();

	if (WidgetTable->IsValidLowLevel() && WidgetTable->IsRooted())
	{
		WidgetTable->RemoveFromRoot();
	}

	if (strAsyncKey_WidgetTable.IsEmpty() == false && DMAsyncLoadManager::Get())
	{
		DMAsyncLoadManager::Get()->CancelAsyncLoad(strAsyncKey_WidgetTable);
		strAsyncKey_WidgetTable.Empty();
	}
}

void DMUIManager::CloseWidgetAll()
{
	TDoubleLinkedList<FDMWidgetData>::TDoubleLinkedListNode* Node = WidgetDataList.GetTail();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		FDMWidgetData& WidgetData = Node->GetValue();
		if (WidgetData.WidgetComponent)
		{
			WidgetData.WidgetComponent->SetActive(false);
			WidgetData.WidgetComponent->RemoveFromRoot();
			WidgetData.WidgetComponent->UnregisterComponent();
			WidgetData.WidgetComponent->DestroyComponent();
		}
		else
		{
			WidgetData.Widget->RemoveFromRoot();
			WidgetData.Widget->RemoveFromViewport();
		}
	}
	WidgetDataList.Empty();
}

void DMUIManager::LoadWidgetTable()
{
	FDMCompleteAsyncLoad NewDelegate = FDMCompleteAsyncLoad::CreateLambda([=](UObject* IN InTable, FString IN InKey)
	{
		if (InTable->IsValidLowLevel())
		{
			TAssetPtr<UDataTable> NewTable(FSoftObjectPath(DEF_WIDGET_TABLE_PATH));
			if (NewTable.IsValid() == false)
			{
				checkf(false, TEXT("Invalid Table Path : %s"), DEF_WIDGET_TABLE_PATH);
			}
			WidgetTable = NewTable;
			WidgetTable.Get()->AddToRoot();
		}
		strAsyncKey_WidgetTable.Empty();
	});
	strAsyncKey_WidgetTable = DMAsyncLoadManager::Get()->AsyncLoadAsset(DEF_WIDGET_TABLE_PATH, NewDelegate);
	if (strAsyncKey_WidgetTable == ASYNCLOADMANAGER_INVALID)
	{
		checkf(false, TEXT("Invalid Table Path : %s"), DEF_WIDGET_TABLE_PATH);
	}
}

FString DMUIManager::OpenPanel(FDMOpenWidgetInfo IN InWidgetInfo)
{
	FString strWidgetPath = "";
	
	if (InWidgetInfo.PanelKind == EDMPanelKind::None)
	{
		if (IsAsyncLoadingWidget(InWidgetInfo.WidgetPath))
			return "";

		strWidgetPath = InWidgetInfo.WidgetPath;
	}
	else
	{
		if (IsAsyncLoadingPanel(InWidgetInfo.PanelKind))
			return "";

		if (IsOpenedPanel(InWidgetInfo.PanelKind))
			return "";

		strWidgetPath = DMPathManager::Get()->GetUIPanelPath(InWidgetInfo.PanelKind);
	}

	auto AsyncComplete = FDMCompleteAsyncLoad::CreateLambda([&](UObject* InObject, FString InKey)
	{
		FDMOpenWidgetInfo* FoundInfo = AsyncList.Find(InKey);
		if (FoundInfo == nullptr)
			return;

		UUserWidget* UserWidget = Cast<UUserWidget>(InObject);
		if (UserWidget == nullptr)
			return;

		UDMWidgetComponentBase* CreatedWidgetComponent = nullptr;
		if (FoundInfo->bIs3DWidget)
		{
			if (CreateWidgetComponent<UDMWidgetComponentBase>(CreatedWidgetComponent, UserWidget))
			{
				CreatedWidgetComponent->SetWorldTransform(FoundInfo->Transform);
				CreatedWidgetComponent->SetFlag(FoundInfo->WidgetComponentFlags);

				// + Desired Size 적용하기
				CreatedWidgetComponent->SetDrawAtDesiredSize(true);
			}
		}
		else
		{
			UDMUIPanel* CastedPanel = Cast<UDMUIPanel>(UserWidget);
			if (CastedPanel == nullptr)
				return;

			CastedPanel->AddToViewport();
		}

		FDMWidgetData NewWidgetData;
		NewWidgetData.PanelKind = FoundInfo->PanelKind;
		NewWidgetData.Widget = UserWidget;
		NewWidgetData.WidgetComponent = CreatedWidgetComponent;
		WidgetDataList.AddTail(NewWidgetData);

		// Complete Delegate
		FoundInfo->CompleteDelegate.ExecuteIfBound(InObject, InKey);

		AsyncList.Remove(InKey);
	});

	FString strAsyncKey = DMAsyncLoadManager::Get()->AsyncCreateWidget(strWidgetPath, AsyncComplete);
	AsyncList.Add(strAsyncKey, InWidgetInfo);
	return strAsyncKey;
}

FString DMUIManager::OpenPanel(const EDMPanelKind IN InKind)
{	
	FDMOpenWidgetInfo TempInfo(InKind);
	return OpenPanel(TempInfo);
}

void DMUIManager::ClosePanel(const EDMPanelKind IN InPanelKind)
{
	// Check Async List
	for (auto& AsyncElement : AsyncList)
	{
		if (AsyncElement.Value.PanelKind == InPanelKind)
		{
			DMAsyncLoadManager::Get()->CancelAsyncLoad(AsyncElement.Key);
			AsyncList.Remove(AsyncElement.Key);
			return;
		}
	}

	// Check Panel List
	TDoubleLinkedList<FDMWidgetData>::TDoubleLinkedListNode* Node = WidgetDataList.GetTail();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		FDMWidgetData& WidgetData = Node->GetValue();
		if (WidgetData.PanelKind == InPanelKind)
		{
			if (WidgetData.WidgetComponent)
			{
				WidgetData.WidgetComponent->SetActive(false);
				WidgetData.WidgetComponent->RemoveFromRoot();
				WidgetData.WidgetComponent->UnregisterComponent();
				WidgetData.WidgetComponent->DestroyComponent();
			}
			else
			{
				WidgetData.Widget->RemoveFromRoot();
				WidgetData.Widget->RemoveFromViewport();
			}

			WidgetDataList.RemoveNode(Node);
			return;
		}
	}
}

void DMUIManager::CloseWidget(UUserWidget* IN InUserWidget)
{	
	TDoubleLinkedList<FDMWidgetData>::TDoubleLinkedListNode* Node = WidgetDataList.GetTail();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		FDMWidgetData& WidgetData = Node->GetValue();
		if (WidgetData.Widget != InUserWidget)
			continue;

		if (WidgetData.WidgetComponent)
		{
			WidgetData.WidgetComponent->SetActive(false);
			WidgetData.WidgetComponent->RemoveFromRoot();
			WidgetData.WidgetComponent->UnregisterComponent();
			WidgetData.WidgetComponent->DestroyComponent();
		}
		else
		{
			WidgetData.Widget->RemoveFromRoot();
			WidgetData.Widget->RemoveFromViewport();
		}

		WidgetDataList.RemoveNode(Node);
		return;
	}
}

bool DMUIManager::IsOpenedPanel(const EDMPanelKind IN InKind)
{
	TDoubleLinkedList<FDMWidgetData>::TDoubleLinkedListNode* Node = WidgetDataList.GetTail();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		FDMWidgetData& WidgetData = Node->GetValue();
		if (WidgetData.PanelKind == InKind)
			return true;;
	}

	return false;
}

bool DMUIManager::IsAsyncLoadingPanel(const EDMPanelKind IN InKind)
{
	for (auto Data : AsyncList)
	{
		if (Data.Value.PanelKind == InKind)
			return true;
	}
	return false;
}

bool DMUIManager::IsAsyncLoadingWidget(const FString& IN InWidgetPath)
{
	for (auto Data : AsyncList)
	{
		if (Data.Value.WidgetPath == InWidgetPath)
			return true;
	}
	return false;
}

UUserWidget* DMUIManager::CreateUISync(FString IN InPath)
{
	UUserWidget* pWidget = nullptr;
	FString strPath = "/Game/Blueprints/UI/Slot/";
	strPath += InPath + "." + InPath + "_C";
	FStringClassReference UIClassReference(*strPath);
	UClass* UIClass = nullptr;
	if (UIClassReference.TryLoadClass<UUserWidget>() != nullptr)
	{
		UIClass = UIClassReference.ResolveClass();
	}
	else
	{
		return nullptr;
	}
	if (UIClass)
	{
		pWidget = CreateWidget<UUserWidget>(UDMGameInstance::GetGameInstance()->GetWorld(), UIClass);
		if (pWidget == nullptr)
			return nullptr;
	}
	return pWidget;
}

UUserWidget* DMUIManager::CreateUISyncFullPath(FString IN InPath)
{
	UUserWidget* pWidget = nullptr;
	FString strPath = InPath;
	FStringClassReference UIClassReference(*strPath);
	UClass* UIClass = nullptr;
	if (UIClassReference.TryLoadClass<UUserWidget>() != nullptr)
	{
		UIClass = UIClassReference.ResolveClass();
	}
	else
	{
		return nullptr;
	}
	if (UIClass)
	{
		pWidget = CreateWidget<UUserWidget>(UDMGameInstance::GetGameInstance()->GetWorld(), UIClass);
		if (pWidget == nullptr)
			return nullptr;
	}
	return pWidget;
}

FString DMUIManager::CreateUIASyncFullPath(FString& IN InFullPath, const FDMCompleteAsyncLoad IN InDelegate)
{
	return DMAsyncLoadManager::Get()->AsyncCreateWidget(InFullPath, InDelegate);
}

UUserWidget* DMUIManager::CreateUISyncClass(UClass* InClass)
{
	UUserWidget* pWidget = nullptr;
	if (InClass)
	{
		pWidget = CreateWidget<UUserWidget>(UDMGameInstance::GetGameInstance()->GetWorld(), InClass);
		if (pWidget == nullptr)
			return nullptr;
	}
	return pWidget;
}

const FDMWidgetTable* DMUIManager::GetWidgetCreationTable(const EDMPanelKind IN InPanelKind)
{
	if (WidgetTable->IsValidLowLevel() == false)
		return nullptr;

	FDMWidgetTable* FoundTable = nullptr;

	TArray<FDMWidgetTable*> TempArray;
	WidgetTable->GetAllRows<FDMWidgetTable>(FString(""), TempArray);
	if (TempArray.Num() == 0)
		return FoundTable;

	for (int32 i = 0; i < TempArray.Num(); ++i)
	{
		FDMWidgetTable* pTable = TempArray[i];
		if (pTable == nullptr)
			continue;

		if (pTable->PanelKind == InPanelKind)
		{
			FoundTable = pTable;
			break;
		}
	}

	return FoundTable;
}