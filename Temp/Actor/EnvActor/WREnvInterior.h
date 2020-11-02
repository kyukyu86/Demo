#pragma once
#include "WREnvActor.h"
#include "Enum/EWRCharacter.h"
#include "WREnvInterior.generated.h"


UCLASS()
class WR_API AWREnvInterior : public AWREnvActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWREnvInterior();
	AWREnvInterior(EWREnvType InEnvType);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void BeginInteraction() {};

public:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	void RegistOverlapEvent();	//������ �����Ǳ� ������ manager���� ������ �̺�Ʈ�� �޴´� ���� ���� ������ �Ǹ� �ش� �̺�Ʈ�� ��ü���� ó��

protected:


public:

};

