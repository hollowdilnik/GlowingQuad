// Fill out your copyright notice in the Description page of Project Settings.


#include "GlowUtils.h"

void UGlowUtils::GenerateGlowMesh(const TArray<FVector>& Points, TArray<int32_t>& IndexBuffer,
	TArray<FVector>& VertexBuffer, TArray<FLinearColor>& ColorBuffer, TArray<FVector2D>& UV1, TArray<FVector2D>& UV2, float Eps)
{
	const int N = Points.Num();
	VertexBuffer.SetNumUninitialized(4*N);
	ColorBuffer.SetNumUninitialized(4*N);
	UV1.SetNumUninitialized(4*N);
	UV2.SetNumUninitialized(4*N);
	IndexBuffer.SetNumUninitialized(3 * (N - 2 + N * 4));

	FVector CenterPoint = FVector::ZeroVector;

	// 1. Fill in the inner shape
	for (int i = 0; i < N; ++i)
	{
		VertexBuffer[i] = Points[i];
		ColorBuffer[i] = {0, 0, 0, 0};
		CenterPoint += Points[i];
	}
	CenterPoint /= N;
	
	// 2. Add the new points
	for (int i = 0; i < N; ++i)
	{
		const FVector NextDir = (Points[(i+1+N)%N] - Points[i]).GetSafeNormal();
		const FVector PrevDir = (Points[(i-1+N)%N] - Points[i]).GetSafeNormal();
		const FVector CenterDir = (CenterPoint - Points[i]).GetSafeNormal();
		
		// We need to offset them slightly so t hey don't get auto-merged...
		VertexBuffer[N+3*i+0] = Points[i] - Eps * NextDir;
		VertexBuffer[N+3*i+1] = Points[i] - Eps * PrevDir;
		VertexBuffer[N+3*i+2] = Points[i] - Eps * CenterDir;

		// Masks for us to know which vector offset to apply
		ColorBuffer[N+3*i+0] = {0, 1, 0, 0};
		ColorBuffer[N+3*i+1] = {1, 0, 0, 0};
		ColorBuffer[N+3*i+2] = {0, 0, 1, 0};

		UV1[i] = FVector2D(NextDir);
		UV1[N+3*i+0] = FVector2D(NextDir);
		UV1[N+3*i+1] = FVector2D(NextDir);
		UV1[N+3*i+2] = FVector2D(NextDir);
		
		UV2[i] = FVector2D(PrevDir);
		UV2[N+3*i+0] = FVector2D(PrevDir);
		UV2[N+3*i+1] = FVector2D(PrevDir);
		UV2[N+3*i+2] = FVector2D(PrevDir);
	}

	// 3. Make the index buffer
	// 3.1 Inner shape
	int j = 0;
	for (int i = 1; i < N - 1; ++i)
	{
		IndexBuffer[j++] = i;
		IndexBuffer[j++] = 0;
		IndexBuffer[j++] = i + 1;
	}
	// 3.2 Flaps
	for (int i = 0; i < N; ++i)
	{
		IndexBuffer[j++] = N + 3*i         + 1;
		IndexBuffer[j++] = i;
		IndexBuffer[j++] = N + 3*((i+1)%N) + 0;
		
		IndexBuffer[j++] = N + 3*((i+1)%N) + 0;
		IndexBuffer[j++] = i;
		IndexBuffer[j++] = (i+1)%N;
	}
	// 3.3 Connections
	for (int i = 0; i < N; ++i)
	{
		IndexBuffer[j++] = N + 3*i + 0;
		IndexBuffer[j++] = i;
		IndexBuffer[j++] = N + 3*i + 2;
		
		IndexBuffer[j++] = N + 3*i + 2;
		IndexBuffer[j++] = i;
		IndexBuffer[j++] = N + 3*i + 1;
	}

	// 4. Pack the UVs
	for (int i = 0; i < 4*N; ++i)
	{
		UV1[i] = 0.5f * UV1[i] + 0.5f;
		UV2[i] = 0.5f * UV2[i] + 0.5f;
	}
}

