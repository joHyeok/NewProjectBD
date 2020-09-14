// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BasicHUD.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBD_API ABasicHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD()override;

	virtual void BeginPlay() override;

	class UTexture2D* Crosshair;
	FSoftObjectPath T_Crosshair;

	//델리게이트 쓰니까 UFUNCTION()붙이자.
	UFUNCTION()
		void LoadHUDTexture();
};
