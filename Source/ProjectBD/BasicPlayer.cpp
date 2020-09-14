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
	//하드코딩
	GetMesh()->SetRelativeRotation(FRotator(0, -90.f, 0));

	//캐릭터 무브먼트에 Navmovement의 CanCrouch 체크하기 하드코딩
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
	//절대 회전, 카메라 같게 설정
	//GetControlRotation().Vector()
	FVector CameraLocation;
	FRotator CameraRotator;
	//컨트롤러가 카메라의 정보를 가지고 있다. 카메라의 위치, 회전값을 저장한다.
	GetController()->GetPlayerViewPoint(CameraLocation, CameraRotator);

	// 캐릭터의 이동을 카메라의 방향으로 이동시킬건지 컨트롤러의 방향으로 이동시킬건지
	// 정해야한다. 스프링암이 폰의 회전값을 가져오는지 안 가져오는지도 설정함으로서
	// 다른 결과가 나온다.

	//카메라의 방향으로 이동한다. 스프링암의 yaw방향으로만 이동한다.
	//FRotator YawBaseRotation = FRotator(0, CameraRotator.Yaw, 0);

	// 컨트롤러의 방향으로 이동한다.
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
	//발사가 아니면 return
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

	//GetViewportSize : 플레이어가 가진 화면 사이즈
	PC->GetViewportSize(ScreenSizeX, ScreenSizeY);

	//카메라를 관리하는 플레이어 컨트롤러가 가지고 있다.
	//ProjectWorldLocationToScreen : 월드좌표를 화면좌표계로 바꾼다.
	//DeprojectScreenPositionToWorld : 화면 좌표계를 월드 좌표계로 바꾼다. 엔진의 기능임
	//아래 코드는 화면의 가운데 지점을 월드좌표로 가져온거다. 화면의 가운데 즉 에임의 위치다.
	PC->DeprojectScreenPositionToWorld(ScreenSizeX / 2, ScreenSizeY / 2, CrosshairWorldPosition, CrosshairWorldDirection);

	//카메라 위치 가져오기
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	//라인 트레이스 사용
	FVector TraceStart = CameraLocation;
	//끝점은 시작점에서 조준점방향으로 쭉 늘리는것
	FVector TraceEnd = TraceStart + (CrosshairWorldDirection * 99999.f);

	//프젝세팅에 트레이스 체널들의 배열값이다. 그 이넘값을 바이트 형태로 만든것
	//충돌체크할 리스트를 만드는거다.
	TArray <TEnumAsByte<EObjectTypeQuery>> Objects;

	//손으로 하드코딩은 거의 안한다. UPROPERTY()로 빼서 캐릭터 블프에서 사용한다.
	Objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	Objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	//Objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	//ECC_Pawn으로 하면 캐릭터의 캡슐 컴포넌트까지 적용해서 디테일하지 않다
	//mesh의 콜리전 프리셋을 커스텀으로 바꾸고 ObjectType을 PhysicsBody로 바꿔서
	// Mesh 콜리전에만 적용시키자
	Objects.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));

	//값을 안넣어서 다 맞는걸로 함
	TArray<AActor*> ActorToIgnore;

	//맞은놈 정보 저장
	//온갖 정보를 가지고 있다.
	FHitResult OutHit;

	//충돌하면 true를 반환함
	//월드정보, 시작, 끝, 충돌할 리스트, 스테틱메시의 충돌확인을 false면 단순콜리전으로 할것이냐 true면 복합콜리전으로 할것이냐를 설정한다, 
	//안맞을 애들, 그려진 타입, 맞은 놈 정보, 무시 사항에 폰을 뺄거냐?, 라인색, 맞은 후에 색, 나타날 시간
	//마지막 3개는 보통 안쓴다 보일려면 쓴다
	//카메라에서 쏘는 이유는 UI의 조준선이 화면 기준으로 되어 있기 때문에 에임을 맞춘다는게 카메라에서 조준섬으로 라인을 쏘는걸로 한다.
	//맞는다면 총에서 라인을 쏴서 진짜 맞았는지 확인
	bool Result = UKismetSystemLibrary::LineTraceSingleForObjects(
		GetWorld(), TraceStart, TraceEnd, Objects, true, ActorToIgnore, EDrawDebugTrace::None,
		OutHit, true, FLinearColor::Red, FLinearColor::Green, 5.0f);

	//충돌한다면
	if (Result) {
		//HitEffect(Blood) and Decal
		//피격했을때 이펙트효과
		if (Cast<ACharacter>(OutHit.GetActor())) {
			//캐릭터
			//+ (OutHit.ImpactNormal * 10) : 피격 위치보다 조금 내쪽으로 당긴거다 이펙트가 더 잘보일려고
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodHitEffect, OutHit.ImpactPoint + (OutHit.ImpactNormal * 10));

			//not use skeletalMesh
			//UDecalComponent* NewDecal = UGameplayStatics::SpawnDecalAtLocation(
			//	GetWorld(), NormalDecal, FVector(5, 5, 5), OutHit.ImpactPoint, OutHit.ImpactNormal.Rotation(), 10.0f);
			////화면 사이즈 비율로 정해진다.
			////0.005퍼센트보다 작아지면 안보임
			//NewDecal->SetFadeScreenSize(0.005f);
		}
		else {
			//비캐릭터
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, OutHit.ImpactPoint + (OutHit.ImpactNormal * 10));

			UDecalComponent* NewDecal = UGameplayStatics::SpawnDecalAtLocation(
				GetWorld(), NormalDecal, FVector(5, 5, 5), OutHit.ImpactPoint, OutHit.ImpactNormal.Rotation(), 10.0f);

			//화면 사이즈 비율로 정해진다.
			//0.005퍼센트보다 작아지면 안보임
			NewDecal->SetFadeScreenSize(0.005f);
		}

		//그냥 데미지
		////맞은놈, 데미지, 때린 플레이어 컨트롤러, 때린놈(사람이냐 총알이냐), 데미지타입
		//UGameplayStatics::ApplyDamage(OutHit.GetActor(), 30.f, GetController(), this, UBulletDamageType::StaticClass());
		////OutHit.
		//UE_LOG(LogClass, Warning, TEXT("Hit %s"), *OutHit.GetActor()->GetName());

		////RadialDamage
		//TArray<AActor*> IgnoreActors;
		////데미지량, 데미지 발생위치, 데미지범위, 데미지타입, 무시할 오브젝트, 때린놈, 때린 플레이어, 풀데미지적용여부, 데미지 막아주는 채널 정보(충돌채)
		//UGameplayStatics::ApplyRadialDamage(GetWorld(), 30.0f, OutHit.ImpactPoint, 300.0f, UBulletDamageType::StaticClass(),
		//	IgnoreActors, this, GetController(), true, ECC_Visibility);

		//PointDamage
		//맞은놈, 데미지량, 데미지 방향, 데미지 충돌 정보, 때린 플레이어, 때린놈, 데미지 타입
		UGameplayStatics::ApplyPointDamage(OutHit.GetActor(), 30.0f, -OutHit.ImpactNormal, OutHit, GetController(), this,
			UBulletDamageType::StaticClass());
	}

	//WeaponSond and MuzzleFlash
	//null이 아닐때만 실행
	if (WeaponSound) {
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponSound, Weapon->GetComponentLocation());
	}

	if (MuzzleFlash) {
		//Muzzle이라는 소켓이 총에 있어야 함
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, Weapon->GetSocketTransform(TEXT("Muzzle")));
	}

	//누르고 있으면 연사
	//OnFire 자신을 호출하므로 연사가능
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
	//이미 엔진에서 만들어 놓은 함수이다.
	// 앉을 수 있는지 검사해서 앉고 일어서는 걸 수행한다.
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

