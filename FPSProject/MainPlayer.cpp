// Copyright Epic Games, Inc. All Rights Reserved.

#include "MainPlayer.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "WeaponBase.h"
#include "Interactable.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

AMainPlayer::AMainPlayer()
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	//체력&아머
	health = 0.8f;
	armor = 0.3f;

	//상태
	isAiming = false;
	isShooting = false;
	isSprinting = false;
	isWalking = true;
	isSlowWalking = false;
	isFlashOn = false;
	isReloading = false;

	//이동속도
	sprintSpeed = 600.0f;
	walkSpeed = 300.0f;
	slowWalkSpeed = 100.0f;
	crouchSpeed = 300.0f;
	//발소리 간격
	footStepInterval = 0.8f;

	//상호작용 반경
	interactRange = 200.0f;

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	Arm = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterArm"));
	Arm->SetOnlyOwnerSee(true);
	Arm->SetupAttachment(FirstPersonCameraComponent);
	Arm->bCastDynamicShadow = false;
	Arm->CastShadow = false;
	Arm->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Arm->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
}

#pragma region BeginPlay / Tick

void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();

	//SpawnWithWeapon은 시퀀스가 끝난 후 호출

	//플레이어 이동 속도 설정
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = crouchSpeed;
}

void AMainPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
#pragma endregion

#pragma region 키보드/마우스 입력 바인딩

void AMainPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainPlayer::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMainPlayer::TurnAtRate);

	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainPlayer::LookUpAtRate);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMainPlayer::Fire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMainPlayer::StopFire);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMainPlayer::Reload);
	PlayerInputComponent->BindAction("ChangeFireMode", IE_Pressed, this, &AMainPlayer::ChangeFireMode);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMainPlayer::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMainPlayer::Walk);
	PlayerInputComponent->BindAction("SlowWalk", IE_Pressed, this, &AMainPlayer::SlowWalk);
	PlayerInputComponent->BindAction("SlowWalk", IE_Released, this, &AMainPlayer::Walk);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMainPlayer::DoCrouch);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AMainPlayer::Interact);
	PlayerInputComponent->BindAction("ToggleFlash", IE_Pressed, this, &AMainPlayer::ToggleFlashlight);
}
#pragma endregion

#pragma region 이동과 회전

void AMainPlayer::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		if (isSprinting)
		{
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(headBop_sprint);
		}
		else if (isWalking)
		{
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(headBop_walk);
		}
		else if (isSlowWalking)
		{
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(headBop_slowWalk);
		}
		AddMovementInput(GetActorForwardVector(), Value);
		PlayFootStepSoundCue();
	}
}

void AMainPlayer::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		if (isSprinting)
		{
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(headBop_sprint);
		}
		else if (isWalking)
		{
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(headBop_walk);
		}
		else if (isSlowWalking)
		{
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(headBop_slowWalk);
		}
		AddMovementInput(GetActorRightVector(), Value);
		PlayFootStepSoundCue();
	}
}

void AMainPlayer::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainPlayer::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

//달리기
void AMainPlayer::Sprint()
{
	//야매..?
	isSprinting = true;
	isWalking = false;
	isSlowWalking = false;
	GetCharacterMovement()->MaxWalkSpeed = sprintSpeed;
	footStepInterval = 0.4f;
}

//걷기
void AMainPlayer::Walk()
{
	isSprinting = false;
	isWalking = true;
	isSlowWalking = false;;
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	footStepInterval = 0.8f;
}

//느리게 걷기
void AMainPlayer::SlowWalk()
{
	isSprinting = false;
	isWalking = false;
	isSlowWalking = true;
	GetCharacterMovement()->MaxWalkSpeed = slowWalkSpeed;
	footStepInterval = 1.1f;
}

//앉기
void AMainPlayer::DoCrouch()
{
	if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
	else
	{
		Crouch();
		GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	}
}

#pragma endregion

#pragma region 무기 시스템

