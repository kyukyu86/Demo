// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRLadder.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Actor/Character/WRCharacterPlayer.h"
#include "Manager/WRInputSystemManager.h"
#include "Manager/WRUIManager.h"
#include "WidgetComponent/WRWidgetComponentBase.h"
#include "UI/Base/WRUIPanel.h"
#include "UI/Panel/WRUIPanel_ClimbHandGuide.h"
#include "Manager/WRConsoleManager.h"
#include "Component/Character/WRActorComponentLadder.h"

AWRLadder::AWRLadder()
{

}

void AWRLadder::BeginPlay()
{
	Super::BeginPlay();

	// Each Position
	TArray<UActorComponent*> TopStartPositionComponents = this->GetComponentsByTag(USceneComponent::StaticClass(), "TopStartPosition");
	if (TopStartPositionComponents.IsValidIndex(0))
	{
		TopStartPositionComponent = Cast<USceneComponent>(TopStartPositionComponents[0]);
	}
	else
	{
		ensureMsgf(false, TEXT("TopPosition is not setted"));
	}
	TArray<UActorComponent*> TopEndPositionComponents = this->GetComponentsByTag(USceneComponent::StaticClass(), "TopEndPosition");
	if (TopEndPositionComponents.IsValidIndex(0))
	{
		TopEndPositionComponent = Cast<USceneComponent>(TopEndPositionComponents[0]);
	}
	else
	{
		ensureMsgf(false, TEXT("TopPosition is not setted"));
	}
	TArray<UActorComponent*> BottomPositionComponents = this->GetComponentsByTag(USceneComponent::StaticClass(), "BottomStartPosition");
	if (BottomPositionComponents.IsValidIndex(0))
	{
		BottomPositionComponent = Cast<USceneComponent>(BottomPositionComponents[0]);
	}
	else
	{
		ensureMsgf(false, TEXT("BottomPosition is not setted"));
	}

	// Each Area
	TArray<UActorComponent*> InteractionAreaComponents = this->GetComponentsByTag(UBoxComponent::StaticClass(), "InteractionArea");
	if (InteractionAreaComponents.IsValidIndex(0))
	{
		InteractionArea = Cast<UBoxComponent>(InteractionAreaComponents[0]);
		InteractionArea->OnComponentBeginOverlap.AddDynamic(this, &AWRLadder::OnBeginOverlap_InteractionArea);
		InteractionArea->OnComponentEndOverlap.AddDynamic(this, &AWRLadder::OnEndOverlap_InteractionArea);
	}
	else
	{
		ensureMsgf(false, TEXT("InteractionArea is not setted"));
	}

	// Make Handle Info
	int32 nIndex = 0;
	for (auto StaticComponent : LadderHandles)
	{
		FWRLadderHandle* pHandle = new FWRLadderHandle();
		pHandle->HandleComponent = StaticComponent;
		pHandle->HandType = GetHandType(nIndex);
		pHandle->nIndex = nIndex;
		Handles.Add(pHandle);
		++nIndex;
	}
	FWRLadderHandle* pHandle = Handles.Last(0);
	if (pHandle)
		pHandle->bTop = true;

	// Make Hand Guide Widget
	FWRUILoadCompletedDelegate Delegate = FWRUILoadCompletedDelegate::CreateLambda([=](const FWRCombinedPanelData& InCombinedPanelData)
	{
		if (InCombinedPanelData.UIPanel != nullptr && InCombinedPanelData.WidgetComponentBase != nullptr)
		{
			WidgetComponentBase = InCombinedPanelData.WidgetComponentBase;
			// [ 2019-8-28 : kyu ] 컨텐츠 내용에 따라 창 크기가 유동적이어야 한다.
			WidgetComponentBase->SetDrawAtDesiredSize(true);

			HandGuideWidget = Cast<UWRUIPanel_ClimbHandGuide>(InCombinedPanelData.UIPanel);
			HandGuideWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	});

	FWRWidgetInfo WidgetInfo(EWRUIPanelKind::ClimbHandGuide, EWRUIWidgetCreationBitFlag::Billboard | EWRUIWidgetCreationBitFlag::IgnoreDepth, &Delegate);
	WidgetInfo.SetTransform(GetActorTransform());
	WRUIManager::Get()->OpenUI<UWRWidgetComponentBase>(WidgetInfo);
}

void AWRLadder::BeginDestroy()
{
	for (auto Handle : Handles)
	{
		delete Handle;
	}
	Handles.Empty();
	Super::BeginDestroy();
}

void AWRLadder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateHandGuide(false);
}

