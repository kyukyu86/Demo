// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#include "WRUIManager.h"

#include "WRUIPathManager.h"
#include "WRCharacterManager.h"
#include "WRAsyncLoadUObjectManager.h"
#include "WRConsoleManager.h"
#include "WRInputSystemManager.h"

#include "WidgetComponent/WRWidgetComponentBase.h"
#include "UI/Base/WRUIPanel.h"
#include "UI/Panel/WRUIPanel_Video.h"
#include "UI/Panel/WRUIPanel_DisplaySkillCoolTime.h"
#include "UI/Base/WRUISlot.h"
#include "UI/Panel/WRUIPanel_Menu.h"
#include "UI/Panel/WRUIPanel_MultiTargetUI.h"
#include "UI/Panel/WRUIPanel_Debug.h"

// [ 2019-9-9 : kyu ] Stereo Layer
#include <Engine/TextureRenderTarget2D.h>
#include <Engine/Texture.h>
#include <Engine/StreamableManager.h>
#include <Engine/AssetManager.h>
#include <Components/StereoLayerComponent.h>
#include "Component/Custom/WRUIStereoLayerComponent.h"

#include "Actor/Character/WRCharacterMine.h"
#include "Slate/WidgetRenderer.h"
#include "WidgetBlueprintLibrary.h"
#include "Core/WRGameInstance.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "CustomAsset/WRMenuAsset.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Table/Custom/WRWidgetTable.h"
#include "Engine/PostProcessVolume.h"


#define DEF_WIDGET_TABLE_PATH TEXT("/Game/Asset/Data/Table/Custom/WidgetTable.WidgetTable")


void WRUIManager::OnInitialize()
{
	// [ 2019-8-28 : vuvle ] : OpenUI 예제코드

	/*FTransform a;
	FWRUILoadCompletedDelegate a2;
	a2.BindRaw(this, &WRUIManager::OnTestUIOpened, 1);
	OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::Main, a, 0, &a2);*/
	//GenerateUIHandle(EWRUIPanelKind::Status);

	// Load Widget Table
	LoadWidgetTable();

	// [ 2020-8-12 : vuvle ] : 현재 시점에서 Stereo Layer 를 따로 만들 이유가 없기 때문에(Widget 을 Create 한 후 WidgetComponent 를 거치지 않고 만들때 사용할 수 있지만 현재 문제가 있어 더이상 사용하지 않는다)
//	InitializeStereoLayer();
}

void WRUIManager::OnShutdown()
{
	// [ 2020-5-15 : kyu ] New Widget Table
	if (WidgetTable->IsValidLowLevel() && WidgetTable->IsRooted())
	{
		WidgetTable->RemoveFromRoot();
	}
	//if (DynamicMaterial->IsValidLowLevel() && DynamicMaterial->IsRooted())
	//{
	//	DynamicMaterial->RemoveFromRoot();
	//}
	if (LoadWidgetTableHash.IsEmpty() == false && WRAsyncLoadUObjectManager::Get())
	{
		WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(LoadWidgetTableHash);
		LoadWidgetTableHash.Empty();
	}
	
#ifdef USE_STEREOLAYER_IN_UIMANAGER
	if (WidgetRenderer)
	{
		delete WidgetRenderer;
		WidgetRenderer = nullptr;
	}
#endif // USE_STEREOLAYER_IN_UIMANAGER

	// vuvle TODO: Async Load List 를 모두 날려주는 코드를 만들어야함.
	Clear();
}

void WRUIManager::OnLoadLevelStart()
{
	Clear();
}

bool WRUIManager::CreateStereoLayerComponent(class UWRUIStereoLayerComponent*& OUT OutStereoLayerComponent, UWRUIPanel* IN InUIPanelToLink, const FWRWidgetInfo& IN InWidgetInfo)
{
#if 0
	if (InUIPanelToLink == nullptr)
	{
		ensureMsgf(false, TEXT("Widget must be created before stereolayer component create."));
		return false;
	}

	if (InWidgetInfo.WidgetTable == nullptr)
		return false;

	OutStereoLayerComponent = NewObject<UWRUIStereoLayerComponent>();

	if (OutStereoLayerComponent == nullptr)
		return false;

	if (OutStereoLayerComponent->IsRegistered() == false)
	{
		UWRGameInstance* GameInstance = UWRGameInstance::GetGameInstance();
		if (GameInstance == nullptr)
			return false;

		UWorld* World = GameInstance->GetWorld();
		if (World == nullptr)
			return false;

		OutStereoLayerComponent->RegisterComponentWithWorld(World);
	}

	OutStereoLayerComponent->AddToRoot();
	OutStereoLayerComponent->SetActive(true);
	if (InWidgetInfo.GetFlags() & EWRUIWidgetCreationBitFlag::UseFaceLock)
	{
		OutStereoLayerComponent->SetStereoLayerType(EStereoLayerType::SLT_FaceLocked);
	}
	else if (InWidgetInfo.GetFlags() & EWRUIWidgetCreationBitFlag::UserWorldLock)
	{
		OutStereoLayerComponent->SetStereoLayerType(EStereoLayerType::SLT_WorldLocked);
	}
	else if (InWidgetInfo.GetFlags() & EWRUIWidgetCreationBitFlag::UserTrackerLock)
	{
		OutStereoLayerComponent->SetStereoLayerType(EStereoLayerType::SLT_TrackerLocked);
	}
	
	OutStereoLayerComponent->bLiveTexture = true;
	
// 	TSharedPtr<SWidget> aa = InUIPanelToLink->TakeWidget();
// 	InUIPanelToLink->AddToRoot();
	//WidgetRenderTarget = WidgetRenderer->DrawWidget(aa.ToSharedRef(), InWidgetInfo.WidgetTable->StereoRenderSize);	// Widget 을 Render Target 에 그리고

	//WidgetRenderTarget = WidgetRenderer->DrawWidget(aa.ToSharedRef(), InWidgetInfo.WidgetTable->StereoRenderSize);	// Widget 을 Render Target 에 그리고

	//DynamicMaterial->SetTextureParameterValue("SlateUI", WidgetRenderTarget);		// Material 에 있는 SlateUI 와 Render Target 을 Binding 시키고
	//UKismetRenderingLibrary::DrawMaterialToRenderTarget(UWRGameInstance::GetGameInstance(), WidgetRenderTarget, DynamicMaterial);	// Material 을 Render Target 에 그린다.
	// 과연 Marking 시켜서 Stereo Layer 를 계속 Update 해야하는지 확인해봐야함.
	OutStereoLayerComponent->SetQuadSize(InWidgetInfo.WidgetTable->StereoRenderSize);
	OutStereoLayerComponent->SetVisibility(true);

	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar == nullptr)
		return false;

	UCameraComponent* CamComponet = Cast<UCameraComponent>(MyChar->GetComponentByClass(UCameraComponent::StaticClass()));
	if (CamComponet == nullptr)
		return false;

	FTransform TransformTemp = CamComponet->GetComponentTransform();
	//BeforeYaw = FRotator(TransformTemp.GetRotation()).Yaw;
 	OutStereoLayerComponent->SetRelativeLocation(InWidgetInfo.WidgetTable->StereoLayerLocation);
