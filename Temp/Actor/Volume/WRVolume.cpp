// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRVolume.h"

#include "Components/BrushComponent.h"

#include "Model.h"

#include "Engine/Brush.h"
#include "Engine/Polys.h"

#include "Kismet/KismetSystemLibrary.h"

AWRVolume::AWRVolume(const FObjectInitializer& ObjectInitializer) : AVolume(ObjectInitializer)
{
#if UE_EDITOR
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
#else // UE_EDITOR
#endif // UE_EDITOR
}

void AWRVolume::Tick(float DeltaSeconds)
{
	if (IsDebug == false)
	{
		return;
	}

	UModel* Model = this->GetBrushComponent()->Brush;
#if WITH_EDITOR
	if (Model->Polys != nullptr)
	{
		for (int32 i = 0; i < Model->Polys->Element.Num(); ++i)
		{
			FPoly Poly = Model->Polys->Element[i];
			for (int32 j = 0; j < Poly.Vertices.Num(); ++j)
			{
				FVector LineStart = this->GetActorRotation().RotateVector(this->GetActorScale3D() * Poly.Vertices[j]) + this->GetActorLocation();
				FVector LineEnd = this->GetActorRotation().RotateVector(this->GetActorScale3D() * Poly.Vertices[(j + 1) % Poly.Vertices.Num()]) + this->GetActorLocation();

				UKismetSystemLibrary::DrawDebugLine(this, LineStart, LineEnd, FLinearColor::Green, 0.0F, Thickness);
			}
		}
	}
#endif //WITH_EDITOR

	Super::Tick(DeltaSeconds);
}