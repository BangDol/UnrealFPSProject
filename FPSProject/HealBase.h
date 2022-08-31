// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealBase.generated.h"

UCLASS()
class FPSPROJECT_API AHealBase : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* healMesh;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* boxColl;

private:
	UPROPERTY(EditAnywhere, Category = "Character")
	float healthRegen;

	UPROPERTY(EditAnywhere, Category = "Character")
	float armorRegen;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* pickUpSound;

public:
	UFUNCTION()
	void OnOverlapEvent(
			UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSwepp,
			const FHitResult& SweepResult);

	UFUNCTION()
	void HealPlayer(AActor* OtherActor);

public:	
	// Sets default values for this actor's properties
	AHealBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
