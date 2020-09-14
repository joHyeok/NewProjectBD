// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicAnimInstance.h"
#include "BasicPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
void UBasicAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	ABasicPlayer* Pawn = Cast<ABasicPlayer>(TryGetPawnOwner());
	//유효검사
	if (Pawn && Pawn->IsValidLowLevel()) {
		//방향을 계산한다.
		Direction = CalculateDirection(Pawn->GetCharacterMovement()->Velocity, Pawn->GetActorRotation());

		//크기만 알아도된다면 SizeSquared()를 사용하자.
		Speed = Pawn->GetCharacterMovement()->Velocity.Size();

		bIsCrouch = Pawn->bIsCrouched;
		bIsFire = Pawn->bIsFire;
		bIsIronsight = Pawn->bIsIronsight;
		bIsSprint = Pawn->bIsSprint;

		//q,e키 회전값 적용
		bool bLeftLean = Pawn->bLeftLean;
		bool bRightLean = Pawn->bRightLean;

		float TargetLeanAngle;

		if (bLeftLean && !bRightLean) {
			TargetLeanAngle = -30.0f;
		}
		else if (bRightLean && !bLeftLean) {
			TargetLeanAngle = 30.0f;
		}
		else if (bLeftLean && bRightLean) {
			TargetLeanAngle = 0;
		}
		else TargetLeanAngle = 0;

		CurrentLeanAngle = FMath::FInterpTo(CurrentLeanAngle, TargetLeanAngle, DeltaSeconds, 10.0f);
	}
}
