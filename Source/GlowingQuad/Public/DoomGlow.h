// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoomGlow.generated.h"

class UProceduralMeshComponent;

UCLASS()
class GLOWINGQUAD_API ADoomGlow : public AActor
{
	GENERATED_BODY()
public:	
	ADoomGlow();

	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override { return bTickInViewports; };
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor GlowColor{0.0f, 1.0f, 1.0f, 1.0f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor FillColor{1.0f, 1.0f, 1.0f, 1.0f};

	// The width of the glow effect in local space
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GlowSize{30.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D QuadSize{100.0f, 100.0f};

	// One-sided or two-sided quad
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowBottom{false};

	UPROPERTY(EditAnywhere)
	UMaterialInterface* GlowMaterial;

	// If this is the same as GlowMaterial, they will be drawn as one mesh
	// If this material is different, the quad will be drawn as a separate section
	// If this material is null, the quad will not be drawn
	UPROPERTY(EditAnywhere)
	UMaterialInterface* QuadMaterial;

	// Glow will fade from 1 to 0 when the cosine of the glancing angle goes from Y to X
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D AngleFadeRange{0.001f, 0.1f};

	// Use this to fade the Glow Size depending on the distance between the camera and the center of the quad
	UPROPERTY(EditAnywhere)
	UCurveFloat* DistanceFadeCurve;

	UPROPERTY(EditDefaultsOnly)
	bool bTickInViewports{true};

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* ProceduralMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDrawDebugEdges{false};
	
protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> QuadPoints;

private:
	TArray<int32_t> IndexBuffer;
	TArray<FVector> VertexBuffer;
	TArray<FLinearColor> ColorBuffer;
	
	TArray<FVector> QuadVertexBuffer;
	TArray<FLinearColor> QuadColorBuffer;

	bool GetCameraLocation(FVector& OutCameraLocation);
	void Init();
	void DrawDebugEdges();
};
