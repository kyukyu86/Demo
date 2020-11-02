// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/Base/WRUserWidget.h"
#include "Enum/EWRUIEnum.h"
#include "WRUIPanel.generated.h"

/**
 * 
 */
UCLASS()
class WR_API UWRUIPanel : public UWRUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
		FVector2D DrawSize;
	UPROPERTY(EditAnywhere)
		bool bCyberUI = false;
	UPROPERTY(EditAnywhere)
		bool bUseCyberAni = false;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "100000", ClampMax = "100000000"))
		int32 TranslucentSortPriority = 100000;
	UPROPERTY(EditAnywhere)
		bool bFadeOutWhenClosePanel = false;
	UPROPERTY(EditAnywhere)
		bool bRenderCustomDepth = false;

	UPROPERTY(EditAnywhere)
		bool bUseCylinderGeometry = false;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bUseCylinderGeometry"))
		float CylinderArcAngle = 30.f;

	bool bFadingOut = false; // 지금 페이드 아웃 중인지
	EWRUIPanelKind PanelKind;
	WRUIHandle UIHandle;

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	void OnFadeOutFinished();	

public:
	virtual void ThrowParam(const int32 IN InParam) {} 

	FORCEINLINE bool IsFadeOutWhenClosePanel() { return bFadeOutWhenClosePanel; }
	FORCEINLINE bool IsFadingOut() { return bFadingOut; }

	void ClosePanelAfterFadeOut(); // 페이드 처리후 패널을 닫아달라
	void CancelFadeOutForClosePanel(); // 페이드 처리후 패널을 닫는 것을 취소한다.
	FORCEINLINE bool IsCyberUI() { return bCyberUI; }
	FORCEINLINE bool IsUseCyberAni() { return bUseCyberAni; }
	FORCEINLINE int32 GetTranslucentSortPriority() { return TranslucentSortPriority; }
	FORCEINLINE bool GetRenderCustomDepth() { return bRenderCustomDepth; }

public:
	FORCEINLINE FVector2D GetDrawSize() const { return DrawSize; }
	FORCEINLINE void SetUIHandle(WRUIHandle IN InUIHandle) { UIHandle = InUIHandle; }
	FORCEINLINE WRUIHandle GetUIHandle() const { return UIHandle; }
	FORCEINLINE EWRUIPanelKind GetPanelKind() const { return PanelKind;	}
	FORCEINLINE void SetPanelKind(const EWRUIPanelKind IN InPanelKindToSet) { PanelKind = InPanelKindToSet;  }
	FORCEINLINE bool IsUsedCylinderGeometry() { return bUseCylinderGeometry; }
	FORCEINLINE float GetCylinderArcAngle() { return CylinderArcAngle; }
};
