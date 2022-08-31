// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UENUM(BlueprintType)
enum class EFireMode : uint8
{
	None,
	FM_Safe				UMETA(DisplayName = "Safe"),
	FM_Single			UMETA(DisplayName = "Single"),
	FM_Automatic		UMETA(DisplayName = "Automatic")
};

UCLASS()
class FPSPROJECT_API AWeaponBase : public AActor
{
	GENERATED_BODY()

//컴포넌트
private:
	UPROPERTY(EditAnywhere, BlueprintGetter = GetWeaponMesh)
	class USkeletalMeshComponent* weaponMesh;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* attachmentMesh;

//end of 컴포넌트

public:
	//탄창 비었을 때 사격 시
	UPROPERTY(EditAnywhere, Category = "Gun Sound - Fire")
	class USoundBase* magEmpty;
	//조정간 소리
	UPROPERTY(EditAnywhere, Category = "Gun Sound - Fire")
	class USoundBase* chaningFireModeSound; 
	//총알 부딪히는 파티클
	UPROPERTY(EditAnywhere, Category = "Particle")
	class UParticleSystem* bullet_impact;
	//무기 애니메이션 몽타주
	UPROPERTY(EditAnywhere)
	class UAnimationAsset* fireAnim;
	//무기 장전 애니메이션 몽타주
	UPROPERTY(EditAnywhere)
	class UAnimationAsset* reloadAnim;
	
//멤버 변수
private:
	UPROPERTY(BlueprintGetter = GetWeaponOwner)
	class AMainPlayer* weaponOwner;

	UPROPERTY(EditAnywhere, Category = "Weapon System")
	float damage;

	UPROPERTY(EditAnywhere, Category = "Weapon System", BlueprintGetter = GetAmmo, BlueprintSetter = SetAmmo)
	int currentAmmo;

	UPROPERTY(EditAnywhere, Category = "Weapon System", BlueprintGetter = GetMagazineSize, BlueprintSetter = SetMagazineSize)
	int magazineSize;

	UPROPERTY(EditAnywhere, Category = "Weapon System", BlueprintGetter = GetMaxAmmo, BlueprintSetter = SetMaxAmmo)
	int maxAmmo;

	UPROPERTY(EditAnywhere, Category = "Weapon System")
	float fireRPM;

	UPROPERTY(BlueprintGetter = GetFireRate)
	float fireRate;

	UPROPERTY(EditAnywhere, Category = "Weapon System", BlueprintGetter = GetReloadTime)
	float reloadTime;

	UPROPERTY(EditAnywhere, Category = "Weapon System", BlueprintGetter = GetADSFOV)
	float ADS_FOV;

	UPROPERTY(EditAnywhere, Category = "Weapon System", BlueprintGetter = GetFireMode)
	EFireMode fireMode;
	int fireModeCount;

	FTimerHandle waitHandle_Fire;
	FTimerHandle waitHandle_Sound;
	FTimerHandle waitHandle_Reload;

	FHitResult bullet_hitResult;

//end of 멤버 변수

//Getter&Setter
public:
	//Weapon Owner
	UFUNCTION(BlueprintGetter)
	AMainPlayer* GetWeaponOwner() { return weaponOwner; }

	//Ammo
	UFUNCTION(BlueprintGetter)
	int GetAmmo() { return currentAmmo; }
	UFUNCTION(BlueprintSetter)
	void SetAmmo(int value) { currentAmmo = value; }

	//Magazine Size
	UFUNCTION(BlueprintGetter)
	int GetMagazineSize() { return magazineSize; }
	UFUNCTION(BlueprintSetter)
	void SetMagazineSize(int value) { magazineSize = value; }

	//Max Ammo
	UFUNCTION(BlueprintGetter)
	int GetMaxAmmo() { return maxAmmo; }
	UFUNCTION(BlueprintSetter)
	void SetMaxAmmo(int value) { maxAmmo = value; }

	//Fire Rate
	UFUNCTION(BlueprintGetter)
	float GetFireRate() { return fireRate; }

	//Reload Speed
	UFUNCTION(BlueprintGetter)
	float GetReloadTime() { return reloadTime; }

	//Weapon Mesh
	UFUNCTION(BlueprintGetter)
	USkeletalMeshComponent* GetWeaponMesh() { return weaponMesh; }

	//ADS FOV
	UFUNCTION(BlueprintGetter)
	float GETADSFOV() { return ADS_FOV; }

	//Fire Mode
	UFUNCTION(BlueprintGetter)
	EFireMode GetFireMode() {return fireMode;}
//end of Getter&Setter

//멤버 함수
public:
	void Fire();
	void Reload();
	void ChangeFireMode();
	void Fire_Single();
	void Fire_Auto();

	//파티클
	void AddBulletImpact();

	//사운드
	void AddFireModeSound();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayReloadSound();

	UFUNCTION(BlueprintImplementableEvent)
	void AddNoise();

	//애니메이션
	void PlayOwnerFireAnimation();
	void PlayOwnerReloadAnimation();
	void PlayWeaponFireAnimation();
	void PlayWeaponReloadAnimation();

public:
	// Sets default values for this actor's properties
	AWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

//end of 멤버 함수
};
