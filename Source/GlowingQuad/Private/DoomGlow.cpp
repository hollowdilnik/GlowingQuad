// Fill out your copyright notice in the Description page of Project Settings.


#include "DoomGlow.h"

#include "DrawDebugHelpers.h"
#include "ProceduralMeshComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "UObject/ConstructorHelpers.h"

#if WITH_EDITOR
#include "LevelEditorViewport.h"
#endif

// Sets default values
ADoomGlow::ADoomGlow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProceduralMeshComp = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh"));
	RootComponent = ProceduralMeshComp;

	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialFinder(TEXT("Material'/GlowingQuad/Materials/M_DoomGlow.M_DoomGlow'"));
	if (MaterialFinder.Succeeded())
	{
		GlowMaterial = MaterialFinder.Object;
		QuadMaterial = GlowMaterial;
	}
	
	QuadPoints.SetNumZeroed(4);
	VertexBuffer.SetNumZeroed(16);
	ColorBuffer.SetNumZeroed(16);
	QuadVertexBuffer.SetNumZeroed(4);
	QuadColorBuffer.SetNumZeroed(4);
}

// Called every frame
void ADoomGlow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Everything is in local space unless said otherwise
	FVector CameraPosition;
	if (!GetCameraLocation(CameraPosition))
	{
		return;
	}

	// We don't need to draw anything if the glow material is null
	if (!GlowMaterial)
	{
		return;
	}
	
	for (int i = 0; i < 4; ++i)
	{
		VertexBuffer[i] = QuadPoints[i];
	}
	
	const FVector U = VertexBuffer[1] - VertexBuffer[0];
	const FVector V = VertexBuffer[2] - VertexBuffer[0];
	const FVector QuadNormal = (U ^ V).GetUnsafeNormal();
	const FVector CenterPoint = 0.25f * (VertexBuffer[0] + VertexBuffer[1] + VertexBuffer[2] + VertexBuffer[3]);
	
	const float Dot = (CenterPoint - CameraPosition).GetSafeNormal() | QuadNormal;
	float Sign = FMath::Sign(Dot);
	const float Alpha = FMath::GetMappedRangeValueClamped(AngleFadeRange, {0.0f, 1.0f}, FMath::Abs(Dot));
	
	if (Dot < 0 && bShowBottom)
	{
		// Just flip the quad
		Swap(VertexBuffer[1], VertexBuffer[3]);
		Sign = -Sign;
	}

	FVector EyeToPointWS[4]; // in world space
	for (int i = 0; i < 4; ++i)
	{
		EyeToPointWS[i] = ProceduralMeshComp->GetComponentTransform().TransformVector(VertexBuffer[i] - CameraPosition);
		EyeToPointWS[i].Normalize();
	}

	// TODO: Don't draw the glow if the PushDistance ends up being zero?
	float PushDistance = GlowSize;
	if (DistanceFadeCurve)
	{
		PushDistance *= DistanceFadeCurve->GetFloatValue(FVector::Distance(CameraPosition, CenterPoint));
	}

	// Extrude quad vertices
	for (int i = 0; i < 4; ++i)
	{
		FVector PushDirWS[3]; // in world space
		PushDirWS[0] = Sign * (EyeToPointWS[i] ^ EyeToPointWS[(i+3)%4]).GetSafeNormal();
		PushDirWS[1] = Sign * (EyeToPointWS[(i+1)%4] ^ EyeToPointWS[i]).GetSafeNormal();
		PushDirWS[2] = (PushDirWS[0] + PushDirWS[1]).GetSafeNormal();

		for (int j = 0; j < 3; ++j)
		{
			FVector Offset = PushDistance * PushDirWS[j];
			if (bGlowSizeIgnoresScale)
			{
				Offset = ProceduralMeshComp->GetComponentTransform().InverseTransformVector(Offset);
			}
			else
			{
				Offset = ProceduralMeshComp->GetComponentTransform().InverseTransformVectorNoScale(Offset);
			}
			VertexBuffer[4+j+3*i] = VertexBuffer[i] + Offset;
		}
	}

	const FLinearColor ColorFilled = FLinearColor{FillColor.R, FillColor.G, FillColor.B, Alpha * FillColor.A};
	const FLinearColor ColorEdge = FLinearColor{GlowColor.R, GlowColor.G, GlowColor.B, 0.0f};

	for (int i = 0; i < 4; ++i)
	{
		ColorBuffer[i] = ColorFilled;
	}
	for (int i = 4; i < 16; ++i)
	{
		ColorBuffer[i] = ColorEdge;
	}
	
	ProceduralMeshComp->UpdateMeshSection_LinearColor(0, VertexBuffer, {}, {}, ColorBuffer, {});

	if (QuadMaterial && QuadMaterial != GlowMaterial)
	{
		// We need to draw the quad separately
		for (int i = 0; i < 4; ++i)
		{
			QuadVertexBuffer[i] = VertexBuffer[i];
			QuadColorBuffer[i] = ColorFilled;
		}
		
		TArray<FVector2D> QuadUVs = {
			{1.0f, 1.0f},
			{1.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 1.0f}
		};
		if (Dot < 0 && bShowBottom)
		{
			Swap(QuadUVs[1], QuadUVs[3]);
		}
		
		ProceduralMeshComp->UpdateMeshSection_LinearColor(1, QuadVertexBuffer, {}, QuadUVs, QuadColorBuffer, {});
	}

	DrawDebugEdges();
}

void ADoomGlow::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	Init();
}

void ADoomGlow::SetQuadSize(FVector2D NewSize)
{
	QuadSize = NewSize;
	UpdateQuadPoints();
}

