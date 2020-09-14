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
	}
}