void AWRLadder::OnBeginOverlap_InteractionArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(Other);
	if (CastedPlayer == nullptr)
		return;

	UWRActorComponentLadder* pLadderComp = CastedPlayer->GetComponent<UWRActorComponentLadder>();
	if (pLadderComp == nullptr)
		return;

	pLadderComp->SetOverlapLadderArea(this);

	// to Hand Guide Widget
	if (CastedPlayer->IsMe() && WidgetComponentBase != nullptr && HandGuideWidget != nullptr)
	{
		FWRLadderHandle* pNearestHandle = GetNearestHandle(CastedPlayer);
		if (IsTopHandle(pNearestHandle) == false)
			pNearestHandle = GetBottomStartHandle();

		if (pNearestHandle == nullptr)
			return;

		AttachHandGuide(pNearestHandle, false);
	}
}

void AWRLadder::OnEndOverlap_InteractionArea(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AWRCharacterPlayer* CastedPlayer = Cast<AWRCharacterPlayer>(Other);
	if (CastedPlayer == nullptr)
		return;

	UWRActorComponentLadder* pLadderComp = CastedPlayer->GetComponent<UWRActorComponentLadder>();
	if (pLadderComp == nullptr)
		return;

	pLadderComp->SetOverlapLadderArea(nullptr);

	// to Hand Guide Widget
	if (CastedPlayer->IsMe() && WidgetComponentBase != nullptr && HandGuideWidget != nullptr)
	{
		SetHandGuideVisibility(false);
	}
}

//====================================================================================

int32 AWRLadder::GetHandleIndex(FWRLadderHandle* IN InHandle)
{
	int32 nIndex = 0;
	for (auto Handle : Handles)
	{
		if (Handle == InHandle)
			return nIndex;
		++nIndex;
	}
	return DEF_INVALID_LADDER_HANDLE_INDEX;
}

FTransform AWRLadder::GetTopStartPosition()
{
	return TopStartPositionComponent->GetComponentToWorld();
}

FTransform AWRLadder::GetTopEndPosition()
{
	return TopEndPositionComponent->GetComponentToWorld();
}

FTransform AWRLadder::GetBottomPosition()
{
	return BottomPositionComponent->GetComponentToWorld();
}

FWRLadderHandle* AWRLadder::GetHandle(const int32 IN InIndex)
{
	if (Handles.IsValidIndex(InIndex))
		return Handles[InIndex];

	return nullptr;
}

FWRLadderHandle* AWRLadder::GetTopHandle()
{
	return GetHandle(LadderHandles.Num() - 1);
}

FWRLadderHandle* AWRLadder::GetBottomStartHandle()
{
	return GetHandle(BottomStartingHandleNumber);
}

FWRLadderHandle* AWRLadder::GetNearestHandle(class AWRCharacterPlayer* IN pPlayer)
{
	FVector vFirstHandle = Handles[0]->GetLocation();
	FVector vPlayer = pPlayer->GetActorLocation();
	FWRLadderHandle* pNearestHandle = Handles[0];
	float fNearestDist = FVector::Dist(vFirstHandle, vPlayer);

	for (auto Handle : Handles)
	{
		FVector vHandle = Handle->GetLocation();
		float fDist = FVector::Dist(vHandle, vPlayer);
		if (fNearestDist > fDist)
		{
			fNearestDist = fDist;
			pNearestHandle = Handle;
		}
	}
	return pNearestHandle;
}