// 	OutStereoLayerComponent->SetRelativeRotation(TransformTemp.GetRotation());
//	OutStereoLayerComponent->AttachToComponent(CamComponet, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
// 	OutStereoLayerComponent->SetWorldLocation(FVector(881, -34, 245));
// 	OutStereoLayerComponent->SetRelativeRotation(FQuat(FVector::UpVector, PI));
	OutStereoLayerComponent->SetWorldScale3D(FVector(10, 10, 10));
	OutStereoLayerComponent->SetRelativeScale3D(FVector(10, 10, 10));
	//OutStereoLayerComponent->SetRotation();
//	OutStereoLayerComponent->

	OutStereoLayerComponent->MarkTextureForUpdate();
	OutStereoLayerComponent->SetTexture(Cast<UTexture>(WidgetRenderTarget));



// 	UStereoLayerComponent* ST = Cast<UStereoLayerComponent>(GetDefaultSubobjectByName(TEXT("StereoLayerSystem")));
// 	if (ST->IsRegistered() == false)
// 	{
// 		UWRGameInstance* GameInstance = UWRGameInstance::GetGameInstance();
// 		if (GameInstance == nullptr)
// 			return false;
// 
// 		UWorld* World = GameInstance->GetWorld();
// 		if (World == nullptr)
// 			return false;
// 
// 		OutStereoLayerComponent->RegisterComponentWithWorld(World);
// 	}
// 	ST->MarkTextureForUpdate();
// 	ST->SetTexture(Cast<UTexture>(WidgetRenderTarget));


	return true;

/*
	AWRCharacterMine* MyChar = WRCharacterManager::Get()->GetMyCharacter();
	if (MyChar)
	{
		UCameraComponent* MyCamera = Cast<UCameraComponent>(MyChar->GetComponentByClass(UCameraComponent::StaticClass()));
		if (MyCamera)
		{
			OutStereoLayerComponent = NewObject<UWRUIStereoLayerComponent>();
// 			if (bInEnableTick == true)
// 			{
// 				SetEnableTick(StereoLayerComponent, bInEnableTick);
// 			}
			OutStereoLayerComponent->AttachToComponent(MyCamera, FAttachmentTransformRules::KeepRelativeTransform);

			OutStereoLayerComponent->AddToRoot();
			//MyCamera->AddOwnedComponent(OutStereoLayerComponent);
			//MyCamera->AddInstanceComponent(OutStereoLayerComponent);
			if (OutStereoLayerComponent->IsRegistered() == false)
			{
				UWRGameInstance* GameInstance = UWRGameInstance::GetGameInstance();
				if (GameInstance == nullptr)
					return false;

				UWorld* World = GameInstance->GetWorld();
				if (World == nullptr)
					return false;

				OutStereoLayerComponent->RegisterComponentWithWorld(World);
			}

			TSharedPtr<SWidget> aa = InUIPanelToLink->TakeWidget();
			WidgetRenderTarget = WidgetRenderer->DrawWidget(aa.ToSharedRef(), InWidgetInfo.WidgetTable->StereoRenderSize);	// Widget 을 Render Target 에 그리고
			OutStereoLayerComponent->SetRelativeLocation(FVector(100, 100, -300));
			OutStereoLayerComponent->SetStereoLayerType(EStereoLayerType::SLT_FaceLocked);
			OutStereoLayerComponent->bLiveTexture = true;
			OutStereoLayerComponent->SetQuadSize(FVector2D(5000, 5000));
			OutStereoLayerComponent->MarkTextureForUpdate();
			OutStereoLayerComponent->SetTexture(Cast<UTexture>(WidgetRenderTarget));

			return true;
		}

	}*/
#endif
	return false;
}

bool WRUIManager::CreateUI(const EWRUIPanelKind IN InUIKindToCreate, UWRUIPanel*& OUT OutCreatedPanel)
{
	FStringClassReference UIClassReference(WRUIPathManager::Get()->GetUIPath(InUIKindToCreate));

	UClass* UIClass = UIClassReference.ResolveClass();
	if (UIClass)
	{
		UWRUIPanel* CreatedWidget = CreateWidget<UWRUIPanel>(UWRGameInstance::GetGameInstance()->GetWorld(), UIClass);
		if (CreatedWidget)
		{
			CreatedWidget->SetPanelKind(InUIKindToCreate);
			CreatedWidget->SetUIHandle(GenerateUIHandle());
		}	
		else
		{
			ensureMsgf(false, TEXT("Widget Create Failed"));
			return false;
		}
		OutCreatedPanel = CreatedWidget;
	}
	else
	{
		ensureMsgf(false, TEXT("Widget Create Failed"));
		return false;
	}
	
	return true;
}

const FWRCombinedPanelData& WRUIManager::AddUIToContainer(const EWRUIPanelKind IN InUIKind, UWRUIPanel* IN InUIPanel, UWRWidgetComponentBase* IN InWidgetComponentBase /*= nullptr*/, UWRUIStereoLayerComponent* IN InStereoLayerComponent/*= nullptr*/)
{
	FWRCombinedPanelData CombinedPanelData = FWRCombinedPanelData(InWidgetComponentBase, InUIPanel, InStereoLayerComponent, InUIPanel->GetUIHandle());

	if(InWidgetComponentBase != nullptr)
		SetComponentOptions(InUIPanel, InWidgetComponentBase);

	if (IsMultiCreatableUI(InUIKind))
		return MultipleUIMap.Add(InUIPanel->GetUIHandle(), CombinedPanelData); 
	else
		return UIMap.Add(InUIKind, CombinedPanelData);
}

void WRUIManager::SetComponentOptions(UWRUIPanel* IN InUIPanel, UWRWidgetComponentBase* IN InWidgetComponentBase)
{
	InWidgetComponentBase->SetTranslucentSortPriority(InUIPanel->GetTranslucentSortPriority());
	bool RenderCustomDepth = InUIPanel->GetRenderCustomDepth();
	if (RenderCustomDepth)
	{
		InWidgetComponentBase->SetBlendMode(EWidgetBlendMode::Masked);
		InWidgetComponentBase->SetMaterialByBlendMode(EWidgetBlendMode::Masked);
		InWidgetComponentBase->SetRenderCustomDepth(InUIPanel->GetRenderCustomDepth());
	}
}

