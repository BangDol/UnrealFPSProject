// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MainPlayer.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	health = 100.0f;
	damage = 0.08f;
	isAttacking = false;

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("BlockAll"));

	attackRange = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackRange"));
	attackRange->SetupAttachment(GetMesh());
	attackRange->SetRelativeLocation(FVector(0.0f, 70.0f, 100.0f));
	attackRange->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	FScriptDelegate overlap_delegate;
	overlap_delegate.BindUFunction(this, FName("OnOverlapEvent"));

	attackRange->OnComponentBeginOverlap.Add(overlap_delegate);
}

void AEnemyBase::TakeDamage(float damageValue, FHitResult hitResult)
{
	health -= damageValue;

	AddBloodEffect(hitResult);

	if (health <= 0.0f)	//체력이 다 하면
	{
		//Destroy();
		MakeRagdoll();
		attackRange->DestroyComponent(true);
	}
	else
	{
		//데미지를 받았을 때 취할 행동
		//블루프린트에서 정의
		OnTakeDamage();	
	}
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::AddBloodEffect(FHitResult hitResult)
{
	if (hit_impact != nullptr && hitResult.GetActor() != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			hit_impact,
			hitResult.Location,
			hitResult.GetActor()->GetActorRotation());
	}
}