//데미지 처리되는 모든 액터에 구현을 해야 됨
float ABasicPlayer::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//죽으면 처리하지 않는다
	if (CurrentHP <= 0) {
		return 0.0f;
	}

	//데미지 타입별로 다른 처리
	//포인트데미지
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID)) {
		//맞은놈의 정보를 가져오기 위해서 따로 지정
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
		

		//헤드샷
		if (PointDamageEvent->HitInfo.BoneName.Compare(TEXT("head")) == 0) {
			CurrentHP = 0;
		}
		else {
			//CurrentHP -= DamageAmount;
		}

		CurrentHP = FMath::Clamp(CurrentHP, 0.0f, 100.0f);

		if (CurrentHP <= 0) {
			//죽는거
			//애니메이션
			if (DeadMontage) {
				FString SectionName = FString::Printf(TEXT("Death_%d"), FMath::RandRange(1, 3));

				PlayAnimMontage(DeadMontage, 1.0f, FName(SectionName));
			}

			////물리처리
			//GetMesh()->SetSimulatePhysics(true);
			//GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//
			////GetMesh()->AddImpulse(PointDamageEvent->ShotDirection * 40000.f, TEXT("head"));
			//GetMesh()->AddImpulse(-PointDamageEvent->HitInfo.ImpactNormal * 40000.f, TEXT("head"));
		}
	}

	//레디얼 데미지
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID)) {
		FRadialDamageEvent* RadialDamageEvent = (FRadialDamageEvent*)(&DamageEvent);
		//RadialDamageEvent.
		UE_LOG(LogClass, Warning, TEXT("Radial Damage %f"), DamageAmount);
	}

	//그냥 데미지
	else {
		CurrentHP -= DamageAmount;
	}

	UE_LOG(LogClass, Warning, TEXT("%f"), DamageAmount);
	return 0.0f;
}