UUserWidget* WRUIManager::CreateUISync(FString& IN InPath)
{
	UUserWidget* pWidget = nullptr;
	FString strPath = "/Game/Asset/BluePrint/UI/";
	strPath += InPath;
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
		pWidget = CreateWidget<UUserWidget>(UWRGameInstance::GetGameInstance()->GetWorld(), UIClass);
		if (pWidget == nullptr)
			return nullptr;
	}
	return pWidget;
}

UUserWidget* WRUIManager::CreateUIFullPathSync(FString& IN InFullPath)
{
	UUserWidget* pWidget = nullptr;
	FStringClassReference UIClassReference(*InFullPath);
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
		pWidget = CreateWidget<UUserWidget>(UWRGameInstance::GetGameInstance()->GetWorld(), UIClass);
		if (pWidget == nullptr)
			return nullptr;
	}
	return pWidget;
}

FString WRUIManager::CreateUIFullPathASync(FString& IN InFullPath, const FWRSlotUILoadCompletedDelegate IN InDelegate)
{
	auto OnSlotAsyncLoadCompleted = FWRCompleteAsyncLoad::CreateLambda([=](UObject* InObject, FString InKey)
	{
		InDelegate.ExecuteIfBound(Cast<UWRUISlot>(InObject), InKey);
	});

	return WRAsyncLoadUObjectManager::Get()->AsyncCreateWidget(InFullPath, OnSlotAsyncLoadCompleted);
}

void WRUIManager::CancelUIAsync(const FString& IN InHash)
{
	if (WRAsyncLoadUObjectManager::IsValid() == false)
		return;

	WRAsyncLoadUObjectManager::Get()->CancelAsyncLoad(InHash);
}

void WRUIManager::Clear()
{
#ifdef DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604
#else // DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604
	while (StreamableHandleQueue.IsEmpty() == false)
	{
		TSharedPtr<FStreamableHandle> TempHandle;
		if (StreamableHandleQueue.Dequeue(TempHandle))
		{
			TempHandle->CancelHandle();
		}
	}
	StreamableHandleQueue.Empty();
#endif // DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604
	
	SavedCreatingStateList.Empty();
	
	for (auto It = UIMap.CreateIterator(); It; ++It)
	{
		FWRCombinedPanelData& CombinedPanelData = It.Value();
		if (CombinedPanelData.IsStereoLayerExist())
		{
			CombinedPanelData.StereoLayerComponent->SetTexture(nullptr);
			CombinedPanelData.StereoLayerComponent->SetVisibility(false);
			CombinedPanelData.StereoLayerComponent->SetActive(false);
			if (CombinedPanelData.StereoLayerComponent->IsRegistered())
				CombinedPanelData.StereoLayerComponent->UnregisterComponent();
			CombinedPanelData.StereoLayerComponent->RemoveFromRoot();
			CombinedPanelData.StereoLayerComponent->DestroyComponent();
			CombinedPanelData.StereoLayerComponent = nullptr;
		}

		if (CombinedPanelData.Is3DWidget())
		{
			CombinedPanelData.WidgetComponentBase->SetActive(false);
			CombinedPanelData.WidgetComponentBase->RemoveFromRoot();
			CombinedPanelData.WidgetComponentBase->UnregisterComponent();
			CombinedPanelData.WidgetComponentBase->DestroyComponent();
		}
		else
		{
			if (CombinedPanelData.UIPanel->IsInViewport())
			{
				CombinedPanelData.UIPanel->RemoveFromRoot();
				CombinedPanelData.UIPanel->RemoveFromViewport();
			}
		}
	}
	UIMap.Empty();

	for (auto It = MultipleUIMap.CreateIterator(); It; ++It)
	{
		FWRCombinedPanelData& CombinedPanelData = It.Value();
		if (CombinedPanelData.Is3DWidget())
		{
			CombinedPanelData.WidgetComponentBase->SetActive(false);
			CombinedPanelData.WidgetComponentBase->RemoveFromRoot();
			CombinedPanelData.WidgetComponentBase->UnregisterComponent();
			CombinedPanelData.WidgetComponentBase->DestroyComponent();
		}
		else
		{
			CombinedPanelData.UIPanel->RemoveFromRoot();
			CombinedPanelData.UIPanel->RemoveFromViewport();
		}
	}
	MultipleUIMap.Empty();
}

WRUIHandle WRUIManager::GenerateUIHandle()
{
	static WRUIHandle LastUIHandleID = 0; // [ 2019-8-27 : kyu ] UI Handle
	++LastUIHandleID;
	return LastUIHandleID;
}

#if DEF_REMOVE_NOT_USED_FUNCTIONS_VUVLE_200520
WRUIHandle WRUIManager::GenerateUIHandle(const EWRUIPanelKind IN InPanelKind)
{
	static uint16 LastHandle = 0;
	if (LastHandle == 0xFFFF)
	{
		ensureMsgf(false, TEXT("UIHandle has reached over the range"));
		return 0;
	}
	
	WRUIHandle ResultHandle = (1 << (uint8(InPanelKind) + MAX_UNIQUE_HANDLE_LOWBIT_NUMBER + 1)) + (++LastHandle);
	return ResultHandle;
}
#endif

