// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "Component/Base/WRActorComponent.h"

#include "Components/MeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Curves/CurveFloat.h"

#include "../../Enum/EWRCharacter.h"

#include "Manager/WRAsyncLoadUObjectManager.h"

#include "CoreMinimal.h"
#include "WRActorComponentEquipmentLook.generated.h"

DECLARE_STATS_GROUP(TEXT("WRActorComponentEquipmentLook"), STATGROUP_WRActorComponentEquipmentLook, STATCAT_Advanced);

DECLARE_LOG_CATEGORY_EXTERN(WRActorComponentEquipmentLook, Display, All);

DECLARE_DELEGATE_TwoParams(FWREquipmentComplete, AActor*, EWREquipPart);
DECLARE_DELEGATE_NineParams(FWREquipmentDirectionComplete, EWREquipPart, FName, EWRAdditionalAbility, FString, FString, FString, FString, USkeletalMesh*, USkeletalMesh*);

struct FWRMeshComponent
{
public:
	FWRMeshComponent(class AWRCharacter* InOwner);
	virtual ~FWRMeshComponent();

public:
	virtual void SetMesh(class UStaticMesh* InMesh) {
		CleanupOverrideMaterials();
	}
	virtual void SetMesh(class USkeletalMesh* InMesh) {
		CleanupOverrideMaterials();
	}
	virtual void SetMesh() {
		CleanupOverrideMaterials();
	}

	void Initialize(struct FWREquipmentLookTable* InTable);

	void SetMaterialParameterValue(FName InParameterName, float InValue, class UMeshComponent* InMeshComponent = nullptr);
	void SetMaterialParameterValue(FName InParameterName, FLinearColor InValue, class UMeshComponent* InMeshComponent = nullptr);

	void RestoreMaterialInterface();

	void SetMaterial(int32 ElementIndex, class UMaterialInterface* Material);

	virtual UStaticMesh* GetStaticMesh() {
		return nullptr;
	}
	virtual USkeletalMesh* GetSkeletalMesh() {
		return nullptr;
	}
	virtual UMeshComponent* GetMesh() {
		return nullptr;
	}

	FTransform GetSocketTransform(ERelativeTransformSpace TransformSpace = RTS_World);

	FName GetAttachSocketName() {
		return SocketName;
	}
	EWRAdditionalAbility GetAbility() {
		return Ability;
	}
	WRTableID GetTID() {
		return TID;
	}

	bool IsVisible();

	void SetVisibility(bool bNewVisibility);
	void SetCollisionEnable(ECollisionEnabled::Type NewType);

	void SetBodyVisibility();
	void GetBodyVisibility();

	bool IsMainMeshPart(EWREquipPart InPart) {
		return InPart == EWREquipPart::Body;
	}
	bool IsWeaponMeshPart(EWREquipPart InPart) {
		return InPart == EWREquipPart::RightHand || InPart == EWREquipPart::LeftHand;
	}
	bool IsMergedMeshPart(EWREquipPart InPart) {
		return InPart < EWREquipPart::Body;
	}

	void SettingEquipment(FName InSocketName, EWREquipPart InPart, EWRAdditionalAbility InAbility, const FString& InLight, const FString& InStaticMesh, const FString& In1st, const FString& In3st);
	void SettingEquipment(FName InSocketName, EWREquipPart InPart, EWRAdditionalAbility InAbility, const FString& InLight, const FString& InStaticMesh, USkeletalMesh* In1st, USkeletalMesh* In3st);

protected:
	virtual void CreateComponent();
	void DestroyComponent();

	void CreateAbility(EWRAdditionalAbility InAbility, const FString& InLight);
	void CreateHook();
	void CreateLight(const FString& InLight);

	void ReleaseAbility();
	void ReleaseHook();
	void ReleaseLight();

	void CleanupOverrideMaterials();

	void SetMaterialInterface();
	void ClearMaterialInterface();

	virtual void SettingEquipment(EWREquipPart InPart, const FString& InStaticMesh, const FString& In1st, const FString& In3st);
	virtual void SettingEquipment(EWREquipPart InPart, const FString& InStaticMesh, USkeletalMesh* In1st, USkeletalMesh* In3st);

protected:
	class AWRCharacter* OwnerCharacter = nullptr;

	FName SocketName = NAME_None;

	EWRAdditionalAbility Ability = EWRAdditionalAbility::None;

	TArray<class UMaterialInterface*> MaterialInterfaces;

	WRTableID TID = 0;

	bool BodyVisibility = true;

#if WITH_EDITOR
protected:
	FString MakeComponentName(EWREquipPart InPart);

private:
	static TMap<EWREquipPart, uint32> MeshComponentUIDs;
#else // WITH_EDITOR
#endif // WITH_EDITOR

};

