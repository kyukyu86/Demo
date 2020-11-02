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
	void RegistOverlapEvent();	//완전히 스폰되기 전에는 manager에서 오버랩 이벤트를 받는다 이후 완전 스폰이 되면 해당 이벤트를 객체에서 처리

protected:


public:

};

