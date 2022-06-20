// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GlowUtils.generated.h"

/**
 * 
 */
UCLASS()
class GLOWINGQUAD_API UGlowUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static void GenerateGlowMesh(const TArray<FVector> &Points, TArray<int> &IndexBuffer, TArray<FVector> &VertexBuffer, TArray<FLinearColor> &ColorBuffer, TArray<FVector2D> &UV1, TArray<FVector2D> &UV2, float Eps=0.1f);
	
	UFUNCTION(BlueprintCallable)
	static void GenerateSplitGlowMesh(const TArray<FVector> &Points,
		TArray<int> &FillIndexBuffer, TArray<FVector> &FillVertexBuffer, TArray<FLinearColor> &FillColorBuffer, TArray<FVector2D> &FillUV1, TArray<FVector2D> &FillUV2,
		TArray<int> &GlowIndexBuffer, TArray<FVector> &GlowVertexBuffer, TArray<FLinearColor> &GlowColorBuffer, TArray<FVector2D> &GlowUV1, TArray<FVector2D> &GlowUV2,
		float Eps=0.1f);
};