void UGlowUtils::GenerateSplitGlowMesh(const TArray<FVector>& Points, TArray<int>& FillIndexBuffer,
	TArray<FVector>& FillVertexBuffer, TArray<FLinearColor>& FillColorBuffer, TArray<FVector2D>& FillUV1,
	TArray<FVector2D>& FillUV2, TArray<int>& GlowIndexBuffer, TArray<FVector>& GlowVertexBuffer,
	TArray<FLinearColor>& GlowColorBuffer, TArray<FVector2D>& GlowUV1, TArray<FVector2D>& GlowUV2, float Eps)
{
	const int N = Points.Num();
	GlowVertexBuffer.SetNumUninitialized(4*N);
	GlowColorBuffer.SetNumUninitialized(4*N);
	GlowUV1.SetNumUninitialized(4*N);
	GlowUV2.SetNumUninitialized(4*N);
	GlowIndexBuffer.SetNumUninitialized(3 * (0 + N * 4));

	FVector CenterPoint = FVector::ZeroVector;

	// 1. Fill in the inner shape
	for (int i = 0; i < N; ++i)
	{
		GlowVertexBuffer[i] = Points[i];
		GlowColorBuffer[i] = {0, 0, 0, 0};
		CenterPoint += Points[i];
	}
	CenterPoint /= N;
	
	// 2. Add the new points
	for (int i = 0; i < N; ++i)
	{
		const FVector NextDir = (Points[(i+1+N)%N] - Points[i]).GetSafeNormal();
		const FVector PrevDir = (Points[(i-1+N)%N] - Points[i]).GetSafeNormal();
		const FVector CenterDir = (CenterPoint - Points[i]).GetSafeNormal();
		
		// We need to offset them slightly so t hey don't get auto-merged...
		GlowVertexBuffer[N+3*i+0] = Points[i] - Eps * NextDir;
		GlowVertexBuffer[N+3*i+1] = Points[i] - Eps * PrevDir;
		GlowVertexBuffer[N+3*i+2] = Points[i] - Eps * CenterDir;

		// Masks for us to know which vector offset to apply
		GlowColorBuffer[N+3*i+0] = {0, 1, 0, 0};
		GlowColorBuffer[N+3*i+1] = {1, 0, 0, 0};
		GlowColorBuffer[N+3*i+2] = {0, 0, 1, 0};

		GlowUV1[i] = FVector2D(NextDir);
		GlowUV1[N+3*i+0] = FVector2D(NextDir);
		GlowUV1[N+3*i+1] = FVector2D(NextDir);
		GlowUV1[N+3*i+2] = FVector2D(NextDir);
		
		GlowUV2[i] = FVector2D(PrevDir);
		GlowUV2[N+3*i+0] = FVector2D(PrevDir);
		GlowUV2[N+3*i+1] = FVector2D(PrevDir);
		GlowUV2[N+3*i+2] = FVector2D(PrevDir);
	}

	// 3. Make the index buffer
	// 3.1 Inner shape (is empty)
	int j = 0;
	// 3.2 Flaps
	for (int i = 0; i < N; ++i)
	{
		GlowIndexBuffer[j++] = N + 3*i         + 1;
		GlowIndexBuffer[j++] = i;
		GlowIndexBuffer[j++] = N + 3*((i+1)%N) + 0;
		
		GlowIndexBuffer[j++] = N + 3*((i+1)%N) + 0;
		GlowIndexBuffer[j++] = i;
		GlowIndexBuffer[j++] = (i+1)%N;
	}
	// 3.3 Connections
	for (int i = 0; i < N; ++i)
	{
		GlowIndexBuffer[j++] = N + 3*i + 0;
		GlowIndexBuffer[j++] = i;
		GlowIndexBuffer[j++] = N + 3*i + 2;
		
		GlowIndexBuffer[j++] = N + 3*i + 2;
		GlowIndexBuffer[j++] = i;
		GlowIndexBuffer[j++] = N + 3*i + 1;
	}

	// 4. Pack the UVs
	for (int i = 0; i < 4*N; ++i)
	{
		GlowUV1[i] = 0.5f * GlowUV1[i] + 0.5f;
		GlowUV2[i] = 0.5f * GlowUV2[i] + 0.5f;
	}

	// Fill in the inner shape
	FillVertexBuffer = Points;
	FillColorBuffer.SetNumZeroed(N);
	FillUV1.SetNumZeroed(N);
	FillUV2.SetNumZeroed(N);
	FillIndexBuffer.SetNumUninitialized(3 * N);
	
	j = 0;
	for (int i = 1; i < N - 1; ++i)
	{
		FillIndexBuffer[j++] = i;
		FillIndexBuffer[j++] = 0;
		FillIndexBuffer[j++] = i + 1;
	}
}
