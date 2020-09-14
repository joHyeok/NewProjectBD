// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BasicAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBD_API UBasicAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Status")
		float Speed;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Status")
		float Direction;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Status")
		float CurrentLeanAngle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint64 bIsFire : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint64 bIsSprint : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint64 bIsIronsight : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint64 bIsCrouch : 1;
};
