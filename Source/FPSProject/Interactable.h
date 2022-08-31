// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

UCLASS()
class FPSPROJECT_API AInteractable : public AActor
{
	GENERATED_BODY()
	
//컴포넌트
	UPROPERTY(EditAnywhere)
	class USceneComponent* root;

//멤버 함수
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnInteract();

public:	
	// Sets default values for this actor's properties
	AInteractable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