#if DEF_REMOVE_NOT_USED_FUNCTIONS_VUVLE_200520
void WRUIManager::CreateUIAsync(const EWRUIPanelKind IN InUIKindToLoad, const EWRUISpaceType IN InUISpaceType, const FWRUILoadCompletedDelegate& IN InDelegateToNotify, const bool IN bInSaveCreatingState)
{
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	FSoftObjectPath ObjectPath(WRUIPathManager::Get()->GetUIPath(InUIKindToLoad));

	auto On2DWidgetAsyncLoadCompleted = FStreamableDelegate::CreateLambda([=]()
	{
		TSharedPtr<FStreamableHandle> HandleTemp;
		StreamableHandleQueue.Dequeue(HandleTemp);

		if (bInSaveCreatingState)
			RemoveCreatingState(InUIKindToLoad, nullptr);

		UWRUIPanel* CreatedPanel = nullptr;
		if (CreateUI(InUIKindToLoad, CreatedPanel) == false)
		{
			ensureMsgf(false, TEXT("Widget Create Failed"));
			return;
		}
		CreatedPanel->AddToViewport();
		const FWRCombinedPanelData& AddedPanelData = AddUIToContainer(InUIKindToLoad, CreatedPanel);
		InDelegateToNotify.ExecuteIfBound(AddedPanelData);
	});

	auto On3DWidgetAsyncLoadCompleted = FStreamableDelegate::CreateLambda([=]()
	{
		TSharedPtr<FStreamableHandle> HandleTemp;
		StreamableHandleQueue.Dequeue(HandleTemp);

		if (bInSaveCreatingState)
			RemoveCreatingState(InUIKindToLoad, nullptr);

		UWRUIPanel* CreatedPanel = nullptr;
		if (CreateUI(InUIKindToLoad, CreatedPanel) == false)
		{
			ensureMsgf(false, TEXT("Widget Create Failed"));
			return;
		}

		const FWRCombinedPanelData& AddedPanelData = AddUIToContainer(InUIKindToLoad, CreatedPanel);
		InDelegateToNotify.ExecuteIfBound(AddedPanelData);
	});

	if (InUISpaceType == EWRUISpaceType::Space2D)
	{
		TSharedPtr<FStreamableHandle> StreamHandle = StreamableManager.RequestAsyncLoad(ObjectPath, On2DWidgetAsyncLoadCompleted, 0);
		if (StreamHandle.IsValid())
		{
			StreamableHandleQueue.Enqueue(StreamHandle);
			if (bInSaveCreatingState)
			{
				SavedCreatingStateList.AddTail(TPair<EWRUIPanelKind, const AActor*>(InUIKindToLoad, nullptr));
			}
		}
			
	}
	else if (InUISpaceType == EWRUISpaceType::Space3D)
	{
		TSharedPtr<FStreamableHandle> StreamHandle = StreamableManager.RequestAsyncLoad(ObjectPath, On3DWidgetAsyncLoadCompleted, 0);
		if (StreamHandle.IsValid())
		{
			StreamableHandleQueue.Enqueue(StreamHandle);
			if (bInSaveCreatingState)
			{
				SavedCreatingStateList.AddTail(TPair<EWRUIPanelKind, const AActor*>(InUIKindToLoad, nullptr));
			}
		}
			
	}
	else
	{
		ensureMsgf(0, TEXT("Not defined SpaceType Entered"));
		return;
	}
}

#endif


bool WRUIManager::OnTick(float InDeltaTime)
{

#ifdef USE_STEREOLAYER_IN_UIMANAGER
	// 만약에 Widget Component 가 없으면 RenderTarget 을 생성하여 그린다
	if (WidgetRenderer)

	{
		for (auto& Node : UIMap)
		{
			FWRCombinedPanelData* PanelData = &Node.Value;

			// [ 2020-7-16 : vuvle ] : Stereo Layer 가 존재하지 않으면 넘긴다
			if (PanelData->IsStereoLayerExist() == false)
				continue;

			// [ 2020-7-16 : vuvle ] : 만약에 Stereo Layer 가 존재하고 Widget Component 가 있으면 RenderTarger 을 가져온다.
			if (PanelData->Is3DWidget() && PanelData->IsStereoLayerExist())
			{
				FTransform WidgetComponentTranform = PanelData->WidgetComponentBase->GetRelativeTransform();

				FVector WidgetLocation = WidgetComponentTranform.GetLocation();
				WidgetComponentTranform.SetLocation(WidgetLocation);
				//dgetComponentTranform.SetScale3D(FVector(1.4f, 1.4f, 1.4f));
				WidgetComponentTranform.SetRotation(WidgetComponentTranform.GetRotation() + FQuat(FVector::UpVector, PI));

				// Create 할때 Material 을 바꿔줘야함
				/*PanelData->WidgetComponentBase->SetMaterialAsHidden();*/

				/*if (WidgetComponentTranform != PanelData->StereoLayerComponent->GetRelativeTransform())
				{*/
					PanelData->StereoLayerComponent->SetRelativeTransform(WidgetComponentTranform);
				/*}*/
				if (PanelData->WidgetComponentBase->GetCurrentDrawSize() != PanelData->StereoLayerComponent->GetQuadSize())
				{
					PanelData->StereoLayerComponent->SetQuadSize(PanelData->WidgetComponentBase->GetCurrentDrawSize());
				}
				//PanelData->StereoLayerComponent->SetTexture(Cast<UTexture>(PanelData->WidgetComponentBase->GetRenderTarget()));
			}
			else if(PanelData->IsStereoLayerExist())
			{
				if (PanelData->UIPanel == nullptr)
				{
					continue;
				}

				/*
				FTransform WidgetComponentTransform;
				WidgetComponentTransform.SetLocation(FVector(-100, 10, 50));
				WidgetComponentTransform.SetRotation(FQuat(0, 0, -5.96046448e-08, 5.96046448e-08));
				WidgetComponentTransform.SetScale3D(FVector(0.2, 0.3, 0.3));

				PanelData->StereoLayerComponent->SetRelativeTransform(WidgetComponentTransform);
				PanelData->StereoLayerComponent->SetQuadSize(FVector2D(10000, 10000));

				const FWRWidgetTable* WidgetCreationTable = GetWidgetCreationTable(PanelData->UIPanel->GetPanelKind());
				if (WidgetCreationTable == nullptr)
				{
					continue;
				}

 				TSharedPtr<SWidget> aa = PanelData->UIPanel->TakeWidget();
				WidgetRenderTarget = WidgetRenderer->DrawWidget(aa.ToSharedRef(), WidgetCreationTable->StereoRenderSize);	// Widget 을 Render Target 에 그리고
// 				//DynamicMaterial->SetTextureParameterValue("SlateUI", WidgetRenderTarget);		// Material 에 있는 SlateUI 와 Render Target 을 Binding 시키고
// 				//UKismetRenderingLibrary::DrawMaterialToRenderTarget(UWRGameInstance::GetGameInstance(), WidgetRenderTarget, DynamicMaterial);	// Material 을 Render Target 에 그린다.
// 				// 과연 Marking 시켜서 Stereo Layer 를 계속 Update 해야하는지 확인해봐야함.
// 				PanelData->StereoLayerComponent->SetQuadSize(WidgetCreationTable->StereoRenderSize);
// 
 				PanelData->StereoLayerComponent->MarkTextureForUpdate();
 				PanelData->StereoLayerComponent->SetTexture(Cast<UTexture>(WidgetRenderTarget));
				//PanelData->UIPanel->GetDrawSize()


				AWRCharacterMine* pMine = WRCharacterManager::Get()->GetMyCharacter();
				if (pMine == nullptr)
					return true;

				UStereoLayerComponent* pStereoLayer = pMine->GetStereoLayerComponent();
				if (pStereoLayer == nullptr)
					return true;

				pRenderTarget = pWidgetRenderer->DrawWidget(StereoSlateWidget.ToSharedRef(), pMine->GetRenderSize());
				pStereoLayer->MarkTextureForUpdate();
				pStereoLayer->SetTexture(Cast<UTexture>(pRenderTarget));*/

				
			}
			else
			{
				continue;
			}
		}
	}
#endif // USE_STEREOLAYER_IN_UIMANAGER

	return true;
}

