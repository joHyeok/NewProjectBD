// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WeaponComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "BulletDamageType.h"

// Sets default values
ABasicPlayer::ABasicPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Weapon = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon"));

	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);
	Weapon->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));

	GetMesh()->SetRelativeLocation(FVector(0, 0, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));
	//�ϵ��ڵ�
	GetMesh()->SetRelativeRotation(FRotator(0, -90.f, 0));

	//ĳ���� �����Ʈ�� Navmovement�� CanCrouch üũ�ϱ� �ϵ��ڵ�
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	SpringArm->SocketOffset = FVector(0, 40.0f, 88.0f);
	SpringArm->TargetArmLength = 120.f;
	SpringArm->bUsePawnControlRotation = true;

	GetCharacterMovement()->CrouchedHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

	NormalSpringArmPosition = SpringArm->GetRelativeLocation();
	CrouchedSpringArmPosition = NormalSpringArmPosition + FVector(0, 0, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() / 2);
}

// Called when the game starts or when spawned
void ABasicPlayer::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABasicPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABasicPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ABasicPlayer::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ABasicPlayer::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ABasicPlayer::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ABasicPlayer::Turn);

	PlayerInputComponent->BindAxis(TEXT("CameraView"), this, &ABasicPlayer::CameraView);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ABasicPlayer::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ABasicPlayer::StopJumping);

	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &ABasicPlayer::Sprint);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &ABasicPlayer::StopSprint);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ABasicPlayer::StartFire);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &ABasicPlayer::StopFire);

	PlayerInputComponent->BindAction(TEXT("Ironsight"), IE_Pressed, this, &ABasicPlayer::StartIronsight);
	PlayerInputComponent->BindAction(TEXT("Ironsight"), IE_Released, this, &ABasicPlayer::StopIronsight);

	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ABasicPlayer::StartCrouch);

	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &ABasicPlayer::Reload);

	PlayerInputComponent->BindAction(TEXT("LeftLean"), IE_Pressed, this, &ABasicPlayer::StartLeftLean);
	PlayerInputComponent->BindAction(TEXT("LeftLean"), IE_Released, this, &ABasicPlayer::StopLeftLean);
	PlayerInputComponent->BindAction(TEXT("RightLean"), IE_Pressed, this, &ABasicPlayer::StartRightLean);
	PlayerInputComponent->BindAction(TEXT("RightLean"), IE_Released, this, &ABasicPlayer::StopRightLean);
}

void ABasicPlayer::MoveForward(float AxisValue)
{
	//���� ȸ��, ī�޶� ���� ����
	//GetControlRotation().Vector()
	FVector CameraLocation;
	FRotator CameraRotator;
	//��Ʈ�ѷ��� ī�޶��� ������ ������ �ִ�. ī�޶��� ��ġ, ȸ������ �����Ѵ�.
	GetController()->GetPlayerViewPoint(CameraLocation, CameraRotator);

	// ĳ������ �̵��� ī�޶��� �������� �̵���ų���� ��Ʈ�ѷ��� �������� �̵���ų����
	// ���ؾ��Ѵ�. ���������� ���� ȸ������ ���������� �� ������������ ���������μ�
	// �ٸ� ����� ���´�.

	//ī�޶��� �������� �̵��Ѵ�. ���������� yaw�������θ� �̵��Ѵ�.
	//FRotator YawBaseRotation = FRotator(0, CameraRotator.Yaw, 0);

	// ��Ʈ�ѷ��� �������� �̵��Ѵ�.
	FRotator YawBaseRotation = FRotator(0, GetControlRotation().Yaw, 0);

	FVector CameraForward = UKismetMathLibrary::GetForwardVector(YawBaseRotation);

	AddMovementInput(CameraForward, AxisValue);
}

void ABasicPlayer::MoveRight(float AxisValue)
{
	FRotator YawBaseRotation = FRotator(0, GetControlRotation().Yaw, 0);
	FVector CameraRight = UKismetMathLibrary::GetRightVector(YawBaseRotation);
	AddMovementInput(CameraRight, AxisValue);
}

