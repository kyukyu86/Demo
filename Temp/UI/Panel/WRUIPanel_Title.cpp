// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRUIPanel_Title.h"	
#include "Manager/WRUIManager.h"
#include "Manager/WRSFXManager.h"
#include "Widget.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/WRGameModeLab.h"
#include "Network/IMMessageSender.h"
#include "Network/IMNetwork.h"
#include "Manager/WRMapManager.h"
#include "Core/WRGameInstance.h"
#include "EngineUtils.h"
#include "Engine/PostProcessVolume.h"
#include "Manager/WRMaterialParameterCollectionManager.h"
#include "Utility/WRUIUtility.h"
#include "Define/WRUIDefine.h"

#include <Engine/TextureRenderTarget2D.h>
#include <Engine/Texture.h>
#include "Slate/WidgetRenderer.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Image.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Enum/EWRCharacter.h"

void UWRUIPanel_Title::NativeConstruct()
{
	Super::NativeConstruct();

	SetFocusableInputMode(true, this);


	UWorld* World = UWRGameInstance::GetGameInstance()->GetWorld();
	for (TActorIterator<APostProcessVolume> Iter(World); Iter; ++Iter)
	{
		APostProcessVolume* PostProcessVolume = *Iter;
		if (PostProcessVolume == nullptr)
			continue;

		FPostProcessVolumeProperties PostProcessVolumeProperties = PostProcessVolume->GetProperties();
		FPostProcessSettings* Settings = const_cast<FPostProcessSettings*>(PostProcessVolumeProperties.Settings);
		if (Settings)
			Settings->DepthOfFieldFstop = 1;
		break; 
	}

	WRMaterialParameterCollectionManager::Get()->SetScalarParameterValue(EWRMPC::Monitor, "EmissiveValue", 0);

#if DEF_REMOVE_NOT_USED_FUNCTIONS_VUVLE_200520
	// [ 2019-12-10 : kyu ] Test
//	BeginTestMaterial();
#endif
}

void UWRUIPanel_Title::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

#if DEF_REMOVE_NOT_USED_FUNCTIONS_VUVLE_200520
//	UpdateTestMaterial(InDeltaTime);
#endif
}

void UWRUIPanel_Title::NativeDestruct()
{
	SetFocusableInputMode(false);
	bWaitLoginPacket = false;

	Super::NativeDestruct();
}

FReply UWRUIPanel_Title::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	EWRInput WRInput = WRUIUtility::ConvertFKeyToEWRInput(InKeyEvent.GetKey());
	if (AnyKeys.Contains(WRInput))
	{
		OnAnyButtonClicked();
	}

	return Super::NativeOnKeyUp(InGeometry, InKeyEvent);
}

void UWRUIPanel_Title::OnAnyButtonClicked()
{
	if (WRMapManager::Get()->IsLoading())
		return;

	if (bWaitLoginPacket == false)
	{
		WRSFXManager::Get()->Spawn2D(TEXT("Start_logo_Cue.Start_logo_Cue"));
		IMNetwork::Get()->ConnectToLoginServer();
		bWaitLoginPacket = IMMessageSender::RequestLogin();
	}

	/*AWRGameModeLab* GameModeLab = Cast<AWRGameModeLab>(UGameplayStatics::GetGameMode(this));
	if (GameModeLab)
		GameModeLab->OnAnyButtonInTitleClicked();*/
}

#if DEF_REMOVE_NOT_USED_FUNCTIONS_VUVLE_200520
//====================================================================================
// [ 2019-12-11 : kyu ] Test
void UWRUIPanel_Title::BeginTestMaterial()
{
	BIND_WIDGET(UImage, Image_Test1);
	if (Image_Test1 == nullptr)
		return;

 	if (MateraiInstance == nullptr)
 	{
 		MateraiInstance = WRUIUtility::GetDynamicMaterial(Image_Test1);
 		if (MateraiInstance != nullptr)
 		{
 			MateraiInstance->SetScalarParameterValue("UIRandom", FMath::FRandRange(0.f, 1.f));
 		}
 	}
 	if (pWidgetRenderer == nullptr)
 	{
 		pWidgetRenderer = new FWidgetRenderer(true);
 		if (pWidgetRenderer == nullptr)
 			return;
 
 		pTestWidget = WRUIManager::Get()->CreateUISync(TEXT("Test/Select.Select_C"));
 		if (pTestWidget)
 		{
 			SlateWidget = pTestWidget->TakeWidget();
 			
 			FVector2D vSize = FVector2D(500, 200);
 			
 			//pRenderTarget = pWidgetRenderer->DrawWidget(SlateWidget.ToSharedRef(), vSize);
 			//MateraiInstance->SetTextureParameterValue("SlateUI", pRenderTarget);
 			//UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), pRenderTarget, MateraiInstance);
 
 			FMaterialParameterInfo ParamInfo;
 			ParamInfo.Name = "SlateUI";
 			ParamInfo.Index = 0;
 			UTexture* pMaterialRenderTarget;
 			MateraiInstance->GetTextureParameterValue(ParamInfo, pMaterialRenderTarget);
 			UTextureRenderTarget2D* CastedTexture = Cast<UTextureRenderTarget2D>(pMaterialRenderTarget);
 			if (CastedTexture == nullptr)
 				return;
 
 			pWidgetRenderer->DrawWidget(CastedTexture, SlateWidget.ToSharedRef(), vSize, 0);
 		}
 	}
}

void UWRUIPanel_Title::UpdateTestMaterial(float IN InDeltaTime)
{
	if (fOpacity < 1.0f)
 	{
 		float fMul = 0.3f;
 		fOpacity += (InDeltaTime * fMul);
 		if (MateraiInstance != nullptr)
 		{
 			MateraiInstance->SetScalarParameterValue("UIMainOpacity", fOpacity);
 
 			FVector2D vSize = FVector2D(500, 200);
 			//pRenderTarget = pWidgetRenderer->DrawWidget(SlateWidget.ToSharedRef(), vSize, InDeltaTime);			
 			//MateraiInstance->SetTextureParameterValue("SlateUI", pRenderTarget);
 
 			//UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), pRenderTarget, MateraiInstance);
 
 			FMaterialParameterInfo ParamInfo;
 			ParamInfo.Name = "SlateUI";
 			ParamInfo.Index = 0;
 			UTexture* pMaterialRenderTarget;
 			MateraiInstance->GetTextureParameterValue(ParamInfo, pMaterialRenderTarget);
 			UTextureRenderTarget2D* CastedTexture = Cast<UTextureRenderTarget2D>(pMaterialRenderTarget);
 			if (CastedTexture == nullptr)
 				return;
 
 			pWidgetRenderer->DrawWidget(CastedTexture, SlateWidget.ToSharedRef(), vSize, InDeltaTime);
 		}
 	}
}
#endif
