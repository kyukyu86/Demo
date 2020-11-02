#pragma once
#include "WRStaticMeshActor.h"
#include "WRStaticMeshProjectile.generated.h"

UCLASS()
class WR_API AWRStaticMeshProjectile : public AWRStaticMeshActor
{
	GENERATED_UCLASS_BODY()

public:

	UFUNCTION(BlueprintCallable)
		virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	virtual void Tick(float DeltaSeconds);
	void SetIdendifier(AID_t InOwnerAID, SKILL_SEQ_t InSkillSeqID);

	void Initialize();
	void Release();
	void TickDistance2Spawner(float DeltaSeconds);

	WRDestoryHandle& OnDestroy() {
		return DestoryHandle;
	}

public:
	static FName ProjectileMovementComponentName;
	class UProjectileMovementComponent* ProjectileMovement = nullptr;

private:
	AID_t OwnerAID;
	SKILL_SEQ_t CombatSeqID;

	uint32 UID = 0;

	WRDestoryHandle DestoryHandle;
	bool bPlayingTakeAnim = false;
};