void ADoomGlow::SetQuadPoints(const TArray<FVector>& NewQuadPoints)
{
	if (!ensure(NewQuadPoints.Num() == 4))
	{
		return;
	}

	QuadPoints = NewQuadPoints;
	QuadSize = {0.0f, 0.0f};
}

void ADoomGlow::SetMaterials(UMaterialInterface* NewGlowMaterial, UMaterialInterface* NewQuadMaterial)
{
	if (NewGlowMaterial == GlowMaterial && NewQuadMaterial == QuadMaterial)
	{
		return;
	}
	
	GlowMaterial = NewGlowMaterial;
	QuadMaterial = NewQuadMaterial;

	// We might need to recreate the procedural mesh sections, so just re-init everything
	// TODO: don't recreate the mesh if the topology didn't change
	Init();
}

#if WITH_EDITOR
void ADoomGlow::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	Init();
}
#endif

bool ADoomGlow::GetCameraLocation(FVector& OutCameraLocation)
{
	// Try getting player camera
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (APlayerCameraManager* PCM = PC->PlayerCameraManager)
		{
			OutCameraLocation = PCM->GetCameraLocation();
			OutCameraLocation = ProceduralMeshComp->GetComponentTransform().InverseTransformPosition(OutCameraLocation);
			return true;
		}
	}
	
#if WITH_EDITOR
	// Try getting editor camera
	if (FViewport* ActiveViewport = GEditor->GetActiveViewport())
	{
		if (auto* ViewportClient = StaticCast<FLevelEditorViewportClient*>(ActiveViewport->GetClient()))
		{
			OutCameraLocation = ViewportClient->GetViewLocation();
			OutCameraLocation = ProceduralMeshComp->GetComponentTransform().InverseTransformPosition(OutCameraLocation);
			return true;
		}
	}
#endif
	
	return false;
}

void ADoomGlow::Init()
{
	UpdateQuadPoints();
	
	if (GlowMaterial == QuadMaterial)
	{
		// Draw them together
		IndexBuffer = {0,1,2, 0,2,3, // Quad
					   0,5,7, 0,7,1, 1,8,10, 1,10,2, 2,11,13, 2,13,3, 3,14,4, 3,4,0, // Flaps
					   0,4,6, 0,6,5, 1,7,9, 1,9,8, 2,10,12, 2,12,11, 3,13,15, 3,15,14}; // Connections
	}
	else
	{
		// Draw just the glow
		IndexBuffer = {0,5,7, 0,7,1, 1,8,10, 1,10,2, 2,11,13, 2,13,3, 3,14,4, 3,4,0, // Flaps
					   0,4,6, 0,6,5, 1,7,9, 1,9,8, 2,10,12, 2,12,11, 3,13,15, 3,15,14}; // Connections
	}

	ProceduralMeshComp->ClearAllMeshSections();
	
	if (GlowMaterial)
	{
		ProceduralMeshComp->CreateMeshSection_LinearColor(0, VertexBuffer, IndexBuffer, {}, {}, ColorBuffer, {}, false);
		ProceduralMeshComp->SetMaterial(0, GlowMaterial);
	}
	
	if (QuadMaterial && QuadMaterial != GlowMaterial)
	{
		// We need to draw the quad separately
		TArray<FVector2D> QuadUVs = {
			{1.0f, 1.0f},
			{1.0f, 0.0f},
			{0.0f, 0.0f},
			{0.0f, 1.0f}
		};
		ProceduralMeshComp->CreateMeshSection_LinearColor(1, QuadVertexBuffer, {0,1,2, 0,2,3}, {}, QuadUVs, QuadColorBuffer, {}, false);
		ProceduralMeshComp->SetMaterial(1, QuadMaterial);
	}
}

void ADoomGlow::DrawDebugEdges()
{
#if ENABLE_DRAW_DEBUG
	// Wireframe visualization
	if (bDrawDebugEdges)
	{
		const FTransform WorldToLocal = ProceduralMeshComp->GetComponentTransform();
		for (int i = 0; i < 4; ++i)
		{
			DrawDebugLine(GetWorld(), WorldToLocal.TransformPosition(VertexBuffer[5+3*i]), WorldToLocal.TransformPosition(VertexBuffer[4+(i+1)%4*3]), FColor::Blue);
			DrawDebugLine(GetWorld(), WorldToLocal.TransformPosition(VertexBuffer[4+3*i]), WorldToLocal.TransformPosition(VertexBuffer[6+3*i]), FColor::Blue);
			DrawDebugLine(GetWorld(), WorldToLocal.TransformPosition(VertexBuffer[5+3*i]), WorldToLocal.TransformPosition(VertexBuffer[6+3*i]), FColor::Blue);
			DrawDebugLine(GetWorld(), WorldToLocal.TransformPosition(VertexBuffer[i]), WorldToLocal.TransformPosition(VertexBuffer[(i+1)%4]), FColor::Blue);
		
			for (int j = 0; j < 3; ++j)
			{
				DrawDebugLine(GetWorld(), WorldToLocal.TransformPosition(VertexBuffer[i]), WorldToLocal.TransformPosition(VertexBuffer[4+j+3*i]), FColor::Blue);
			}
		}
	}
#endif
}

void ADoomGlow::UpdateQuadPoints()
{
	if (QuadSize.X > 0 && QuadSize.Y > 0)
	{
		// If width or height are zero, someone else will probably take care of it
		QuadPoints = {
			{-QuadSize.X / 2, -QuadSize.Y / 2, 0},
			{ QuadSize.X / 2, -QuadSize.Y / 2, 0},
			{ QuadSize.X / 2,  QuadSize.Y / 2, 0},
			{-QuadSize.X / 2,  QuadSize.Y / 2, 0}
		};
	}
}