struct FWRStaticMeshComponent : public FWRMeshComponent
{
public:
	FWRStaticMeshComponent(class AWRCharacter* InOwner, struct FWREquipmentLookTable* InTable);
	virtual ~FWRStaticMeshComponent();

public:
	virtual void SetMesh(class UStaticMesh* InMesh) override;
	virtual void SetMesh() override;

	virtual UStaticMesh* GetStaticMesh() override {
		return StaticMeshComponent->GetStaticMesh();
	}
	virtual UMeshComponent* GetMesh() override {
		return StaticMeshComponent;
	}

	virtual void CreateComponent() override;

protected:
	bool IsHeadPart(EWREquipPart InPart) {
		return InPart > EWREquipPart::Ring && InPart < EWREquipPart::Backpack;
	}

	virtual void SettingEquipment(EWREquipPart InPart, const FString& InStaticMesh, const FString& In1st, const FString& In3st) override;
	virtual void SettingEquipment(EWREquipPart InPart, const FString& InStaticMesh, USkeletalMesh* In1st, USkeletalMesh* In3st) override;

private:
	class UStaticMeshComponent* StaticMeshComponent = nullptr;
};

struct FWRSkeletalMeshComponent : public FWRMeshComponent
{
public:
	FWRSkeletalMeshComponent(class AWRCharacter* InOwner, struct FWREquipmentLookTable* InTable);
	virtual ~FWRSkeletalMeshComponent();

public:
	virtual void SetMesh(class USkeletalMesh* InMesh) override;
	virtual void SetMesh() override;

	virtual USkeletalMesh* GetSkeletalMesh() override {
		return SkeletalMeshComponent->SkeletalMesh;
	}
	virtual UMeshComponent* GetMesh() override {
		return SkeletalMeshComponent;
	}

	virtual void CreateComponent() override;

protected:
	bool IsFirstPersonPart(EWREquipPart InPart) {
		return InPart < EWREquipPart::Face;
	}

	virtual void SettingEquipment(EWREquipPart InPart, const FString& InStaticMesh, const FString& In1st, const FString& In3st) override;
	virtual void SettingEquipment(EWREquipPart InPart, const FString& InStaticMesh, USkeletalMesh* In1st, USkeletalMesh* In3st) override;

private:
	class USkeletalMeshComponent* SkeletalMeshComponent = nullptr;

	EWREquipPart EquipmentPart = EWREquipPart::None;
};

USTRUCT()
struct FWREquipmentAsyncTask
{
	GENERATED_BODY()

public:
	FWREquipmentAsyncTask();
	FWREquipmentAsyncTask(const FString& InHash);
	virtual ~FWREquipmentAsyncTask();

public:
	void AddHash(const FString& InHash) {
		if (Objects.Contains(InHash))
		{
			return;
		}
		Objects.Add(InHash, nullptr);
	}

	void AddObject(const FString& InHash, UObject* InObject) {
		if (Objects.Contains(InHash) == false)
		{
			Objects.Add(InHash, nullptr);
		}
		Objects[InHash] = InObject;
	}

	bool Contains(const FString& InHash) {
		return Objects.Contains(InHash);
	}

	bool HasObjects(const FString& InHash) {
		if (Objects.Contains(InHash) == false)
		{
			return false;
		}
		return Objects[InHash] != nullptr;
	}

	void ExecuteIfBound(AActor* InActor) {
		CompleteDelegate.ExecuteIfBound(InActor, EquipmentPart);
	}

public:
	TMap<FString, UObject*> Objects;

	EWREquipPart EquipmentPart = EWREquipPart::None;

	FName SocketName = NAME_None;

	USkeletalMesh* FirstMergedMesh = nullptr;
	USkeletalMesh* ThirdMergedMesh = nullptr;

	struct FWREquipmentLookTable* Table = nullptr;

	FWREquipmentComplete CompleteDelegate = nullptr;
};

struct FWREquipmentDirectionData
{
public:
	float AccDeltaTime = 0.0F;

	EWREquipmentLookAction Action = EWREquipmentLookAction::None;
	EWREquipPart EquipmentPart = EWREquipPart::None;

	FName SocketName = NAME_None;

	USkeletalMesh* FirstMergedMesh = nullptr;
	USkeletalMesh* ThirdMergedMesh = nullptr;

	struct FWREquipmentLookTable* Table = nullptr;

	FWREquipmentDirectionComplete DirectionDelegate = nullptr;

	class AWRCharacter* Character = nullptr;

	UMeshComponent* CurrentMeshComponent = nullptr;
	UMeshComponent* NextMeshComponent = nullptr;