void ABasicPlayer::LookUp(float AxisValue)
{
	AddControllerPitchInput(AxisValue);
}

void ABasicPlayer::Turn(float AxisValue)
{
	AddControllerYawInput(AxisValue);
}

void ABasicPlayer::Sprint()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void ABasicPlayer::StopSprint() {
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ABasicPlayer::CameraView(float AxisValue)
{

}

void ABasicPlayer::StartFire()
{
	bIsFire = true;
	OnFire();
}

void ABasicPlayer::StopFire()
{
	bIsFire = false;
}

void ABasicPlayer::OnFire()
{
	//�߻簡 �ƴϸ� return
	if (!bIsFire) {
		return;
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	int32 ScreenSizeX;
	int32 ScreenSizeY;
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	FVector CameraLocation;
	FRotator CameraRotation;

	//GetViewportSize : �÷��̾ ���� ȭ�� ������
	PC->GetViewportSize(ScreenSizeX, ScreenSizeY);

	//ī�޶� �����ϴ� �÷��̾� ��Ʈ�ѷ��� ������ �ִ�.
	//ProjectWorldLocationToScreen : ������ǥ�� ȭ����ǥ��� �ٲ۴�.
	//DeprojectScreenPositionToWorld : ȭ�� ��ǥ�踦 ���� ��ǥ��� �ٲ۴�. ������ �����
	//�Ʒ� �ڵ�� ȭ���� ��� ������ ������ǥ�� �����°Ŵ�. ȭ���� ��� �� ������ ��ġ��.
	PC->DeprojectScreenPositionToWorld(ScreenSizeX / 2, ScreenSizeY / 2, CrosshairWorldPosition, CrosshairWorldDirection);

	//ī�޶� ��ġ ��������
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	//���� Ʈ���̽� ���
	FVector TraceStart = CameraLocation;
	//������ ���������� �������������� �� �ø��°�
	FVector TraceEnd = TraceStart + (CrosshairWorldDirection * 99999.f);

	//�������ÿ� Ʈ���̽� ü�ε��� �迭���̴�. �� �̳Ѱ��� ����Ʈ ���·� �����
	//�浹üũ�� ����Ʈ�� ����°Ŵ�.
	TArray <TEnumAsByte<EObjectTypeQuery>> Objects;

	//������ �ϵ��ڵ��� ���� ���Ѵ�. UPROPERTY()�� ���� ĳ���� �������� ����Ѵ�.
	Objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	Objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	//Objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	//ECC_Pawn���� �ϸ� ĳ������ ĸ�� ������Ʈ���� �����ؼ� ���������� �ʴ�
	//mesh�� �ݸ��� �������� Ŀ�������� �ٲٰ� ObjectType�� PhysicsBody�� �ٲ㼭
	// Mesh �ݸ������� �����Ű��
	Objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));

	//���� �ȳ־ �� �´°ɷ� ��
	TArray<AActor*> ActorToIgnore;

	//������ ���� ����
	//�°� ������ ������ �ִ�.
	FHitResult OutHit;

	//�浹�ϸ� true�� ��ȯ��
	//��������, ����, ��, �浹�� ����Ʈ, ����ƽ�޽��� �浹Ȯ���� false�� �ܼ��ݸ������� �Ұ��̳� true�� �����ݸ������� �Ұ��̳ĸ� �����Ѵ�, 
	//�ȸ��� �ֵ�, �׷��� Ÿ��, ���� �� ����, ���� ���׿� ���� ���ų�?, ���λ�, ���� �Ŀ� ��, ��Ÿ�� �ð�
	//������ 3���� ���� �Ⱦ��� ���Ϸ��� ����
	//ī�޶󿡼� ��� ������ UI�� ���ؼ��� ȭ�� �������� �Ǿ� �ֱ� ������ ������ ����ٴ°� ī�޶󿡼� ���ؼ����� ������ ��°ɷ� �Ѵ�.
	//�´´ٸ� �ѿ��� ������ ���� ��¥ �¾Ҵ��� Ȯ��
	bool Result = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(), TraceStart, TraceEnd, Objects, true, ActorToIgnore, EDrawDebugTrace::None,
		OutHit, true, FLinearColor::Red, FLinearColor::Green, 5.0f);

	//�浹�Ѵٸ�
	if (Result) {
		//HitEffect(Blood) and Decal
		//�ǰ������� ����Ʈȿ��
		if (Cast<ACharacter>(OutHit.GetActor())) {
			//ĳ����
			//+ (OutHit.ImpactNormal * 10) : �ǰ� ��ġ���� ���� �������� ���Ŵ� ����Ʈ�� �� �ߺ��Ϸ���
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodHitEffect, OutHit.ImpactPoint + (OutHit.ImpactNormal * 10));

			//not use skeletalMesh
			//UDecalComponent* NewDecal = UGameplayStatics::SpawnDecalAtLocation(
			//	GetWorld(), NormalDecal, FVector(5, 5, 5), OutHit.ImpactPoint, OutHit.ImpactNormal.Rotation(), 10.0f);
			////ȭ�� ������ ������ ��������.
			////0.005�ۼ�Ʈ���� �۾����� �Ⱥ���
			//NewDecal->SetFadeScreenSize(0.005f);
		}
		else {
			//��ĳ����
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, OutHit.ImpactPoint + (OutHit.ImpactNormal * 10));

			UDecalComponent* NewDecal = UGameplayStatics::SpawnDecalAtLocation(
				GetWorld(), NormalDecal, FVector(5, 5, 5), OutHit.ImpactPoint, OutHit.ImpactNormal.Rotation(), 10.0f);

			//ȭ�� ������ ������ ��������.
			//0.005�ۼ�Ʈ���� �۾����� �Ⱥ���
			NewDecal->SetFadeScreenSize(0.005f);
		}

		//�׳� ������
		////������, ������, ���� �÷��̾� ��Ʈ�ѷ�, ������(����̳� �Ѿ��̳�), ������Ÿ��
		//UGameplayStatics::ApplyDamage(OutHit.GetActor(), 30.f, GetController(), this, UBulletDamageType::StaticClass());
		////OutHit.
		//UE_LOG(LogClass, Warning, TEXT("Hit %s"), *OutHit.GetActor()->GetName());

		////RadialDamage
		//TArray<AActor*> IgnoreActors;
		////��������, ������ �߻���ġ, ����������, ������Ÿ��, ������ ������Ʈ, ������, ���� �÷��̾�, Ǯ���������뿩��, ������ �����ִ� ä�� ����(�浹ä)
		//UGameplayStatics::ApplyRadialDamage(GetWorld(), 30.0f, OutHit.ImpactPoint, 300.0f, UBulletDamageType::StaticClass(),
		//	IgnoreActors, this, GetController(), true, ECC_Visibility);

		//PointDamage
		//������, ��������, ������ ����, ������ �浹 ����, ���� �÷��̾�, ������, ������ Ÿ��
		UGameplayStatics::ApplyPointDamage(OutHit.GetActor(), 30.0f, -OutHit.ImpactNormal, OutHit, GetController(), this,
			UBulletDamageType::StaticClass());
	}

	//WeaponSond and MuzzleFlash
	//null�� �ƴҶ��� ����
	if (WeaponSound) {
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponSound, Weapon->GetComponentLocation());
	}

	if (MuzzleFlash) {
		//Muzzle�̶�� ������ �ѿ� �־�� ��
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, Weapon->GetSocketTransform(TEXT("Muzzle")));
	}

	//������ ������ ����
	//OnFire �ڽ��� ȣ���ϹǷ� ���簡��
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ABasicPlayer::OnFire, 0.12f, false);
}

