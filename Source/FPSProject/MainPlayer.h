// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainPlayer.generated.h"


UCLASS(config = Game)
class FPSPROJECT_API AMainPlayer : public ACharacter
{
	GENERATED_BODY()

//컴포넌트
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Arm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpotLightComponent* flashlight;

//end of 컴포넌트

//애니메이션
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;
	UPROPERTY(EditAnywhere, Category = Gameplay)
	class UAnimMontage* ReloadAnimation;

	UPROPERTY(EditAnywhere, Category = Camera)
	TSubclassOf<UMatineeCameraShake> headBop_sprint;
	UPROPERTY(EditAnywhere, Category = Camera)
	TSubclassOf<UMatineeCameraShake> headBop_walk;
	UPROPERTY(EditAnywhere, Category = Camera)
	TSubclassOf<UMatineeCameraShake> headBop_slowWalk;

//end of 애니메이션

//멤버 변수
private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, Category = "Weapon System")
	TSubclassOf<AWeaponBase> weaponToSpawnWith;

	UPROPERTY(BlueprintGetter = GetEquippedWeapon)
	class AWeaponBase* equippedWeapon; 
		
	UPROPERTY(EditAnywhere, Category = "Health System", BlueprintGetter = GetHealth, BlueprintSetter = SetHealth)
	float health;
	UPROPERTY(EditAnywhere, Category = "Health System", BlueprintGetter = GetArmor, BlueprintSetter = SetArmor)
	float armor;

	UPROPERTY(BlueprintGetter = GetIsAiming, BlueprintSetter = SetIsAiming)
	bool isAiming;
	UPROPERTY(BlueprintGetter = GetIsShooting)
	bool isShooting;
	UPROPERTY(BlueprintGetter = GetIsReloading, BlueprintSetter = SetIsReloading)
	bool isReloading;
	UPROPERTY(BlueprintGetter = GetIsSprinting)
	bool isSprinting;
	UPROPERTY()
	bool isWalking;
	UPROPERTY()
	bool isSlowWalking;
	UPROPERTY()
	bool isFlashOn;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float sprintSpeed;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float walkSpeed;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float slowWalkSpeed;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float crouchSpeed;
	

	UPROPERTY(EditAnywhere, Category = "Interact")
	float interactRange;
	FHitResult interact_hitResult;
	TSubclassOf<AActor> interactingActor;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Weapon System")
	FTransform defaultArmTransform;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon System")
	FTransform ADSTransform;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UWidget> revealAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float footStepInterval;

//end of 멤버 변수

//Getter&Setter
public:
	USkeletalMeshComponent* GetArm() const { return Arm; }
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UFUNCTION(BlueprintGetter)
	float GetHealth() { return health; }
	UFUNCTION(BlueprintSetter)
	void SetHealth(float value) { health = value; }

	UFUNCTION(BlueprintGetter)
	float GetArmor() { return armor; }
	UFUNCTION(BlueprintSetter)
	void SetArmor(float value) { armor = value; }

	UFUNCTION(BlueprintGetter)
	AWeaponBase* GetEquippedWeapon() { return equippedWeapon; }

	UFUNCTION(BlueprintGetter)
	bool GetIsAiming() { return isAiming; }
	UFUNCTION(BlueprintSetter)
	void SetIsAiming(bool value) { isAiming = value; }

	UFUNCTION(BlueprintGetter)
	bool GetIsSprinting() {return isSprinting;}

	UFUNCTION(BlueprintGetter)
	bool GetIsShooting() {return isShooting;}

	UFUNCTION(BlueprintGetter)
	bool GetIsReloading() { return isReloading; }
	UFUNCTION(BlueprintSetter)
	void SetIsReloading(bool value) { isReloading = value; }

//end of Getter&Setter

//멤버 함수
public:
	UFUNCTION(BlueprintCallable)
	void SpawnWithWeapon();
	void Fire();
	void StopFire();
	void Reload();
	void ChangeFireMode();
	FTransform CalculateADSTransform();

	void Sprint();
	void Walk();
	void SlowWalk();
	void DoCrouch();

	void Interact();
	void ToggleFlashlight();
	void CheckUI();

	UFUNCTION(BlueprintImplementableEvent)
	void AmmoPickedUp();
	UFUNCTION(BlueprintImplementableEvent)
	void HealPickedUp();

	void PlayFireAnimation();
	void PlayReloadAnimation();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayFootStepSoundCue();

	UFUNCTION(BlueprintCallable)
	void TakeDamage(float damage);
	UFUNCTION(BlueprintImplementableEvent)
	void OnTakeDamage();
	UFUNCTION(BlueprintImplementableEvent)
	void CreateOnDeathUI();

public:
	AMainPlayer();

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds);

protected:
	void MoveForward(float Val);
	void MoveRight(float Val);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

//end of 멤버 함수
};