	UCurveFloat* SrcCurve = nullptr;
	UCurveFloat* DestCurve = nullptr;
};

struct FWREquipmentDirection
{
public:
	FWREquipmentDirection();
	virtual ~FWREquipmentDirection();

public:
	// [ 2020-6-29 : animal14 ] 삽입 연출
	bool BeginPlay(class AWRCharacter* InCharacter, UMeshComponent* InCurrentMeshComponent, struct FWREquipmentLookTable* InTable, const FName& InSocketName, FWREquipmentDirectionComplete InDelegate, USkeletalMesh* In1st, USkeletalMesh* In3rd);
	// [ 2020-6-29 : animal14 ] 삭제 연출
	bool BeginPlay(class AWRCharacter* InCharacter, UMeshComponent* InCurrentMeshComponent, EWREquipPart InPart, FWREquipmentDirectionComplete InDelegate);
	void StopPlay();

	bool IsDirecting() {
		return (DirectionDatas.Num()) ? true : false;
	}

private:
	bool Tick(float DeltaTime);

	void OnPreDirectionComplete(FWREquipmentDirectionData* InData);
	void OnPostDirectionComplete(FWREquipmentDirectionData* InData);

	bool IsStaticMesh(EWREquipPart InPart) {
		return InPart < EWREquipPart::RightHand;
	}

	bool IsMergedMesh(USkeletalMesh* In1st, USkeletalMesh* In3rd) {
		return (In1st || In3rd);
	}

private:
	TDoubleLinkedList<FWREquipmentDirectionData*> DirectionDatas;

	FDelegateHandle TickHandle;
};

//USTRUCT()
//struct FWREquipmentDirectionCompleteData
//{
//	GENERATED_BODY()
//
//public:
//	EWREquipPart InPart = EWREquipPart::None;
//
//	FName InSocketName = NAME_None;
//	
//	EWRAdditionalAbility InAbility = EWRAdditionalAbility::None;
//	
//	FString InLight;
//	
//	FString InStaticMesh;
//	FString In1stSkeletalMesh;
//	FString In3rdSkeletalMesh;
//
//	USkeletalMesh* In1stMergedMesh = nullptr;
//	USkeletalMesh* In3rdMergedMesh = nullptr;
//};

/**
 *
 */
UCLASS()
class WR_API UWRActorComponentEquipmentLook : public UWRActorComponent
{
	GENERATED_BODY()

public:
	virtual void OnCreateComponent() override;
	virtual void OnDestroyComponent() override;

public:
	// [ 2020-6-29 : animal14 ] 일반
	bool Equip(WRTableID InTID, const FName& InSocketName = NAME_None, FWREquipmentComplete InDelegate = nullptr, USkeletalMesh* In1st = nullptr, USkeletalMesh* In3rd = nullptr);
	bool Equip(struct FWREquipmentLookTable* InTable, const FName& InSocketName = NAME_None, FWREquipmentComplete InDelegate = nullptr, USkeletalMesh* In1st = nullptr, USkeletalMesh* In3rd = nullptr);

	void Unequip(EWREquipPart InPart, FWREquipmentComplete InDelegate = nullptr);

	template <typename T>
	void SetMaterialParameterValue(FName InParamName, T InValue);
	template <typename T>
	void SetMaterialParameterValue(FName InParamName, T InValue, const EWREquipPart InPart);

	void RestoreMaterialInterface();
	void RestoreMaterialInterface(const EWREquipPart InPart);

	void SetMaterial(int32 ElementIndex, UMaterialInterface* Material);
	void SetMaterial(int32 ElementIndex, UMaterialInterface* Material, const EWREquipPart InPart);

	void UpdateEquipment(const class AWRCharacterMine* InCharacter);

	bool IsVisible(EWREquipPart InPart);

	void SetVisibility(EWREquipPart InPart, bool bNewVisibility);
	void SetCollisionEnable(EWREquipPart InPart, ECollisionEnabled::Type NewType);

	void SetBodyVisibility();
	void GetBodyVisibility();

	UStaticMesh* GetStaticMesh(EWREquipPart InPart);
	USkeletalMesh* GetSkeletalMesh(EWREquipPart InPart);
	UMeshComponent* GetMesh(EWREquipPart InPart);