void ABasicPlayer::StartIronsight()
{
	bIsIronsight = true;
}

void ABasicPlayer::StopIronsight()
{
	bIsIronsight = false;
}

void ABasicPlayer::StartCrouch()
{
	//�̹� �������� ����� ���� �Լ��̴�.
	// ���� �� �ִ��� �˻��ؼ� �ɰ� �Ͼ�� �� �����Ѵ�.
	if (CanCrouch()) {
		Crouch();
	}
	else {
		UnCrouch();
	}
}

void ABasicPlayer::Reload()
{
	UAnimInstance* AnimInstance = (GetMesh()) ? GetMesh()->GetAnimInstance() : nullptr;

	if (ReloadMontage && AnimInstance) {
		if (!AnimInstance->Montage_IsPlaying(ReloadMontage)) {
			PlayAnimMontage(ReloadMontage);
		}
	}
}

void ABasicPlayer::StartLeftLean()
{
	bLeftLean = true;
}

void ABasicPlayer::StartRightLean()
{
	bRightLean = true;
}

void ABasicPlayer::StopLeftLean()
{
	bLeftLean = false;
}

void ABasicPlayer::StopRightLean()
{
	bRightLean = false;
}

//������ ó���Ǵ� ��� ���Ϳ� ������ �ؾ� ��
float ABasicPlayer::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//������ ó������ �ʴ´�
	if (CurrentHP <= 0) {
		return 0.0f;
	}

	//������ Ÿ�Ժ��� �ٸ� ó��
	//����Ʈ������
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID)) {
		//�������� ������ �������� ���ؼ� ���� ����
		FPointDamageEvent* PointDamageEvent = (FPointDamageEvent*)(&DamageEvent);
		//PointDamageEvent.
		UE_LOG(LogClass, Warning, TEXT("Point Damage %s"), *(PointDamageEvent->HitInfo.BoneName.ToString()));


		//UAnimInstance * AnimInstance = (GetMesh()) ? GetMesh()->GetAnimInstance() : nullptr;
		//if (HitReactMontage && AnimInstance) {
		//	if (AnimInstance->Montage_IsPlaying(HitReactMontage)) {
		//		AnimInstance->Montage_JumpToSection(
		//			AnimInstance->Montage_GetNextSectionID(HitReactMontage, AnimInstance->Montage_GetCurrentSection, HitReactMontage);
		//	}
		//	PlayAnimMontage(HitReactMontage);
		//
		//}

		if (HitReactMontage) {
			FString HitSectionName = FString::Printf(TEXT("React_%d"), FMath::RandRange(1, 4));
			PlayAnimMontage(HitReactMontage, 1.0f, FName(HitSectionName));
		}
		

		//��弦
		if (PointDamageEvent->HitInfo.BoneName.Compare(TEXT("head")) == 0) {
			CurrentHP = 0;
		}
		else {
			//CurrentHP -= DamageAmount;
		}

		CurrentHP = FMath::Clamp(CurrentHP, 0.0f, 100.0f);

		if (CurrentHP <= 0) {
			//�״°�
			//�ִϸ��̼�
			if (DeadMontage) {
				FString SectionName = FString::Printf(TEXT("Death_%d"), FMath::RandRange(1, 3));

				PlayAnimMontage(DeadMontage, 1.0f, FName(SectionName));
			}

			////����ó��
			//GetMesh()->SetSimulatePhysics(true);
			//GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//
			////GetMesh()->AddImpulse(PointDamageEvent->ShotDirection * 40000.f, TEXT("head"));
			//GetMesh()->AddImpulse(-PointDamageEvent->HitInfo.ImpactNormal * 40000.f, TEXT("head"));
		}
	}

	//����� ������
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID)) {
		FRadialDamageEvent* RadialDamageEvent = (FRadialDamageEvent*)(&DamageEvent);
		//RadialDamageEvent.
		UE_LOG(LogClass, Warning, TEXT("Radial Damage %f"), DamageAmount);
	}

	//�׳� ������
	else {
		CurrentHP -= DamageAmount;
	}

	UE_LOG(LogClass, Warning, TEXT("%f"), DamageAmount);
	return 0.0f;
}

