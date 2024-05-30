// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActor.h"
#include "components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

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

	SetReplicates(true);
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

	OnRep_IsActive();
}

void ASPowerupActor::Interact_Implementation(APawn* InstigatorPawn)
{
	
}

FText ASPowerupActor::GetInteractMessageText_Implementation(APawn* InstigatorPawn)
{
	return FText::GetEmpty();
}

void ASPowerupActor::OnRep_IsActive()
{
	// 碰撞状态
	SetActorEnableCollision(bIsActive);
	// 可视化状态
	RootComponent->SetVisibility(bIsActive, true);
}

void ASPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerupActor, bIsActive);
}