float AWRLadder::GetCalculatedCharLocationZ(FWRLadderHandle* IN InHandle)
{
	FVector vHandleLoc = InHandle->GetLocation(); // 핸들 위치 가져오고
	vHandleLoc.Z += GetHandleMeshHeight(); // 실제 손잡이 위치만큼 올리고
	vHandleLoc.Z += CharAttachOffset; // 케릭터 오프셋만큼 위치 시킴

	return vHandleLoc.Z;
}

bool AWRLadder::IsTopHandle(const int32 IN InHandleIndex)
{
	if (InHandleIndex == (LadderHandles.Num() - 1))
		return true; // 위로 이동시 마지막 인덱스는 끝
	return false;
}

bool AWRLadder::IsTopHandle(FWRLadderHandle* IN InComp)
{
	if (InComp == nullptr)
		return false;
	if (InComp == GetTopHandle())
		return true;
	return false;
}
bool AWRLadder::IsBottomHandle(const int32 IN InHandleIndex)
{
	if (InHandleIndex == BottomStartingHandleNumber)
		return true; // 아래로 이동시 아래 마지막 인덱스는 끝
	return false;
}

EWRLadderHand AWRLadder::GetHandType(const int32 IN InIndex)
{
	if (InIndex == 0 || InIndex % 2 == 0)
	{
		if(StartHandType == EWRLadderHand::Left)
			return EWRLadderHand::Left;
	}
	else
	{
		if (StartHandType == EWRLadderHand::Right)
			return EWRLadderHand::Left;
	}
	return EWRLadderHand::Right;
}

//====================================================================================

void AWRLadder::ReleaseFromMe(class AWRCharacterPlayer* IN InMe)
{
	SetHandGuideVisibility(false);
	PrevDirection = EWRLadderDirection::None;
	MyHandleIndex = DEF_INVALID_LADDER_HANDLE_INDEX;

	// Finish, Cancel이어도 영역에 들어와 있으면 다시 출력해야한다
	if (InMe->IsMe() && WidgetComponentBase != nullptr && HandGuideWidget != nullptr)
	{
		if (InteractionArea->IsOverlappingActor(InMe) == false)
			return;

		FWRLadderHandle* pNearestHandle = GetNearestHandle(InMe);
		if (IsTopHandle(pNearestHandle) == false)
			pNearestHandle = GetBottomStartHandle();

		if (pNearestHandle == nullptr)
			return;

		AttachHandGuide(pNearestHandle, false);
	}
}

//====================================================================================

