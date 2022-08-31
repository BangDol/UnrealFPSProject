// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

UCLASS()
class FPSPROJECT_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

//컴포넌트
private:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* attackRange;

//end of 컴포넌트

//멤버 변수
private:
	UPROPERTY()
	float health;

	UPROPERTY(EditAnywhere, BlueprintGetter = GetDamage, Category = "Damage")
	float damage;

	UPROPERTY(BlueprintGetter = GetIsAttacking, BlueprintSetter = SetIsAttacking)
	bool isAttacking;

	UPROPERTY(BlueprintGetter = GetIsDetectingPlayer, BlueprintSetter = SetIsDetectingPlayer)
	bool isDetectingPlayer;

//end of 멤버변수

//파티클
public:
	UPROPERTY(EditAnywhere)
	class UParticleSystem* hit_impact;

//end of 파티클

//Getter&Setter
public:
	UFUNCTION(BlueprintGetter)
	float GetDamage() { return damage; }

	UFUNCTION(BlueprintGetter)
	bool GetIsAttacking() { return isAttacking; }
	UFUNCTION(BlueprintSetter)
	void SetIsAttacking(bool value) { isAttacking = value; }

	UFUNCTION(BlueprintGetter)
	bool GetIsDetectingPlayer() { return isDetectingPlayer; }
	UFUNCTION(BlueprintSetter)
	void SetIsDetectingPlayer(bool value) { isDetectingPlayer = value; }

//end of Getter&Setter

//멤버함수
public:
	UFUNCTION()
	void TakeDamage(float value, FHitResult hitResult);
	UFUNCTION(BlueprintImplementableEvent)
	void OnTakeDamage();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void Attack(float damageValue);

	UFUNCTION(BlueprintImplementableEvent)
	void MakeRagdoll();

	void AddBloodEffect(FHitResult hitResult);

public:
	// Sets default values for this character's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

//end of 멤버함수
};
