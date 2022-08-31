// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MainPlayer.h"
#include "WeaponBase.h"

// Sets default values
AAmmoBase::AAmmoBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	magazineSize = 30;
	
	ammoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	ammoMesh->SetupAttachment(RootComponent);

	boxColl = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	boxColl->SetupAttachment(ammoMesh);
	boxColl->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void AAmmoBase::BeginPlay()
{
	Super::BeginPlay();

	FScriptDelegate overlap_delegate;
	overlap_delegate.BindUFunction(this, FName("OnOverlapEvent"));
	
	boxColl->OnComponentBeginOverlap.Add(overlap_delegate);
}

// Called every frame
void AAmmoBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAmmoBase::OnOverlapEvent(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSwepp,
	const FHitResult& SweepResult)
{
	AddMagazineToWeapon(OtherActor);
}

void AAmmoBase::AddMagazineToWeapon(AActor* OtherActor)
{
	if (OtherActor != nullptr)
	{
		AMainPlayer* player = Cast<AMainPlayer>(OtherActor);

		if (player != nullptr)
		{
			if (player->GetEquippedWeapon() != nullptr)
			{
				if (pickUpSound != nullptr)
				{
					UGameplayStatics::PlaySoundAtLocation(this, pickUpSound, GetActorLocation());
				}

				player->GetEquippedWeapon()->SetMaxAmmo(
					player->GetEquippedWeapon()->GetMaxAmmo() +
					magazineSize);

				//블루프린트에서 UI 표시
				player->AmmoPickedUp();

				Destroy();
			}
		}
	}
}