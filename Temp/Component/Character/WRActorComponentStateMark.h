// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Component/Base/WRActorComponent.h"
#include "WRActorComponentStateMark.generated.h"


/**
 * 
 */
UCLASS()
class WR_API UWRActorComponentStateMark : public UWRActorComponent
{
	GENERATED_BODY()
public:
	// enum 순서는 우선순위를 결정한다. 앞에 있을수록 우선순위가 높다
	enum EWRStateMarkType { None, Talking, AvailableTalk};
public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:
	void AddMark(const EWRStateMarkType IN InStateMarkType);
	void RemoveMark(const EWRStateMarkType IN InStateMarkType);
	bool ContainsMark(const EWRStateMarkType IN InStateMarkType);
private:
	void ShowMark(const EWRStateMarkType IN InStateMarkType);
	void HideMark(const EWRStateMarkType IN InStateMarkType);
	bool IsAvailableShowMark(const EWRStateMarkType IN InStateMarkType);
	bool IsAvailableShowMark(const EWRStateMarkType IN InStateMarkType, const EWRStateMarkType IN InVisibleMarkType);

	TDoubleLinkedList<EWRStateMarkType> StateMarkTypes;
	EWRStateMarkType VisibleStateMarkType = EWRStateMarkType::None;
};