void AWRLadder::SetHandGuideVisibility(const bool IN InVisible)
{
	if (bVisibleGuideWidget == true && InVisible == false)
	{
		bVisibleGuideWidget = false;
		if (HandGuideWidget != nullptr)
		{
			HandGuideWidget->SetVisibility(bVisibleGuideWidget ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		}
	}
	else if (bVisibleGuideWidget == false && InVisible == true)
	{
		bVisibleGuideWidget = true;
		if (HandGuideWidget != nullptr)
		{
			HandGuideWidget->SetVisibility(bVisibleGuideWidget ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		}
	}
}

void AWRLadder::UpdateHandGuideOnlyForMe(const int32 IN InCurrentHandleIndex)
{
	if (WidgetComponentBase == nullptr || HandGuideWidget == nullptr)
		return;

	MyHandleIndex = InCurrentHandleIndex;
	UpdateHandGuide(true);
}

void AWRLadder::UpdateHandGuide(const bool IN InForce)
{
	if (bVisibleGuideWidget == false)
		return;

	if (MyHandleIndex == DEF_INVALID_LADDER_HANDLE_INDEX)
		return;
	
	EWRLadderDirection HMDDirection = WRObjectInteractionManager::Get()->GetLadderDirection();
	if (InForce == false && HMDDirection == PrevDirection)
		return;

	PrevDirection = HMDDirection;

	FWRLadderHandle* pCurHandle = GetHandle(MyHandleIndex);

	switch (HMDDirection)
	{	
	case EWRLadderDirection::Up:
	{
		if (IsTopHandle(MyHandleIndex) == false)
		{
			FWRLadderHandle* pHandle = GetHandle(MyHandleIndex + 1);
			AttachHandGuide(pHandle, false);
		}
 		else
 		{
 			AttachTopHandleGuide();
 		}
	}
	break;
	case EWRLadderDirection::Down:
	{
		//if (IsBottomHandle(MyHandleIndex) == false)
		{
			FWRLadderHandle* pHandle = GetHandle(MyHandleIndex - 1);
			AttachHandGuide(pHandle, true);
		}
	}
	break;
	}
}

void AWRLadder::AttachHandGuide(FWRLadderHandle* IN InHandle, bool IN InInverse)
{
	if (InHandle == nullptr)
		return;
	if (WidgetComponentBase == nullptr)
		return;
	if (HandGuideWidget == nullptr)
		return;

	float fAdd = 0.f;
	EWRLadderHand eHandType = InHandle->HandType;
 	if (InInverse)
 	{
		if (eHandType == EWRLadderHand::Left)
		{
			eHandType = EWRLadderHand::Right;
		}
		else if (eHandType == EWRLadderHand::Right)
		{
			eHandType = EWRLadderHand::Left;
		}
 	}

	if (eHandType == EWRLadderHand::Left)
	{
		fAdd = HandGuideOffset * -1.f;
		HandGuideWidget->SetLeft(true);
	}
	else if (eHandType == EWRLadderHand::Right)
	{
		fAdd = HandGuideOffset;
		HandGuideWidget->SetLeft(false);
	}

	FTransform WCTransform = InHandle->GetTransform();
	FVector vSide = InHandle->GetForwardVector() * fAdd; // 매쉬가 90도 돌아가 있어서 RightVector가 아닌 ForwardVector를 쓴다
	FVector vDstLoc = WCTransform.GetLocation() + vSide;
	vDstLoc.Z += HandleMeshHeight; // 실제 손잡이는 메쉬의 중간즈음임
	WCTransform.SetLocation(vDstLoc);
	WidgetComponentBase->SetWorldTransform(WCTransform);

	SetHandGuideVisibility(true);
}

void AWRLadder::AttachTopHandleGuide()
{
	FWRLadderHandle* TopHandle = GetTopHandle();
	float fAdd = 0.f;
	if (TopHandle->HandType == EWRLadderHand::Left)
	{
		if(PrevDirection == EWRLadderDirection::Up)
			fAdd = HandGuideOffset;
		else if(PrevDirection == EWRLadderDirection::Down)
			fAdd = HandGuideOffset * -1.f;

		HandGuideWidget->SetLeft(false); // 반대임
	}
	else if (TopHandle->HandType == EWRLadderHand::Right)
	{
		if (PrevDirection == EWRLadderDirection::Up)
			fAdd = HandGuideOffset * -1.f;
		else if (PrevDirection == EWRLadderDirection::Down)
			fAdd = HandGuideOffset;

		HandGuideWidget->SetLeft(true); // 반대임
	}

	FTransform WCTransform = TopEndPositionComponent->GetComponentTransform();
	FVector vSide = TopHandle->GetForwardVector() * fAdd;
	FVector vDstLoc = WCTransform.GetLocation() + vSide;
	WCTransform.SetLocation(vDstLoc);
//	WCTransform.SetRotation(WCTransform.GetRotation().Inverse());
	WidgetComponentBase->SetWorldTransform(WCTransform);

	SetHandGuideVisibility(true);
}
