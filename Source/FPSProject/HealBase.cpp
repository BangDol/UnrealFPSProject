// Fill out your copyright notice in the Description page of Project Settings.


#include "HealBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MainPlayer.h"

// Sets default values
AHealBase::AHealBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	healthRegen = 0.3f;
	armorRegen = 0.0f;

	healMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HealMesh"));
	healMesh->SetupAttachment(RootComponent);

	boxColl = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	boxColl->SetupAttachment(healMesh);
	boxColl->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void AHealBase::BeginPlay()
{
	Super::BeginPlay();
	
	FScriptDelegate overlap_delegate;
	overlap_delegate.BindUFunction(this, FName("OnOverlapEvent"));

	boxColl->OnComponentBeginOverlap.Add(overlap_delegate);
}

// Called every frame
void AHealBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHealBase::OnOverlapEvent(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSwepp,
	const FHitResult& SweepResult)
{
	HealPlayer(OtherActor);
}

void AHealBase::HealPlayer(AActor* OtherActor)
{
	if (OtherActor != nullptr)
	{
		AMainPlayer* player = Cast<AMainPlayer>(OtherActor);

		if (player != nullptr)
		{
			if (pickUpSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, pickUpSound, GetActorLocation());
			}

			player->SetHealth(
				player->GetHealth() +
				healthRegen);

			player->SetArmor(
				player->GetArmor() +
				armorRegen);

			//블루프린트에서 UI 표시
			player->HealPickedUp();

			Destroy();
		}
	}
}