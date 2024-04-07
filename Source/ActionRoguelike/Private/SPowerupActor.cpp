// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActor.h"
#include "components/SphereComponent.h"

// Sets default values
ASPowerupActor::ASPowerupActor()
{
 	
	SphereComp = CreateDefaultSubobject<USphereComponent>("ShpereComp");
	SphereComp->SetCollisionProfileName("Powerup");
	RootComponent = SphereComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetupAttachment(RootComponent);

	bIsActive = true;
	RespawnTime = 10.0f;

}

// Called when the game starts or when spawned
void ASPowerupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASPowerupActor::ShowPowerup()
{
	SetPowerupState(true);
}

void ASPowerupActor::HideAndCooldownPowerup()
{
	SetPowerupState(false);

	GetWorldTimerManager().SetTimer(TimerHandle_RespawnTime, this, &ASPowerupActor::ShowPowerup, RespawnTime);
}

void ASPowerupActor::SetPowerupState(bool bNewIsActive)
{
	bIsActive = bNewIsActive;

	// 碰撞状态
	SetActorEnableCollision(bIsActive);
	// 可视化状态
	RootComponent->SetVisibility(bIsActive, true);
}

// Called every frame
void ASPowerupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASPowerupActor::Interact_Implementation(APawn* InstigatorPawn)
{
	
}

FText ASPowerupActor::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	return FText::GetEmpty();
}