//무기를 메시 소켓에 스폰
void AMainPlayer::SpawnWithWeapon()
{
	if (weaponToSpawnWith != nullptr)
	{
		FTransform weaponPlacementTransform = Arm->GetSocketTransform(TEXT("R_GunSocket"));
		FActorSpawnParameters actorSpawnParams;
		equippedWeapon = GetWorld()->SpawnActor<AWeaponBase>(
			weaponToSpawnWith,
			weaponPlacementTransform,
			actorSpawnParams
			);
		equippedWeapon->AttachToComponent(Arm, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("R_GunSocket"));

		//ADS
		defaultArmTransform = Arm->GetRelativeTransform();
		ADSTransform = CalculateADSTransform();

		//플래시
		flashlight->AttachToComponent(Arm, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("flashlightSocket"));
		flashlight->SetVisibility(isFlashOn);
	}
}

//발사
void AMainPlayer::Fire()
{
	if (equippedWeapon != nullptr)
	{
		if (!isReloading && !isSprinting)	//재장전 중이거나 달리는 중에는 사격 못하도록
		{
			isShooting = true;
			equippedWeapon->Fire();
		}
	}
}

void AMainPlayer::StopFire()
{
	if (equippedWeapon != nullptr)
	{
		if (!isReloading && !isSprinting)
		{
			isShooting = false;
			equippedWeapon->Fire();
		}
	}
}

//장전
void AMainPlayer::Reload()
{
	if (!isShooting)	//사격 도중에는 장전 못하도록
	{
		equippedWeapon->Reload();
	}
}

//조정간
void AMainPlayer::ChangeFireMode()
{
	if (!isShooting && !isReloading)
	{
		equippedWeapon->ChangeFireMode();
	}
}

//조준 시 위치
FTransform AMainPlayer::CalculateADSTransform()
{
	FTransform ADStr = UKismetMathLibrary::MakeRelativeTransform(
		Arm->GetComponentToWorld(),
		equippedWeapon->GetWeaponMesh()->GetSocketTransform(TEXT("ADS_Socket"))
	);
	
	return ADStr;
}

#pragma endregion

#pragma region 애니메이션

void AMainPlayer::PlayFireAnimation()
{
	if (FireAnimation != nullptr)
	{
		UAnimInstance* AnimInstance = Arm->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AMainPlayer::PlayReloadAnimation()
{
	if (ReloadAnimation != nullptr)
	{
		UAnimInstance* AnimInstance = Arm->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(ReloadAnimation, 1.f);
		}
	}
}

#pragma endregion

#pragma region 상호작용

void AMainPlayer::Interact()
{
	//라인 트레이스
	FVector interactStart = FirstPersonCameraComponent->GetComponentLocation();
	FVector interactEnd = interactStart + (FirstPersonCameraComponent->GetForwardVector() * interactRange);

	GetWorld()->LineTraceSingleByChannel(
		interact_hitResult,
		interactStart,
		interactEnd,
		ECollisionChannel::ECC_Visibility
	);

	if (interact_hitResult.GetActor() != nullptr)
	{
		//상호작용 가능 클래스 상속받는 액터에 대해
		AInteractable* actorToInteract = Cast<AInteractable>(interact_hitResult.GetActor());
		if (actorToInteract != nullptr)
		{
			actorToInteract->OnInteract();
		}
	}

}

void AMainPlayer::ToggleFlashlight()
{
	if (!isFlashOn)
	{
		isFlashOn = true;
	}
	else
	{
		isFlashOn = false;
	}
	flashlight->SetVisibility(isFlashOn);
}

void AMainPlayer::TakeDamage(float damage)
{
	OnTakeDamage();
	if (armor > 0.0f)
	{
		armor -= damage;
	}
	else
	{
		health -= damage;
	}

	if (health <= 0.0f)
	{
		//사망 처리 UI 생성
		CreateOnDeathUI();
		Destroy();
		equippedWeapon->Destroy();
	}
}

#pragma endregion