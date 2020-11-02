#include "WREnvInterior.h"
#include "Components/BoxComponent.h"
#include "WREnvActor.h"

AWREnvInterior::AWREnvInterior()
{
	EnvType = EWREnvType::Interior;
}

AWREnvInterior::AWREnvInterior(EWREnvType InEnvType)
{

}

void AWREnvInterior::BeginPlay()
{
	Super::BeginPlay();
}

void AWREnvInterior::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWREnvInterior::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
// 	if (bIsOpened == false)
// 	{
// 		bIsOpened = true;
// 
// 		if (OpenSound.IsNull() == false)
// 		{
// 			USoundBase* OpenSoundBase = Cast<USoundBase>(OpenSound.LoadSynchronous());
// 
// 			WRSFXManager::Get()->SpawnAtLocation(OpenSoundBase, this->GetActorLocation());
// 		}
// 	}
}

void AWREnvInterior::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
// 	if (bIsOpened == true)
// 	{
// 		bIsOpened = false;
// 		if (CloseSound.IsNull() == false)
// 		{
// 			USoundBase* CloseSoundBase = Cast<USoundBase>(CloseSound.LoadSynchronous());
// 
// 			WRSFXManager::Get()->SpawnAtLocation(CloseSoundBase, this->GetActorLocation());
// 		}
// 	}
}

void AWREnvInterior::RegistOverlapEvent()
{
	UBoxComponent* BoxComponent = Cast<UBoxComponent>(this->GetComponentByClass(UBoxComponent::StaticClass()));
	if (BoxComponent)
	{
		BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		BoxComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Ignore);
		BoxComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel5, ECR_Ignore);
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AWREnvInterior::OnBeginOverlap);
		BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AWREnvInterior::OnEndOverlap);
	}
}