void WRUIManager::OnDepartureEffectToMapCalled()
{
	Clear();
}

UWRUISlot* WRUIManager::CreateUISlot(const EWRUISlotKind IN InSlotKind)
{
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
	FSoftObjectPath ObjectPath(WRUIPathManager::Get()->GetUIPath(InSlotKind));

	StreamableManager.RequestSyncLoad(ObjectPath);

	FStringClassReference UIClassReference(WRUIPathManager::Get()->GetUIPath(InSlotKind));

	UClass* UIClass = UIClassReference.ResolveClass();
	if (UIClass)
	{
		UWRUISlot* CreatedWidget = CreateWidget<UWRUISlot>(UWRGameInstance::GetGameInstance()->GetWorld(), UIClass);
		return CreatedWidget;
	}
	
	return nullptr;
}

FString WRUIManager::CreateUISlotAsync(const EWRUISlotKind IN InSlotKind, const FWRSlotUILoadCompletedDelegate IN InDelegate)
{
	auto OnSlotAsyncLoadCompleted = FWRCompleteAsyncLoad::CreateLambda([=](UObject* InObject, FString InKey)
	{
		InDelegate.ExecuteIfBound(Cast<UWRUISlot>(InObject), InKey);
	});

	return WRAsyncLoadUObjectManager::Get()->AsyncCreateWidget(WRUIPathManager::Get()->GetUIPath(InSlotKind), OnSlotAsyncLoadCompleted);
}

#if DEF_REMOVE_NOT_USED_FUNCTIONS_VUVLE_200520
void WRUIManager::Open2DWidget(const EWRUIPanelKind IN InUIKind, const FWRUILoadCompletedDelegate* IN InDelegate /*= nullptr*/)
{
	if (UIMap.Contains(InUIKind))
	{
		ensureMsgf(false, TEXT("The Key Entered already Exist."));
		if (InDelegate != nullptr)
		{
			InDelegate->ExecuteIfBound(UIMap[InUIKind]);
		}
		return;
	}

	CreateUIAsync(InUIKind, EWRUISpaceType::Space2D, *InDelegate, false);
}

#endif





void WRUIManager::CloseUI(const EWRUIPanelKind IN InUIKind, const bool IN bCheckFadeOut /*= true*/)
{
	FWRCombinedPanelData* pFoundWidget = UIMap.Find(InUIKind);
	if (pFoundWidget == nullptr)
		return;

	if (bCheckFadeOut)
	{
		if (pFoundWidget->UIPanel->IsFadeOutWhenClosePanel())
		{
			if (pFoundWidget->UIPanel->IsFadingOut() == false)
				pFoundWidget->UIPanel->ClosePanelAfterFadeOut();
			return;
		}
	}

	UIMap.Remove(InUIKind);

	if (pFoundWidget->Is3DWidget())
	{
		pFoundWidget->WidgetComponentBase->SetActive(false);
		pFoundWidget->WidgetComponentBase->RemoveFromRoot();
		pFoundWidget->WidgetComponentBase->UnregisterComponent();
		pFoundWidget->WidgetComponentBase->DestroyComponent();
	}
	else
	{
		pFoundWidget->UIPanel->RemoveFromRoot();
		pFoundWidget->UIPanel->RemoveFromViewport();
	}
}

void WRUIManager::CloseUI(const WRUIHandle IN InUIHandle, const bool IN bCheckFadeOut /*= true*/)
{
	FWRCombinedPanelData* pFoundWidget = MultipleUIMap.Find(InUIHandle);
	if (pFoundWidget != nullptr)
	{
		if (bCheckFadeOut)
		{
			if (pFoundWidget->UIPanel->IsFadeOutWhenClosePanel())
			{
				if (pFoundWidget->UIPanel->IsFadingOut() == false)
					pFoundWidget->UIPanel->ClosePanelAfterFadeOut();
				return;
			}
		}

		MultipleUIMap.Remove(InUIHandle);

		if (pFoundWidget->Is3DWidget())
		{
			pFoundWidget->WidgetComponentBase->SetActive(false);
			pFoundWidget->WidgetComponentBase->RemoveFromRoot();
			pFoundWidget->WidgetComponentBase->UnregisterComponent();
			pFoundWidget->WidgetComponentBase->DestroyComponent();
		}
		else
		{
			pFoundWidget->UIPanel->RemoveFromRoot();
			pFoundWidget->UIPanel->RemoveFromViewport();
		}
	}
	else
	{
		for (auto It = UIMap.CreateIterator(); It; ++It)
		{
			FWRCombinedPanelData& CombindPanelData = It.Value();
			if(CombindPanelData.UIHandle != InUIHandle)
				continue;

			if (bCheckFadeOut)
			{
				if (CombindPanelData.UIPanel->IsFadeOutWhenClosePanel())
				{
					if (CombindPanelData.UIPanel->IsFadingOut() == false)
						CombindPanelData.UIPanel->ClosePanelAfterFadeOut();
					return;
				}
			}

			if (CombindPanelData.Is3DWidget())
			{
				CombindPanelData.WidgetComponentBase->SetActive(false);
				CombindPanelData.WidgetComponentBase->RemoveFromRoot();
				CombindPanelData.WidgetComponentBase->UnregisterComponent();
				CombindPanelData.WidgetComponentBase->DestroyComponent();
			}
			else
			{
				CombindPanelData.UIPanel->RemoveFromRoot();
				CombindPanelData.UIPanel->RemoveFromViewport();
			}

			UIMap.Remove(CombindPanelData.UIPanel->GetPanelKind());
			break;
		}
	}
}


