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

void ASPowerupActor::ShowPowerup()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_RespawnTime);
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

void ASPowerupActor::Interact_Implementation(APawn* InstigatorPawn)
{
	
}