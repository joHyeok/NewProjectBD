// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BasicPlayer.generated.h"

UCLASS()
class PROJECTBD_API ABasicPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABasicPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		class USpringArmComponent* SpringArm;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		class UCameraComponent* Camera;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		class UWeaponComponent* Weapon;

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void LookUp(float AxisValue);
	void Turn(float AxisValue);
	void CameraView(float AxisValue);
	
	void Sprint();
	void StopSprint();

	void StartFire();
	void StopFire();

	void OnFire();

	void StartIronsight();
	void StopIronsight();

	void StartCrouch();

	void Reload();

	void StartLeftLean();
	void StartRightLean();
	void StopLeftLean();
	void StopRightLean();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float WalkSpeed = 300.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		float SprintSpeed = 600.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		float CurrentHP = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		float MaxHP = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint64 bIsFire : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint64 bIsSprint : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint64 bIsIronsight : 1;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bLeftLean : 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Status")
		uint8 bRightLean : 1;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SpringArm");
		FVector NormalSpringArmPosition;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SpringArm");
		FVector CrouchedSpringArmPosition;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray <TEnumAsByte<EObjectTypeQuery>> TestObjects;

	FTimerHandle FireTimerHandle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	class UParticleSystem* HitEffect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UParticleSystem* BloodHitEffect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
	class USoundBase* WeaponSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* DeadMontage;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* ReloadMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UAnimMontage* HitReactMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Data")
		class UMaterialInstance* NormalDecal;

	virtual float TakeDamage(float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;

};
