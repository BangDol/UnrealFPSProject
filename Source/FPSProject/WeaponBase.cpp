// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimInstance.h"
#include "DrawDebugHelpers.h"
#include "MainPlayer.h"
#include "EnemyBase.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	damage = 15.0f;
	magazineSize = 30;
	currentAmmo = magazineSize;
	maxAmmo = 200;
	fireRPM = 950.0f;
	reloadTime = 2.0f;
	ADS_FOV = 75.0f;
	fireMode = EFireMode::FM_Single;

	weaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	weaponMesh->SetupAttachment(RootComponent);

	attachmentMesh = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("AttachementMesh"));
	attachmentMesh->SetCollisionProfileName(TEXT("NoCollision"));
	attachmentMesh->AttachToComponent(weaponMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RailSocket"));
}

#pragma region BeginPlay / Tick

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	weaponOwner = Cast<AMainPlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	fireRate = 60.0f / fireRPM;	// 자동 사격 시 발사 사이의 간격 = 60초 / 분당 발사 수
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#pragma endregion

#pragma region 사격 / 재장전

void AWeaponBase::Fire()
{
	if (weaponOwner != nullptr)
	{
		if (weaponOwner->GetIsShooting())
		{
			switch (fireMode)
			{
				//안전
			case EFireMode::FM_Safe:
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Please change the fire mode"));
				break;

				//단발
			case EFireMode::FM_Single:
				Fire_Single();
				break;

				//자동
			case EFireMode::FM_Automatic:
				Fire_Auto();
				break;

			default:
				break;
			}
			//탄이 없는 경우
			if (currentAmmo <= 0 && fireMode != EFireMode::FM_Safe)
			{
				if (magEmpty != nullptr)
				{
					UGameplayStatics::PlaySoundAtLocation(this, magEmpty, GetActorLocation());
				}
			}
		}
		//사격 타이머 해제하면 마우스 땠을 때 사격 중지됨
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(waitHandle_Fire);
		}
	}
}

//단발
void AWeaponBase::Fire_Single()
{
	//라인 트레이스
	FVector startTrace = weaponMesh->GetSocketLocation(TEXT("ReticleSocket"));
	FVector endTrace = startTrace +
		(weaponOwner->GetFirstPersonCameraComponent()->GetForwardVector() * 100000.0f) +
		UKismetMathLibrary::RandomUnitVector() * 3000.0f;

	GetWorld()->LineTraceSingleByChannel(
		bullet_hitResult,
		startTrace,
		endTrace,
		ECollisionChannel::ECC_Visibility
	);

	if (currentAmmo > 0)
	{
		currentAmmo -= 1;

		//소음 만들기
		AddNoise();
		//애니메이션
		PlayOwnerFireAnimation();
		PlayWeaponFireAnimation();

		if (bullet_hitResult.GetActor() != nullptr)
		{
			AEnemyBase* opponent = Cast<AEnemyBase>(bullet_hitResult.GetActor());

			//적을 맞췄을 때
			if (opponent != nullptr)
			{
				opponent->TakeDamage(damage, bullet_hitResult);
			}
			//이외에 다른 곳을 맞췄을 때
			else
			{
				//파티클
				AddBulletImpact();
			}
		}
	}
}

//자동
void AWeaponBase::Fire_Auto()
{
	Fire_Single();
	//단발 사격을 연사력만큼의 간격으로 계속 호출
	GetWorld()->GetTimerManager().SetTimer(waitHandle_Fire, this, &AWeaponBase::Fire_Single, fireRate, true);
}

//장전
void AWeaponBase::Reload()
{
	if (currentAmmo < magazineSize)
	{
		weaponOwner->SetIsReloading(true);
		PlayOwnerReloadAnimation();
		PlayWeaponReloadAnimation();

		GetWorld()->GetTimerManager().SetTimer(waitHandle_Reload, FTimerDelegate::CreateLambda([&]()
			{
				if (magazineSize <= maxAmmo)
				{
					maxAmmo -= (magazineSize - currentAmmo);
					currentAmmo = magazineSize;
				}
				else if (magazineSize > maxAmmo)
				{
					currentAmmo += maxAmmo;
					maxAmmo = 0;
				}
				weaponOwner->SetIsReloading(false);
			}), reloadTime, false);
	}
}

//조정간
void AWeaponBase::ChangeFireMode()
{
	AddFireModeSound();

	switch (fireMode)
	{
	case EFireMode::FM_Safe:
		fireMode = EFireMode::FM_Single;
		break;

	case EFireMode::FM_Single:
		fireMode = EFireMode::FM_Automatic;
		break;

	case EFireMode::FM_Automatic:
		fireMode = EFireMode::FM_Safe;
		break;

	default:
		break;
	}
}

#pragma endregion

#pragma region 파티클

void AWeaponBase::AddBulletImpact()
{
	if (bullet_impact != nullptr && bullet_hitResult.GetActor() != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), 
			bullet_impact, 
			bullet_hitResult.Location, 
			bullet_hitResult.GetActor()->GetActorRotation());
	}
}

#pragma endregion

#pragma region 사운드

//조정간
void AWeaponBase::AddFireModeSound()
{
	if (chaningFireModeSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, chaningFireModeSound, GetActorLocation());
	}
}

#pragma endregion

#pragma region 애니메이션

//플레이어 애니메이션 몽타주
void AWeaponBase::PlayOwnerFireAnimation()
{
	weaponOwner->PlayFireAnimation();
}

void AWeaponBase::PlayOwnerReloadAnimation()
{
	weaponOwner->PlayReloadAnimation();
}

//무기 애니메이션 몽타주
void AWeaponBase::PlayWeaponFireAnimation()
{
	if (fireAnim != nullptr)
	{
		weaponMesh->PlayAnimation(fireAnim, false);
	}
}

void AWeaponBase::PlayWeaponReloadAnimation()
{
	if (reloadAnim != nullptr)
	{
		weaponMesh->PlayAnimation(reloadAnim, false);
	}
}

#pragma endregion