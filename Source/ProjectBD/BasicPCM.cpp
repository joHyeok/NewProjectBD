// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicPCM.h"
#include "BasicPlayer.h"
#include "GameFramework/SpringArmComponent.h"

ABasicPCM::ABasicPCM() {

}

void ABasicPCM::UpdateCamera(float DeltaTime)
{
	Super::UpdateCamera(DeltaTime);
	//화각이라고 거리에 따라 카메라 폭의 조정이 있다. FOV라고 한다.

	//날 가지고 있는 플레이어 컨트롤러가 누구냐 그 플레이어 컨트롤러를 가지고 있는 폰이 누구냐
	// 그 폰을 BasicPlayer로 변환
	ABasicPlayer* Pawn = GetOwningPlayerController()->GetPawn<ABasicPlayer>();

	if (Pawn) {
		//줌인 줌아웃 설정
		float TargetFOV = Pawn->bIsIronsight ? IronsightFOV : NormalFOV;

		//현재 FOV : GetFOVAngle()
		//Lerp : 두 값 사이에 보간을 수행
		//float ResultFOV = FMath::Lerp(GetFOVAngle(), TargetFOV, 0.5f);

		// 두 값 사이를 원하는 시간동안 부드럽게 보간
		float ResultFOV = FMath::FInterpTo(GetFOVAngle(), TargetFOV, DeltaTime, 20.0f);
		SetFOV(ResultFOV);

		FVector TargetPosition = Pawn->bIsCrouched ? Pawn->CrouchedSpringArmPosition : Pawn->NormalSpringArmPosition;

		FVector ResultSpringArmPosition = FMath::VInterpTo(Pawn->SpringArm->GetRelativeLocation(), 
			TargetPosition, DeltaTime, 15.0f);

		Pawn->SpringArm->SetRelativeLocation(ResultSpringArmPosition);
	}
}