void WRUIManager::CloseUI(const AActor* IN InCharacter, const EWRUIPanelKind IN InUIKind, const bool IN bCheckFadeOut /*= true*/)
{
	if (InCharacter == nullptr)
		return;

	for (auto It = MultipleUIMap.CreateIterator(); It; ++It)
	{
		FWRCombinedPanelData& CombindPanelData = It.Value();
		if (CombindPanelData.Is3DWidget() == false)
			continue;

		const AActor* OwnerActor = CombindPanelData.WidgetComponentBase->GetOwnerActor();
		if (OwnerActor != InCharacter)
			continue;

		if (CombindPanelData.UIPanel->GetPanelKind() != InUIKind)
			continue;

		if (bCheckFadeOut)
		{
			if (CombindPanelData.UIPanel->IsFadeOutWhenClosePanel())
			{
				if (CombindPanelData.UIPanel->IsFadingOut() == false)
					CombindPanelData.UIPanel->ClosePanelAfterFadeOut();
				return;
			}
		}

		CombindPanelData.WidgetComponentBase->SetActive(false);
		CombindPanelData.WidgetComponentBase->RemoveFromRoot();
		CombindPanelData.WidgetComponentBase->UnregisterComponent();
		CombindPanelData.WidgetComponentBase->DestroyComponent();

		It.RemoveCurrent();
		return;
	}
}

void WRUIManager::CloseAllUIFromCharacter(const AActor* IN InCharacter)
{
	if (InCharacter->IsValidLowLevel() == false)
		return;

	for (auto It = UIMap.CreateIterator(); It; ++It)
	{
		FWRCombinedPanelData& CombindPanelData = It.Value();
		if (CombindPanelData.Is3DWidget() == false)
			continue;

		const AActor* OwnerActor = CombindPanelData.WidgetComponentBase->GetOwnerActor();
		if (OwnerActor != InCharacter)
			continue;

		CombindPanelData.WidgetComponentBase->SetActive(false);
		CombindPanelData.WidgetComponentBase->RemoveFromRoot();
		CombindPanelData.WidgetComponentBase->UnregisterComponent();
		CombindPanelData.WidgetComponentBase->DestroyComponent();

		It.RemoveCurrent();
	}
	
	for (auto It = MultipleUIMap.CreateIterator(); It; ++It)
	{
		FWRCombinedPanelData& CombindPanelData = It.Value();
		if (CombindPanelData.Is3DWidget() == false)
			continue;

		const AActor* OwnerActor = CombindPanelData.WidgetComponentBase->GetOwnerActor();
		if (OwnerActor != InCharacter)
			continue;

		CombindPanelData.WidgetComponentBase->SetActive(false);
		CombindPanelData.WidgetComponentBase->RemoveFromRoot();
		CombindPanelData.WidgetComponentBase->UnregisterComponent();
		CombindPanelData.WidgetComponentBase->DestroyComponent();

		It.RemoveCurrent();
	}
}


void WRUIManager::CloseAllMultipleUI(const EWRUIPanelKind IN InUIKind)
{
	for (auto It = MultipleUIMap.CreateIterator(); It; ++It)
	{
		FWRCombinedPanelData& CombindPanelData = It.Value();
		if (CombindPanelData.Is3DWidget() == false)
			continue;

// 		const AActor* OwnerActor = CombindPanelData.WidgetComponentBase->GetOwnerActor();
// 		if (OwnerActor != InCharacter)
// 			continue;

		if (CombindPanelData.UIPanel->GetPanelKind() != InUIKind)
			continue;

		CombindPanelData.WidgetComponentBase->SetActive(false);
		CombindPanelData.WidgetComponentBase->RemoveFromRoot();
		CombindPanelData.WidgetComponentBase->UnregisterComponent();
		CombindPanelData.WidgetComponentBase->DestroyComponent();

		It.RemoveCurrent();
	}
}

void WRUIManager::HideAllWidgetComponents()
{
	bWidgetComponentVisibility = false;
	for (auto It = UIMap.CreateIterator(); It; ++It)
	{
		FWRCombinedPanelData& CombindPanelData = It.Value();
		if (CombindPanelData.Is3DWidget() == false)
			continue;

		CombindPanelData.WidgetComponentBase->SetVisibility(false);
	}

	for (auto It = MultipleUIMap.CreateIterator(); It; ++It)
	{
		FWRCombinedPanelData& CombindPanelData = It.Value();
		if (CombindPanelData.Is3DWidget() == false)
			continue;
	
		CombindPanelData.WidgetComponentBase->SetVisibility(false);
	}
}

void WRUIManager::ShowAllWidgetComponents()
{
	bWidgetComponentVisibility = true;
	for (auto It = UIMap.CreateIterator(); It; ++It)
	{
		FWRCombinedPanelData& CombindPanelData = It.Value();
		if (CombindPanelData.Is3DWidget() == false)
			continue;

		CombindPanelData.WidgetComponentBase->SetVisibility(true);
	}

	for (auto It = MultipleUIMap.CreateIterator(); It; ++It)
	{
		FWRCombinedPanelData& CombindPanelData = It.Value();
		if (CombindPanelData.Is3DWidget() == false)
			continue;

		CombindPanelData.WidgetComponentBase->SetVisibility(true);
	}
}

bool WRUIManager::IsWidgetOpened(const EWRUIPanelKind IN InUIKind)
{
	if (UIMap.Contains(InUIKind))
	{
		return true;
	}
	return false;
}

WRUIHandle WRUIManager::IsWidgetOpened(const AActor* IN InCharacter, const EWRUIPanelKind IN InUIKind)
{
	if (InCharacter == nullptr)
		return INVALID_UI_HANDLE;

	for (auto It = MultipleUIMap.CreateIterator(); It; ++It)
	{
		FWRCombinedPanelData& CombindPanelData = It.Value();
		if (CombindPanelData.Is3DWidget() == false)
			continue;

		const AActor* OwnerActor = CombindPanelData.WidgetComponentBase->GetOwnerActor();
		if (OwnerActor != InCharacter)
			continue;

		if (CombindPanelData.UIPanel->GetPanelKind() != InUIKind)
			continue;

		return CombindPanelData.UIHandle;
	}

	return INVALID_UI_HANDLE;
}

FWRCombinedPanelData* WRUIManager::FindUIData(const WRUIHandle IN InUIHandle)
{
	FWRCombinedPanelData* pFoundWidget = MultipleUIMap.Find(InUIHandle);
	return pFoundWidget;
}

FWRCombinedPanelData* WRUIManager::FindUIData(const AActor* IN InCharacter, const EWRUIPanelKind IN InUIKind)
{
	if (InCharacter == nullptr)
		return nullptr;

	for (auto It = MultipleUIMap.CreateIterator(); It; ++It)
	{
		FWRCombinedPanelData& CombindPanelData = It.Value();
		if (CombindPanelData.Is3DWidget() == false)
			continue;

		const AActor* OwnerActor = CombindPanelData.WidgetComponentBase->GetOwnerActor();
		if (OwnerActor != InCharacter)
			continue;

		if (CombindPanelData.UIPanel->GetPanelKind() != InUIKind)
			continue;

		return &CombindPanelData;
	}

	return nullptr;
}

