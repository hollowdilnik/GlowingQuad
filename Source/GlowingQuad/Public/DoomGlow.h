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
	virtual void OnConstruction(const FTransform& Transform) override;

	// Updates the quad size and points
	UFUNCTION(BlueprintCallable)
	void SetQuadSize(FVector2D NewSize);

	// Manually set the quad points. Expects 4 points. The index buffer for the quad is {0,1,2, 0,2,3}.
	UFUNCTION(BlueprintCallable)
	void SetQuadPoints(const TArray<FVector>& NewQuadPoints);

	// Updates the materials for the procedural mesh component
	// This regenerates the procedural mesh, so might be expensive
	UFUNCTION(BlueprintCallable)
	void SetMaterials(UMaterialInterface* NewGlowMaterial, UMaterialInterface* NewQuadMaterial);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor GlowColor{0.0f, 1.0f, 1.0f, 1.0f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor FillColor{1.0f, 1.0f, 1.0f, 1.0f};

	// The width of the glow effect in local space
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GlowSize{30.0f};

	// Should the glow size ignore actor's scale and always be in world space units?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bGlowSizeIgnoresScale{false};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* DistanceFadeCurve;

	UPROPERTY(EditDefaultsOnly)
	bool bTickInViewports{true};

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* ProceduralMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDrawDebugEdges{false};
	
protected:
	UPROPERTY(BlueprintReadOnly)
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
	void UpdateQuadPoints();
};