	bool IsMainMeshPart(EWREquipPart InPart) {
		return InPart == EWREquipPart::Body;
	}
	bool IsWeaponMeshPart(EWREquipPart InPart) {		
		return InPart == EWREquipPart::RightHand || InPart == EWREquipPart::LeftHand;
	}
	bool IsDefaultPart(EWREquipPart InPart) {
		return InPart > EWREquipPart::Outfit;
	}
	bool IsExchangePart(EWREquipPart InPart) {
		return InPart == EWREquipPart::Gloves || InPart == EWREquipPart::Shoes;
	}
	bool IsBodyPart(EWREquipPart InPart) {
		return InPart > EWREquipPart::LeftHand && InPart < EWREquipPart::Gloves;
	}
	bool IsInventoryPart(EWREquipPart InPart) {
		return InPart < EWREquipPart::Body;
	}
	bool IsFirstPersonPart(EWREquipPart InPart) {
		return InPart < EWREquipPart::Face;
	}

	bool IsDirecting() {
		return Director.IsDirecting();
	}

	bool IsEquipped(EWREquipPart InPart);

private:
	UFUNCTION()
	void OnAsyncLoadComplete(UObject* InObject, FString InHash);
	//UFUNCTION()
	void OnDirectionComplete(EWREquipPart InPart, FName InSocketName, EWRAdditionalAbility InAbility, FString InLight, FString InStaticMesh, FString In1stSkeletalMesh, FString In3rdSkeletalMesh, USkeletalMesh* In1stMergedMesh, USkeletalMesh* In3rdMergedMesh);
	void OnUnequipComplete(EWREquipPart InPart, FName InSocketName, EWRAdditionalAbility InAbility, FString InLight, FString InStaticMesh, FString In1stSkeletalMesh, FString In3rdSkeletalMesh, USkeletalMesh* In1stMergedMesh, USkeletalMesh* In3rdMergedMesh);

	bool Initialize(FWREquipmentAsyncTask* InTask);

	bool IsMergedMesh(USkeletalMesh* In1st, USkeletalMesh* In3rd) {
		return (In1st || In3rd);
	}

	WRTableID GetDefaultPart(struct FWREquipmentLookTable* InTable);
	WRTableID GetDefaultPart(EWREquipPart InPart);

	void UpdateDirection(EWREquipPart InPart, bool& OutIsDirected);

	void PreUpdateVisibility(EWREquipPart InPart);
	void PostUpdateVisibility(EWREquipPart InPart);

private:
	TMap<EWREquipPart, FWRMeshComponent*> Parts;
	TMap<EWREquipPart, FWREquipmentAsyncTask*> AsyncTasks;

	FWREquipmentDirection Director;
};

template <typename T>
void UWRActorComponentEquipmentLook::SetMaterialParameterValue(FName InParamName, T InValue)
{
	for (auto& Part : Parts)
	{
		Part.Value->SetMaterialParameterValue(InParamName, InValue);
	}
}

template <typename T>
void UWRActorComponentEquipmentLook::SetMaterialParameterValue(FName InParamName, T InValue, const EWREquipPart InPart)
{
	if (Parts.Contains(InPart) == false)
	{
		return;
	}
	Parts[InPart]->SetMaterialParameterValue(InParamName, InValue);
}

