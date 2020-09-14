// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicHUD.h"
#include "Engine/Canvas.h"
#include "Engine/StreamableManager.h"
#include "Engine/Texture2D.h"



void ABasicHUD::DrawHUD()
{
	Super::DrawHUD();

	//캔버스가져오기
	//Canvas->Get

	//선그리기
	//Draw2DLine(100, 100, 200, 200, FColor::Red);

	//두께도 조절
	//DrawLine(300, 300, 400, 400, FLinearColor::Green, 3.0f);

	//DrawTextureSimple()

	if (Crosshair) {
		DrawTextureSimple(Crosshair, 
			Canvas->SizeX / 2 - (Crosshair->GetSizeX() / 2),
			Canvas->SizeY / 2 - (Crosshair->GetSizeY() / 2));
	}
}

void ABasicHUD::BeginPlay()
{
	Super::BeginPlay();
	FStreamableManager loader;
	
	//경로 설정하기
	T_Crosshair.SetPath(TEXT("Texture2D'/Game/UI/Crosshair/crosshair.crosshair'"));

	//적용하기
	//동기 로딩, 로딩시 화면이 멈춤, 10개 매시 로딩 끝날때까지 멈춤
	//Crosshair = loader.LoadSynchronous<UTexture2D>(T_Crosshair);

	//비동기 로딩
	loader.RequestAsyncLoad(T_Crosshair, FStreamableDelegate::CreateUObject(this, &ABasicHUD::LoadHUDTexture));
	
}

void ABasicHUD::LoadHUDTexture() {
	//로드시도
	Crosshair = Cast<UTexture2D>(T_Crosshair.TryLoad());
}