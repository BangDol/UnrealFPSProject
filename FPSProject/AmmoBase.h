// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AmmoBase.generated.h"

UCLASS()
class FPSPROJECT_API AAmmoBase : public AActor
{
	GENERATED_BODY()
	
//컴포넌트
private:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* ammoMesh;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* boxColl;
//end of 컴포넌트

//멤버 변수
private:
	UPROPERTY(EditAnywhere, Category = "Weapon System")
	int magazineSize;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* pickUpSound;
//end of 멤버 변수

//멤버 함수
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
	void AddMagazineToWeapon(AActor* OtherActor);

public:	
	// Sets default values for this actor's properties
	AAmmoBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

//end of 멤버 함수
};
