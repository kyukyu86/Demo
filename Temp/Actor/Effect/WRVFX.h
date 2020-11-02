// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Enum/EWRVFX.h"

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"

#include "WRVFX.generated.h"

#define WR_INFINITE_LIFESPAN -1.0F

DECLARE_MULTICAST_DELEGATE(FWRComplete);

USTRUCT(BlueprintType)
struct FWRVFXs
{
	GENERATED_BODY()

public:
	bool Tick(float DeltaTime);

	void Reset();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVFX")
	float Lifespan = -1.0F;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WRVFX")
	float AccLifespan = 0.0F;
};

USTRUCT(BlueprintType)
struct FWRVFXParameters
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVFXParameter")
	float Duration = 1.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVFXParameter")
	FName ParameterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVFXParameter")
	EWRVFXParameter Distribution = EWRVFXParameter::Scalar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVFXParameter", DisplayName = "Value", meta = (EditCondition = "Distribution == EWRVFXParameter::Scalar"))
	float Scalar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVFXParameter", DisplayName = "Value", meta = (EditCondition = "Distribution == EWRVFXParameter::Vector"))
	FVector Vector;

	float LastScalar = 0.0F;
	FVector LastVector = FVector::ZeroVector;
};

class UParticleSystemComponent;
class UNiagaraComponent;
class UWRVFXComponentAttach;
class UWRVFXComponentMove;
class UWRVFXComponentSound;
class UWRVFXComponentSpawn;

UCLASS()
class WR_API AWRVFX : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWRVFX();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	void Initialize(AActor* InSpawner, AActor* InTarget, AWRVFX* InParent, const EWRVFX InBits, const FRotator& InAngle);
	void Release();

	void SetActive(const bool bEnabled);

	class AActor* GetSpawner() {
		return Spawner;
	}

	class AActor* GetTarget() {
		return Target;
	}

	class AWRVFX* GetParent() {
		return Parent;
	}

	uint64 GetTargetAID();
	uint64 GetSpawnerAID();
	uint32 GetParentUID();

	void RemoveSpawner() {
		Spawner = nullptr;
	}

	void RemoveTarget() {
		Target = nullptr;
	}

	void RemoveChilds();

#ifdef WR_EFFECT_NIAGARA
	// [ 2019-11-14 : animal14 ] 범용 함수
	const struct FNiagaraParameterStore* GetOverrideParameters();

	// [ 2019-11-14 : animal14 ] 전용 함수
	void SetNiagaraDataInterfaceSkeletalMesh();
#else // WR_EFFECT_NIAGARA
#endif // WR_EFFECT_NIAGARA

	FWRComplete& OnComplete() {
		return Complete;
	}

#ifdef WR_EFFECT_NIAGARA
	void SetNiagaraVariableActor(const FString& InVariableName, AActor* InActor);
	AActor* GetNiagaraVariableActor(const FString& InVariableName);

	void SetNiagaraVariableBool(const FString& InVariableName, bool InValue);
	bool GetNiagaraVariableBool(const FString& InVariableName);

	void SetNiagaraVariableFloat(const FString& InVariableName, float InValue);
	float GetNiagaraVariableFloat(const FString& InVariableName);

	void SetNiagaraVariableInt(const FString& InVariableName, int32 InValue);
	int32 GetNiagaraVariableInt(const FString& InVariableName);

	void SetNiagaraVariableLinearColor(const FString& InVariableName, const FLinearColor& InValue);
	FLinearColor GetNiagaraVariableLinearColo(const FString& InVariableName);

	void SetNiagaraVariableQuat(const FString& InVariableName, const FQuat& InValue);
	FQuat GetNiagaraVariableQuat(const FString& InVariableName);

	void SetNiagaraVariableVec2(const FString& InVariableName, FVector2D InValue);
	FVector2D GetNiagaraVariableVec2(const FString& InVariableName);

	void SetNiagaraVariableVec3(const FString& InVariableName, FVector InValue);
	FVector GetNiagaraVariableVec3(const FString& InVariableName);

	void SetNiagaraVariableVec4(const FString& InVariableName, const FVector4& InValue);
	FVector4 GetNiagaraVariableVec4(const FString& InVariableName);
#else // WR_EFFECT_NIAGARA
#endif // WR_EFFECT_NIAGARA

	void SetActorParameter(const FName& InParameterName, class AActor* InParam);
	bool GetActorParameter(const FName& InParameterName, class AActor*& OutActor);

	void SetColorParameter(const FName& InParameterName, const FLinearColor& InParam);
	bool GetColorParameter(const FName& InParameterName, FLinearColor& OutColor);

	void SetFloatParameter(const FName& InParameterName, const float InParam);
	bool GetFloatParameter(const FName& InParameterName, float& OutFloat);

	void SetFloatRandParameter(const FName& InParameterName, const float InParam, const float InParamLow);

	void SetVectorParameter(const FName& InParameterName, const FVector& InParam);
	bool GetVectorParameter(const FName& InParameterName, FVector& OutVector);

	void SetVectorRandParameter(const FName& InParameterName, const FVector& InParam, const FVector& InParamLow);

	void SetMaterialParameter(const FName& InParameterName, class UMaterialInterface* InParam);
	bool GetMaterialParameter(const FName& InParameterName, class UMaterialInterface*& OutMaterial);

	SKILL_SEQ_t GetVFXCombatSeqID() { 
		return CombatSeqID;
	}

