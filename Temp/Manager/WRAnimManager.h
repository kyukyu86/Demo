#pragma once
#include "SingletonClass/WRSingleton.h"
#include "Enum/EWRCharacter.h"
#include <Animation/AnimMontage.h>


class WRAnimManager : public WRSingleton<WRAnimManager>
{
public:
	WRAnimManager();
	virtual ~WRAnimManager();

public:
	// [ 2019-6-14 : magedoga ] �Ŵ��� ���� ������ Initialize ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnInitialize()			override;

	// [ 2019-6-14 : magedoga ] ���� ���� ������ Shutdown ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnShutdown()			override;

	// [ 2019-6-14 : magedoga ] ���� ���� �� ���� ���� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelStart()		override;

	// [ 2019-6-14 : magedoga ] ���� ���� �� ���� ���� �Ϸ� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-6-14 : magedoga ] ���� ���� �� �������� ���۵� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnRenderStart() 		override;
	//====================================================================================

	virtual bool OnTick(float InDeltaTime) override;
	
public:
	void LoadMontage();
	void LoadMontage(WRTableID InMontageTID);
	void OnLoadMontageCompleted(UObject* InMontage, FString InHashKey);
	bool Contains(WRTableID InMontageTID);
	UAnimMontage* FindMontage(WRRecordTID InMontageID);

private:
	TMap<WRTableID, UAnimMontage*> mapAnimMontage;		//MontageTID, montage*
	TMap<FString, WRTableID> AsyncAssetsKey;
};