FWRCombinedPanelData* WRUIManager::FindUIData(const EWRUIPanelKind IN InUIKind)
{
	return UIMap.Find(InUIKind);
}

const FWRWidgetTable* WRUIManager::GetWidgetCreationTable(const EWRUIPanelKind IN InUIKind)
{
	if (WidgetTable->IsValidLowLevel() == false)
		return nullptr;

	FWRWidgetTable* FoundTable = nullptr;

	TArray<FWRWidgetTable*> TempArray;
	WidgetTable->GetAllRows(FString(""), TempArray);
	if (TempArray.Num() == 0)
		return FoundTable;

	for (int32 i = 0; i < TempArray.Num(); ++i)
	{
		FWRWidgetTable* pTable = TempArray[i];
		if(pTable == nullptr)
			continue;

		if (pTable->PanelKind == InUIKind)
		{
			FoundTable = pTable;
			break;
		}
	}

	return FoundTable;
}

void WRUIManager::RemoveCreatingState(const EWRUIPanelKind IN InUIPanelKind, const AActor* IN InOwnerCharacter)
{
	TDoubleLinkedList<TPair<EWRUIPanelKind, const AActor*>>::TDoubleLinkedListNode* Node = SavedCreatingStateList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		if(Node->GetValue().Key != InUIPanelKind)
			continue;

		if(Node->GetValue().Value != InOwnerCharacter)
			continue;

		SavedCreatingStateList.RemoveNode(Node);
		break;
	}
}

void WRUIManager::LoadWidgetTable()
{
	FWRCompleteAsyncLoad NewDelegate = FWRCompleteAsyncLoad::CreateLambda([=](UObject* IN InTable, FString IN InKey)
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
		LoadWidgetTableHash.Empty();
	});
	LoadWidgetTableHash = WRAsyncLoadUObjectManager::Get()->AsyncLoadAsset(DEF_WIDGET_TABLE_PATH, NewDelegate);
	if (LoadWidgetTableHash == WRASYNCLOADMANAGER_INVALID)
	{
		checkf(false, TEXT("Invalid Table Path : %s"), DEF_WIDGET_TABLE_PATH);
	}
}

bool WRUIManager::IsWidgetCreating(const EWRUIPanelKind IN InUIPanelKind, const AActor* IN InOwnerCharacter /*= nullptr*/)
{
	TDoubleLinkedList<TPair<EWRUIPanelKind, const AActor*>>::TDoubleLinkedListNode* Node = SavedCreatingStateList.GetHead();
	for (; Node != nullptr; Node = Node->GetNextNode())
	{
		if (Node->GetValue().Key != InUIPanelKind)
			continue;

		if (InOwnerCharacter == nullptr)
			return true;

		if (Node->GetValue().Value != InOwnerCharacter)
			continue;

		return true;
	}

	return false;
}

void WRUIManager::OpenSkillCoolTimeUI(const float IN InRemainTime)
{
	if (IsWidgetOpened(EWRUIPanelKind::DisplaySkillCoolTime))
		return;
	
	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.Is3DWidget())
		{
			UWRUIPanel_DisplaySkillCoolTime* Panel_DisplaySkillCoolTime = Cast<UWRUIPanel_DisplaySkillCoolTime>(InCombinedPanelData.UIPanel);
			if (Panel_DisplaySkillCoolTime)
				Panel_DisplaySkillCoolTime->DisplayCoolTime(InRemainTime);
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::DisplaySkillCoolTime, EWRUIWidgetCreationBitFlag::AttachToCamera | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
	OpenUI<UWRWidgetComponentBase>(WidgetInfo);
	//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::DisplaySkillCoolTime, FTransform(FRotator(0.f, 180.f, 0.f), FVector(250, 0, -40), FVector::OneVector), EWRUIWidgetCreationBitFlag::AttachToCamera | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
}

void WRUIManager::EditDebugUIFlag(uint16 InFlag)
{
	if (InFlag == EWRDebugUIBitFlag::None)
	{
		CloseAllMultipleUI(EWRUIPanelKind::Debug);
		DebugUIFlags = InFlag;
	}
	else if (InFlag == EWRDebugUIBitFlag::All)
	{
		DebugUIFlags = InFlag;
	}
	else
	{
		if (DebugUIFlags & InFlag)
			DebugUIFlags &= ~InFlag;
		else
			DebugUIFlags |= InFlag;
	}
}

void WRUIManager::UpdateDebugUI(const FWRDebugUIData IN InData)
{
	if (DebugUIFlags == EWRDebugUIBitFlag::None)
	{
		CloseUI(InData.Actor, EWRUIPanelKind::Debug);
		return;
	}

	if ((DebugUIFlags != EWRDebugUIBitFlag::All) && ((DebugUIFlags & InData.Flag) == false))
	{
		CloseUI(InData.Actor, EWRUIPanelKind::Debug);
		return;
	}

	if (InData.CompareActor->IsValidLowLevel())
	{
		if (FVector::Dist(InData.Actor->GetActorLocation(), InData.CompareActor->GetActorLocation()) > InData.fDistance)
		{
			CloseUI(InData.Actor, EWRUIPanelKind::Debug);
			return;
		}
	}

	FString strDebugContents = InData.Contents;
	int32 nIndex = 0;
	for (const FString& str : InData.ArrayContents)
	{
		strDebugContents += str;
		++nIndex;
		if (nIndex < InData.ArrayContents.Num())
			strDebugContents += "\n";
	}

	if (IsWidgetOpened(InData.Actor, EWRUIPanelKind::Debug))
	{
		UWRUIPanel_Debug* CastedPanel = FindUI<UWRUIPanel_Debug>(InData.Actor, EWRUIPanelKind::Debug);
		if (CastedPanel != nullptr)
		{
			CastedPanel->SetDebugContents(strDebugContents);
		}
	}
	else
	{
		FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
		{
			if (InCombinedPanelData.Is3DWidget())
			{
				//InCombinedPanelData.WidgetComponentBase->SetOffsetScaleForFixedSize(WRConsoleManager::Get()->GetFloat(EWRConsole::CommonUI_FixedSize) * 0.0001f);
				//InCombinedPanelData.WidgetComponentBase->SetPivot(FVector2D(0.5f, 0.5f));
				UWRUIPanel_Debug* CastedPanel = FindUI<UWRUIPanel_Debug>(InData.Actor, EWRUIPanelKind::Debug);
				if (CastedPanel != nullptr)
				{
					CastedPanel->SetDebugContents(strDebugContents);
				}
			}
		});

		FWRWidgetInfo WidgetInfo(EWRUIPanelKind::Debug, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate, InData.Actor);
		WidgetInfo.Location = FVector(0, 100, 100) + InData.Offset;
		OpenUI<UWRWidgetComponentBase>(WidgetInfo);
		//WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(EWRUIPanelKind::Debug, InData.Actor, FTransform(FRotator::ZeroRotator, vLocation, FVector::OneVector), EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::AttachToCharacter | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
	}
}