#if UE_EDITOR
	FWRVFXs GetVFX() {
		return VFX;
	}
#else // UE_EDITOR
#endif // UE_EDITOR

	void ResetParameter() {
		VFX.Reset(); 
	}

	bool IsLooping() {
		return (VFX.Lifespan > 0.0F) ? false : true;
	}

	bool IsEmpty() {
		return VFXParameters.Num() == 0 ? true : false;
	}

#if UE_EDITOR
	void SetLifespan4Preview() {
		this->SetLifeSpan(VFX.Lifespan);
	}
#else // UE_EDITOR
#endif // UE_EDITOR

	void SetAdditionalAngle(const FRotator& InAngle);

	template<class T>
	T* GetCastedComponent(const EWRVFXComponent InType);

protected:
	template<class T>
	T* GetComponentBy(const EWRVFXComponent InType);

private:
	void BuildComponents();

	void Check();

	void SetParent(AWRVFX* InActor) {
		Parent = InActor;
	}

	void Adopted(AWRVFX* InActor);
	void Adopting(AWRVFX* InActor);
	void Abandon(AWRVFX* InActor);
	void Abandon();

	bool IsBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OnComponentBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult& SweepResult);

	UFUNCTION()
	void OnNiagaraSystemFinished(class UNiagaraComponent* InComponent);
	UFUNCTION()
	void OnSystemFinished(class UParticleSystemComponent* InComponent);
	//UFUNCTION()
	//void OnParticleCollide(FName InEventName, float InEmitterTime, int32 InParticleTime, FVector InLocation, FVector InVelocity, FVector InDirection, FVector InNormal, FName InBoneName, UPhysicalMaterial* InPhysMat);

	void Tick4Parameter();
	void Tick4Mesh();
	void Tick4Scale();
	void Tick4Rotation();

	void PostInitialize(const bool InShouldFadeParticle);
	void PreDestroy();

	//EWRVFXMove GetMovementType();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVFX")
	EWRVFXCollisionResult CollisionResult;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WRVFX")
	EWRVFX Bits = EWRVFX::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVFX")
	FWRVFXs VFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVFX")
	TArray<FWRVFXParameters> VFXParameters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVFX", meta = (UIMin = 0, ClampMin = 0, UIMax = 899, ClampMax = 899))
	int32 AdditionalTranslucencySortPriority = 0;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVFX")
	//TSoftObjectPtr<class UMaterialInterface> MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVFXMeshes", DisplayName = "Size By Life")
	bool SizeByLife = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVFXMeshes", DisplayName = "Scale", meta = (EditCondition = "SizeByLife"))
	FVector ToScale = FVector::ZeroVector;
	FVector FromScale = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVFXMeshes")
	bool Rotation = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVFXMeshes", DisplayName = "Axis", meta = (EditCondition = "Rotation"))
	FRotator Axis = FRotator::ZeroRotator;
	FRotator NewRotation = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WRVFXMeshes", DisplayName = "Rate", meta = (EditCondition = "Rotation"))
	float Rate = 0.0F;

	AActor* Spawner = nullptr;
	AActor* Target = nullptr;

	AWRVFX* Parent = nullptr;

	TMap<uint32, AWRVFX*> Childs;
	TMap<EWRVFXComponent, UActorComponent*> Components;

	float LastLifespan = -1.0F;

	EWRVFXFade Type = EWRVFXFade::None;

	FWRComplete Complete;

	SKILL_SEQ_t CombatSeqID;

private:
	friend class UWRVFXComponentAttach;
	friend class UWRVFXComponentMove;
	friend class UWRVFXComponentSound;
	friend class UWRVFXComponentSpawn;

	friend class UWRActorComponentVFX;

	friend class WRVFXManager;
};

template<class T>
T* AWRVFX::GetCastedComponent(const EWRVFXComponent InType)
{
	if (Components.Contains(InType) == false)
	{
		return nullptr;
	}
	return Cast<T>(Components[InType]);
}

template<class T>
T* AWRVFX::GetComponentBy(const EWRVFXComponent InType)
{
	if (Components.Contains(InType))
	{
		return Cast<T>(Components[InType]);
	}

	T* Component = Cast<T>(GetComponentByClass(T::StaticClass()));
	if (Component == nullptr)
	{
		return nullptr;
	}

	Component->AddToRoot();

	this->AddOwnedComponent(Component);
	Components.Add(InType, Component);

	return Component;
}