//#define WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK 1
//
//DECLARE_DELEGATE_TwoParams(FWREquipCompleteDelegate, AActor*, EWREquipPart);
//
//struct FWREquippedMeshComponentWrapping
//{
//public:
//
//	FWREquippedMeshComponentWrapping(AWRCharacter* InOwner) :Owner(InOwner), AdditionalAbility(EWRAdditionalAbility::None), AttachSocketName(NAME_None) {}
//	virtual ~FWREquippedMeshComponentWrapping() {/* EquipmentLookTable = nullptr;*/ }
//
//
//	// [ 2019-10-1 : magedoga ] Set
//	virtual void SetMesh(class UStaticMesh* InMesh) { this->CleanupOverrideMaterials(); }
//	virtual void SetMesh(class USkeletalMesh* InMesh) { this->CleanupOverrideMaterials(); }
//	virtual void SetNullMesh() { this->CleanupOverrideMaterials(); }
//
//	virtual void SetVisibility(const bool bInVisible);
//	bool IsVisible();
//
//	void SetMaterialParameterValue(FName InParamName, float InValue, UMeshComponent* InMeshComponent = nullptr);
//	void SetMaterialParameterValue(FName InParamName, FLinearColor InValue, UMeshComponent* InMeshComponent = nullptr);
//	bool SetCollisionEnable(ECollisionEnabled::Type InCollisionEnableType);
//	//====================================================================================
//
//	// [ 2019-10-1 : magedoga ] Get
//	virtual UStaticMesh* GetStaticMesh() { return nullptr; }
//	virtual USkeletalMesh* GetSkeletalMesh() { return nullptr; }
//	virtual UMeshComponent* GetMeshComponent() { return nullptr; }
//
//	EWRAdditionalAbility GetAdditionalAbilityType() const { return AdditionalAbility; }
//	FName GetAttachSocketName() const { return AttachSocketName; }
//	FTransform GetAttachSocketTransform(ERelativeTransformSpace InSpace) const;
//	//====================================================================================
//
//	virtual bool AttachActor(AActor* InActor, FName InSocketName) { return false; };
//	virtual bool DetachActor(AActor* InActor) { return false; };
//
//	void SettingForEquip(FWREquipmentLookTable* InTable);
//	void SettingForEquip(FWREquipmentLookTable* InTable, USkeletalMesh* InMergedSkeletalMesh);
//
//	bool IsBodyPart(EWREquipPart InPart) const { return InPart == EWREquipPart::Body; }
//	bool IsWeaponPart(EWREquipPart InPart) const { return InPart == EWREquipPart::RightHand || InPart == EWREquipPart::LeftHand; }
//
//protected:
//
//	virtual void CreateComponent() {}
//	void DestroyComponent();
//
//	void CreateAdditionalAbility(EWRAdditionalAbility InAdditionalAbility, FWREquipmentLookTable* InTable);
//	void ReleaseAdditionalAbility();
//
//	void CleanupOverrideMaterials() { this->GetMeshComponent()->EmptyOverrideMaterials(); }
//
//	virtual void GetMeshMaterial(TArray<UMaterialInterface*>& OutMaterial) {}
//
//	virtual void Setting_Internal(FWREquipmentLookTable* InTable) {}
//	virtual void Setting_Internal(FWREquipmentLookTable* InTable, USkeletalMesh* InMergedSkeletalMesh) {}
//
//protected:
//
//	AWRCharacter* Owner;
//	EWRAdditionalAbility AdditionalAbility;
//	FName AttachSocketName;
//
//
//#if WITH_EDITOR
//private:
//	static TMap<EWREquipPart, uint32>	MeshComponentUniqueID;
//protected:
//	FString MakeComponentNameForDetailPanel(EWREquipPart InPart);
//#endif // WITH_EDITOR
//};
//
////====================================================================================
//
//
////====================================================================================
//// [ 2019-9-19 : magedoga ] Static Mesh
//
//struct FWREquippedMeshComponentWrapping_StaticMesh : public FWREquippedMeshComponentWrapping
//{
//public:
//
//	FWREquippedMeshComponentWrapping_StaticMesh(FWREquipmentLookTable* InTable, AWRCharacter* InOwner) : FWREquippedMeshComponentWrapping(/*InTable,*/ InOwner)
//	{
//		CreateComponent();
//	}
//
//	virtual ~FWREquippedMeshComponentWrapping_StaticMesh()
//	{
//		DestroyComponent();
//		StaticMeshComponent = nullptr;
//	}
//
//	virtual void CreateComponent() override
//	{
//		if (StaticMeshComponent == nullptr)
//			StaticMeshComponent = NewObject<UStaticMeshComponent>(Owner);
//	}
//
//	// [ 2019-10-1 : magedoga ] Set
//	virtual void SetMesh(class UStaticMesh* InMesh) override;
//	virtual void SetNullMesh() override;
//
//	// [ 2019-10-1 : magedoga ] Get
//	virtual UStaticMesh* GetStaticMesh() override { return StaticMeshComponent->GetStaticMesh(); }
//	virtual UMeshComponent* GetMeshComponent() override { return StaticMeshComponent; }
//
//	virtual bool AttachActor(AActor* InActor, FName InSocketName) override;
//	virtual bool DetachActor(AActor* InActor) override;
//
//protected:
//
//	virtual void Setting_Internal(FWREquipmentLookTable* InTable) override;
//	virtual void Setting_Internal(FWREquipmentLookTable* InTable, USkeletalMesh* InMergedSkeletalMesh) override;
//
//private:
//
//	class UStaticMeshComponent* StaticMeshComponent = nullptr;
//};
//
////====================================================================================
//
//
////====================================================================================
//// [ 2019-9-19 : magedoga ] Skeletal Mesh
//
//struct FWREquippedMeshComponentWrapping_SkeletalMesh : public FWREquippedMeshComponentWrapping
//{
//public:
//	FWREquippedMeshComponentWrapping_SkeletalMesh(FWREquipmentLookTable* InTable, AWRCharacter* InOwner) : FWREquippedMeshComponentWrapping(/*InTable,*/ InOwner)
//	{
//		EquippedPart = InTable->EquipPart;
//
//		if (this->IsBodyPart(EquippedPart) == false)
//		{
//			CreateComponent();
//		}
//		else
//		{
//			SkeletalMeshComponent = InOwner->GetMesh();
//		}
//	}
//
//	virtual ~FWREquippedMeshComponentWrapping_SkeletalMesh()
//	{
//		if (this->IsBodyPart(EquippedPart) == false)
//		{
//			DestroyComponent();
//		}
//		SkeletalMeshComponent = nullptr;
//	}
//
//	virtual void CreateComponent() override
//	{
//		if (SkeletalMeshComponent == nullptr)
//			SkeletalMeshComponent = NewObject<USkeletalMeshComponent>(Owner);
//	}
//
//	// [ 2019-10-1 : magedoga ] Set
//	virtual void SetMesh(class USkeletalMesh* InMesh) override;
//	virtual void SetNullMesh() override;
//
//	// [ 2019-10-1 : magedoga ] Get
//	virtual USkeletalMesh* GetSkeletalMesh() override { return SkeletalMeshComponent->SkeletalMesh; }
//	virtual UMeshComponent* GetMeshComponent() override { return SkeletalMeshComponent; }
//
//	virtual bool AttachActor(AActor* InActor, FName InSocketName) override;
//	virtual bool DetachActor(AActor* InActor) override;
//
//protected:
//
//	virtual void Setting_Internal(FWREquipmentLookTable* InTable) override;
//	virtual void Setting_Internal(FWREquipmentLookTable* InTable, USkeletalMesh* InMergedSkeletalMesh) override;
//
//private:
//
//	class USkeletalMeshComponent* SkeletalMeshComponent = nullptr;
//	EWREquipPart EquippedPart;
//
//};
//
////====================================================================================
//
//#ifdef ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//DECLARE_DELEGATE_TwoParams(FWROnCompleteEquipLookChangeDirect, FWREquipmentLookTable*, EWREquipPart)
//#else // ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//DECLARE_DELEGATE_OneParam(FWROnCompleteEquipLookChangeDirect, FWREquipmentLookTable*)
//#endif // ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//
//#define WR_CHANGEEQUIP_MULTI_DIRECT 1
//
//#if WR_CHANGEEQUIP_MULTI_DIRECT
//struct FWREquipDirecteData
//{
//public:
//	FWREquipDirecteData() {}
//
//	AWRCharacter* TargetCharacter = nullptr;
//	FWREquipmentLookTable* NextEquipLookTable = nullptr;
//	FWROnCompleteEquipLookChangeDirect CompleteCallback;
//
//	UMeshComponent* CurrentPartMeshComponent = nullptr;
//	UMeshComponent* ChangePartMeshComponent = nullptr;
//
//	//UPROPERTY(VisibleAnywhere, Category = "WREquipment")
//	UCurveFloat* CurveSRC = nullptr;
//	//UPROPERTY(VisibleAnywhere, Category = "WREquipment")
//	UCurveFloat* CurveDEST = nullptr;
//
//	float AccTime;
//
//	EWREquipmentLookAction Action = EWREquipmentLookAction::None;
//	EWREquipPart Part = EWREquipPart::None;
//};
//#endif // WR_CHANGEEQUIP_MULTI_DIRECT
//
//// [ 2020-2-10 : magedoga ] Change Director
//struct FWREquipmentLookChangeDirector
//{
//public:
//
//	FWREquipmentLookChangeDirector()
//	{
//		TickHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FWREquipmentLookChangeDirector::TickDirect));
//	}
//	~FWREquipmentLookChangeDirector()
//	{
//		if (TickHandle.IsValid() == true)
//		{
//			FTicker::GetCoreTicker().RemoveTicker(TickHandle);
//			TickHandle.Reset();
//		}
//	}
//
//#ifdef ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//	bool StartDirect(AWRCharacter* InCharacter, UMeshComponent* InCurrentMeshComponent, FWREquipmentLookTable* InTable, FWROnCompleteEquipLookChangeDirect InCallback, EWREquipPart InPart = EWREquipPart::None);
//	void StopDirect();
//#else // ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//	bool StartDirect(AWRCharacter* InTargetCharacter, UMeshComponent* InCurrentPartMeshComponent, FWREquipmentLookTable* InTable, FWROnCompleteEquipLookChangeDirect InCompleteCallback);
//	void StopDirect();
//#endif // ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//
//#if WR_CHANGEEQUIP_MULTI_DIRECT
//	bool IsDirecting() { return DirectingDatas.Num(); }
//#else // WR_CHANGEEQUIP_MULTI_DIRECT
//	bool IsDirecting() { return TickHandle.IsValid(); }
//#endif // WR_CHANGEEQUIP_MULTI_DIRECT
//
//private:
//
//	bool IsStaticMesh(const EWREquipPart InPart) {
//		return InPart < EWREquipPart::Body;
//	}
//
//#ifdef ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//	bool TickDirect(float DeltaTime);
//#else // ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//	bool TickDirect(float InDeltaTime);
//#endif // ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//
//	void CreateStaticMeshComponent(AWRCharacter* InCharacter, FWREquipDirecteData* InDirectingData, FWREquipmentLookTable* InTable);
//	void CreateSkeletalMeshComponent(AWRCharacter* InCharacter, FWREquipDirecteData* InDirectingData, FWREquipmentLookTable* InTable);
//
//#if WR_CHANGEEQUIP_MULTI_DIRECT
//	void SetParameter(FName InParameterName, float InSRCValue, float InDESTValue, FWREquipDirecteData* InDirectData);
//	void OnPreEndDirect(FWREquipDirecteData* InData);
//	void OnPostEndDirect(FWREquipDirecteData* InData);
//#else
//	void SetParameter(FName InParameterName, float InSRCValue, float InDESTValue);
//	void OnPreEndDirect();
//	void OnPostEndDirect();
//#endif // WR_CHANGEEQUIP_MULTI_DIRECT
//
//private:
//
//#if WR_CHANGEEQUIP_MULTI_DIRECT
//	TDoubleLinkedList<FWREquipDirecteData*>	DirectingDatas;
//#else // WR_CHANGEEQUIP_MULTI_DIRECT
//	AWRCharacter* TargetCharacter;
//	FWREquipmentLookTable* NextEquipLookTable;
//	FWROnCompleteEquipLookChangeDirect CompleteCallback;
//
//	UMeshComponent* CurrentPartMeshComponent;
//	UMeshComponent* ChangePartMeshComponent;
//
//	//UPROPERTY(VisibleAnywhere, Category="WREquipment")
//	UCurveFloat* CurveSRC;
//
//	//UPROPERTY(VisibleAnywhere, Category = "WREquipment")
//	UCurveFloat* CurveDEST;
//
//	float AccTime;
//#endif // WR_CHANGEEQUIP_MULTI_DIRECT
//	FDelegateHandle TickHandle;
//};
//
////====================================================================================
//
//#if WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//struct FWREquipAsyncTask
//{
//public:
//
//	FWREquipAsyncTask()
//	{
//		RequestedTable = nullptr;
//	}
//
//	FWREquipAsyncTask(const FString& InHashKey) : FWREquipAsyncTask()
//	{
//		this->AddHash(InHashKey);
//	}
//
//	~FWREquipAsyncTask()
//	{
//		for (auto It = LoadedObjects.CreateIterator(); It; ++It)
//		{
//			UObject* Object = It.Value();
//			if (Object->IsValidLowLevel() == true && Object->IsRooted() == true)
//			{
//				Object->RemoveFromRoot();
//			}
//		}
//
//		LoadedObjects.Empty();
//		RequestedTable = nullptr;
//		EquipCompleteDelegate.Unbind();
//	}
//
//	void AddHash(const FString& InHashKey)
//	{
//		if (LoadedObjects.Contains(InHashKey) == true)
//			return;
//
//		LoadedObjects.Add(InHashKey, nullptr);
//	}
//
//	void AddObject(const FString& InHashKey, UObject* InObject)
//	{
//		this->AddHash(InHashKey);
//
//		LoadedObjects[InHashKey] = InObject;
//	}
//
//	bool ContainsHash(const FString& InHashKey)
//	{
//		return LoadedObjects.Contains(InHashKey);
//	}
//
//	bool HasObject(const FString& InHashKey)
//	{
//		if (this->ContainsHash(InHashKey) == false)
//			return false;
//
//		return LoadedObjects[InHashKey] != nullptr;
//	}
//
//	bool HasDirect()
//	{
//		bool bIsNull = RequestedTable->ChangeCurveSRC.IsNull();
//		return !bIsNull;
//	}
//
//	void ExcuteIfBound(AActor* InActor)
//	{
//		EquipCompleteDelegate.ExecuteIfBound(InActor, RequestedTable->EquipPart);
//	}
//
//public:
//
//	TMap<FString, UObject*>	LoadedObjects;
//	FWREquipmentLookTable* RequestedTable;
//	FWREquipCompleteDelegate EquipCompleteDelegate;
//};
//#endif // WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//
//**
//*
// */
//UCLASS()
//class WR_API UWRActorComponentEquipmentLook : public UWRActorComponent
//{
//	GENERATED_BODY()
//
//public:
//
//	virtual void OnCreateComponent() override {};
//	virtual void OnDestroyComponent() override;
//
//#ifdef ADD_MERGE_SKELETALMESH_BY_ANIMAL14_
//	bool Equip(WRTableID InTableID, USkeletalMesh* InMergedSkeletalMesh = nullptr, FWREquipCompleteDelegate InDelegate = nullptr);
//	bool Equip(FWREquipmentLookTable* InTable, USkeletalMesh* InMergedSkeletalMesh = nullptr, FWREquipCompleteDelegate InDelegate = nullptr);
//#else // ADD_MERGE_SKELETALMESH_BY_ANIMAL14_
//	bool Equip(WRTableID InTableID, FWREquipCompleteDelegate InEquipeCompleteDelegate = nullptr);
//	bool Equip(FWREquipmentLookTable* InTable, FWREquipCompleteDelegate InEquipeCompleteDelegate = nullptr);
//#endif // ADD_MERGE_SKELETALMESH_BY_ANIMAL14_
//
//	void UnEquip(EWREquipPart InPart);
//
//	bool IsEquipedVisible(EWREquipPart InPart);
//
//	bool IsDirecting() { return ChangeEquipLookDirector.IsDirecting(); }
//
//	// [ 2019-10-1 : magedoga ] Set
//	template <typename TParameter>
//	void SetMaterialParameterValue(FName InParamName, TParameter InValue);
//	template <typename TParameter>
//	void SetMaterialParameterValue(FName InParamName, TParameter InValue, EWREquipPart InPart);
//
//	void SetVisibility(EWREquipPart InPart, const bool bInVisible);
//	bool SetCollisionEnable(EWREquipPart InPart, ECollisionEnabled::Type InCollisionEnableType);
//
//	void SetMesh(EWREquipPart InPart, class UStaticMesh* InStaticMesh);
//	void SetMesh(EWREquipPart InPart, class USkeletalMesh* InSkeletalMesh);
//
//	// [ 2019-10-1 : magedoga ] Get
//	FName GetAttachSocketName(EWREquipPart InPart);
//	FTransform GetAttachSocketTransform(EWREquipPart InPart, ERelativeTransformSpace InSpace);
//	UStaticMesh* GetStaticMesh(EWREquipPart InPart);
//	USkeletalMesh* GetSkeletalMesh(EWREquipPart InPart);
//	UMeshComponent* GetMeshComponent(EWREquipPart InPart);
//	EWRAdditionalAbility GetAdditionalAbility(EWREquipPart InPart);
//
//	void Update_AttachItemToSuitableMesh(class AWRCharacterMine* InContext);
//
//	// 	bool AttachActorTo(EWREquipPart InPart, AActor* InActor, FName InSocketName);
//	// 	bool DetachActorFrom(EWREquipPart InPart, AActor* InActor);
//
//private:
//#ifdef ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//	void OnEndChangeEquipLookDirect(FWREquipmentLookTable* InTable, EWREquipPart InPart);
//#else // ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//	void OnEndChangeEquipLookDirect(FWREquipmentLookTable* InTable);
//#endif // ADD_DIRECTING_EQUIPMENTLOOK_BY_ANIMAL14_
//	void OnDirectionComplete(FWREquipmentLookTable* InTable, EWREquipPart InPart);
//
//	UFUNCTION()
//		void OnAsyncLoadComplete(UObject* InObject, FString InHashKey);
//
//#if WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//	bool NextStepAfterAsyncLoad(FWREquipmentLookTable* InTable);
//#else // WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//	void NextStepAfterAsyncLoad();
//#endif // WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//
//private:
//
//	TMap<EWREquipPart, FWREquippedMeshComponentWrapping*>	EquippedPart;
//
//	FWREquipmentLookChangeDirector ChangeEquipLookDirector;
//
//	// [ 2020-4-28 : magedoga ] for async
//#if WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//
//	TMap<EWREquipPart, FWREquipAsyncTask*>	AsyncTasks;
//
//#else // WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//
//	FWREquipmentLookTable* RequestedTable = nullptr;
//	FWREquipCompleteDelegate EquipCompleteDelegate;
//
//	TMap<FString, UObject*> AsyncTasks;
//
//#endif // WR_EQUIPMENTLOOK_MULTI_ASYNC_TASK
//
//};
//
//template <typename TParameter>
//void UWRActorComponentEquipmentLook::SetMaterialParameterValue(FName InParamName, TParameter InValue)
//{
//	for (auto It = EquippedPart.CreateIterator(); It; ++It)
//	{
//		It.Value()->SetMaterialParameterValue(InParamName, InValue);
//	}
//}
//
//template <typename TParameter>
//void UWRActorComponentEquipmentLook::SetMaterialParameterValue(FName InParamName, TParameter InValue, EWREquipPart InPart)
//{
//	if (EquippedPart.Contains(InPart) == false)
//	{
//		return;
//	}
//	EquippedPart[InPart]->SetMaterialParameterValue(InParamName, InValue);
//}
//
//// For Header
//DECLARE_LOG_CATEGORY_EXTERN(EquipmentLook, Display, All);