//====================================================================================
// [ 2019-9-6 : kyu ] Stereo Layer
//====================================================================================
void WRUIManager::OnStereoLayerInitialized()
{
	if (WRCharacterManager::IsValid() == false)
		return;

	AWRCharacterMine* pMine = WRCharacterManager::Get()->GetMyCharacter();
	if (pMine == nullptr)
		return;

	UStereoLayerComponent* pStereoLayer = pMine->GetStereoLayerComponent();
	if (pStereoLayer == nullptr)
		return;

	//====================================================================================
	// 참고 코드
// 	UTextureRenderTarget2D* WidgetTarget = NewObject<UTextureRenderTarget2D>();
// 	WidgetTarget->Filter = TF_Bilinear;
// 	WidgetTarget->ClearColor = FLinearColor::Transparent;
// 	WidgetTarget->TargetGamma = 1;
// 	WidgetTarget->InitCustomFormat(2048, 2048, PF_B8G8R8A8, true);
// 	WidgetTarget->UpdateResource();
// 
// 	UStereoLayerComponent* StereoLayer = NewObject<UStereoLayerComponent>(Controller);
// 	StereoLayer->RelativeLocation = FVector(100.0f, 0.0f, 0.0f);
// 	StereoLayer->SetQuadSize(FVector2D(150.0f, 150.0f));
// 	StereoLayer->SetTexture(WidgetTarget);
// 	StereoLayer->RegisterComponentWithWorld(Controller->GetWorld());
	//====================================================================================

// 	CreatedSLComponent = NewObject<UStereoLayerComponent>();
// 	if (CreatedSLComponent == nullptr)
// 	{
// 		ensureMsgf(false, TEXT("Stereo Layer Component create failed."));
// 		return;
// 	}

	//CreatedSLComponent->AddToRoot();
	//CreatedSLComponent->RegisterComponent();

//	UCameraComponent* CameraComponent = Cast<UCameraComponent>(pMine->GetComponentByClass(UCameraComponent::StaticClass()));
//	CreatedSLComponent->AttachToComponent(CameraComponent, FAttachmentTransformRules::KeepRelativeTransform);
//	FVector vLocation = pMine->GetActorForwardVector() * 1000.0f;
//	CreatedSLComponent->SetRelativeLocation(vLocation);
//	CreatedSLComponent->SetActive(true);

	if (pWidgetRenderer == nullptr)
	{
		pWidgetRenderer = new FWidgetRenderer(true);
		if (pWidgetRenderer == nullptr)
			return;

		FString UIPath = TEXT("Panel/Panel_Video_WB.Panel_Video_WB_C");
		pTestWidget = CreateUISync(UIPath);

		// Set Stereo Layer
		pStereoLayer->SetQuadSize(pMine->GetQuadSize());

		StereoSlateWidget = pTestWidget->TakeWidget();
		pRenderTarget = pWidgetRenderer->DrawWidget(StereoSlateWidget.ToSharedRef(), pMine->GetRenderSize());
		pStereoLayer->MarkTextureForUpdate();
		if (pRenderTarget != nullptr)
		{
			UTexture* pTexure = static_cast<UTexture*>(pRenderTarget);
			if (pTexure != nullptr)
			{
				pStereoLayer->SetTexture(pTexure);
			}
		}
	}	
}

void WRUIManager::InitializeStereoLayer()
{
	//FString MaterialPath = TEXT("/Game/Asset/UI/Material/XR_Widget3DPassThrough_Base.XR_Widget3DPassThrough_Base");
	//UMaterialInterface* Material = LoadObject<UMaterialInterface>(NULL, *MaterialPath);

	//FString HiddenMaterialPath = TEXT("/Game/Asset/UI/Material/MaterialHideWidgetComponent");
	//UMaterialInterface* HiddenMaterial = LoadObject<UMaterialInterface>(NULL, *HiddenMaterialPath);

// 	static ConstructorHelpers::FObjectFinder<UMaterialInterface> FoundHiddenWidgetComponentMaterial(TEXT("/Game/Asset/UI/Material/MaterialHideWidgetComponent"));
// 	UMaterialInterface* DynamicHiddenWidgetMaterial = FoundHiddenWidgetComponentMaterial.Object;

	//DynamicMaterial = UMaterialInstanceDynamic::Create(Material, nullptr);
	//DynamicHiddenWidgetMaterial = UMaterialInstanceDynamic::Create(HiddenMaterial, nullptr);

	//static ConstructorHelpers::FObjectFinder<UMaterialInterface> HiddenWidgetMaterial_Finder(TEXT("/Game/Asset/UI/Material/MaterialHideWidgetComponent"));
	//HiddenWidgetMaterial = HiddenWidgetMaterial_Finder.Object;

	/*
	if (WidgetRenderer == nullptr)
	{
		WidgetRenderer = new FWidgetRenderer(true);
	}
	*/
}

#if WITH_EDITOR || PLATFORM_WINDOWS
void WRUIManager::SaveFocusedUI(UWidget* IN InFocusWidget)
{
	if (InFocusWidget == nullptr)
		return;

	UUserWidget* CastedWidget = Cast<UUserWidget>(InFocusWidget);
	if (CastedWidget == nullptr)
		return;

	if (CastedWidget->bIsFocusable == false)
		return;

	SavedFocusWidget = InFocusWidget;
}

void WRUIManager::ReleaseFocusedUI()
{
	SavedFocusWidget = nullptr;
}

void WRUIManager::FocusSavedUI()
{
	if (SavedFocusWidget == nullptr)
		return;

	if (SavedFocusWidget->IsValidLowLevel() == false)
		return;

	APlayerController* PlayerController = UWRGameInstance::GetGameInstance()->GetFirstLocalPlayerController();
	if (PlayerController == nullptr)
		return;

	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, SavedFocusWidget);
}

#endif // WITH_EDITOR || PLATFORM_WINDOWS
