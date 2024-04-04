// Fill out your copyright notice in the Description page of Project Settings.


#include "SDashProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "particles/ParticleSystemComponent.h"

ASDashProjectile::ASDashProjectile()
{
	TeleportDelay = 0.2f;
	DetonateDelay = 0.2f;

	MovementComp->InitialSpeed = 6000.f;
}

void ASDashProjectile::BeginPlay()
{
	Super::BeginPlay();

	// 设置爆炸延迟
	GetWorldTimerManager().SetTimer(TimerHandle_DelayedDetonate, this, &ASDashProjectile::Explode, DetonateDelay);
}

void ASDashProjectile::Explode_Implementation()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_DelayedDetonate);

	// 产生一个粒子发射器
	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());

	// 关闭粒子系统
	EffectComp->DeactivateSystem();

	// 停止运动
	MovementComp->StopMovementImmediately();

	// 关闭碰撞检测
	SetActorEnableCollision(false);

	GetWorldTimerManager().SetTimer(TimerHandle_DelayedTeleport, this, &ASDashProjectile::TeleportInstigator, TeleportDelay);
}

void ASDashProjectile::TeleportInstigator()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_DelayedTeleport);
	
	AActor* ActorToTeleport = GetInstigator();
	if(ensure(ActorToTeleport))
	{
		// 把 instigator 传送到当前 Actor 所在位置，instigator方向不改变
		FVector OldLocation = ActorToTeleport->GetActorLocation();
		FVector ProjLocation = GetActorLocation();
		FVector NewLocation = FVector(ProjLocation.X, ProjLocation.Y, OldLocation.Z);
		ActorToTeleport->TeleportTo(NewLocation, ActorToTeleport->GetActorRotation(), false, false);
	}

	Destroy();
}
