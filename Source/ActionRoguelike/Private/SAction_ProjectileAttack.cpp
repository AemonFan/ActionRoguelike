// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction_ProjectileAttack.h"

#include "SCharacter.h"
#include "Kismet/GameplayStatics.h"

USAction_ProjectileAttack::USAction_ProjectileAttack()
{
	AttackAnimDelay = 0.2f;

	HandSocketName = "Muzzle_01";
}

void USAction_ProjectileAttack::StartAction_Implementation(AActor* InstigatorActor)
{
	Super::StartAction_Implementation(InstigatorActor);
	
	if(!ensure(InstigatorActor))
	{
		return;
	}
	
	ASCharacter* PlayerCharacter = Cast<ASCharacter>(InstigatorActor);
	if(!PlayerCharacter)
	{
		return;
	}
	
	if(!ensure(AttackAnim))
	{
		return;
	}
	
	PlayerCharacter->PlayAnimMontage(AttackAnim);

	// SnapToTarget : 锁定目标
	UGameplayStatics::SpawnEmitterAttached(CastingEffect, PlayerCharacter->GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
	
	FTimerDelegate InDelegate;
	InDelegate.BindUFunction(this, "OnAttackDelay_TimerElapsed", PlayerCharacter);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, InDelegate, AttackAnimDelay, false);
}

void USAction_ProjectileAttack::OnAttackDelay_TimerElapsed(ACharacter* InstigatorCharacter)
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_AttackDelay);
	
	if(!ensure(InstigatorCharacter))
	{
		return;
	}

	const FVector HandLocation = InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);

	// 指定投射物生成方式
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = InstigatorCharacter; // Instigator: 对生成的Actor造成的伤害负责的 APawn

	// 碰撞追踪形状
	FCollisionShape Shape;
	Shape.SetSphere(20.0f);

	// 碰撞查询忽略当前Character本身
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(InstigatorCharacter);
	
	// 添加碰撞查询类型
	FCollisionObjectQueryParams ObjParams;
	ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjParams.AddObjectTypesToQuery(ECC_Pawn);
	
	// 追踪起点为摄像机的位置，终点为当前摄像机位置的5000米
	FVector TraceStart = InstigatorCharacter->GetPawnViewLocation();
	FVector TraceEnd = TraceStart + (InstigatorCharacter->GetControlRotation().Vector() * 5000);

	// 追踪扫描，遇到第一个碰撞对象即返回
	FHitResult Hit;
	if (GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity, ObjParams, Shape, Params))
	{
		// 有检测到碰撞对象，保存碰撞对象位置
		TraceEnd = Hit.ImpactPoint;
	}

	// 根据目标位置和方向生成 transform
	FRotator ProjRotation = FRotationMatrix::MakeFromX(TraceEnd - HandLocation).Rotator();
	FTransform SpawnTM = FTransform(ProjRotation, HandLocation);
	
	// 在目前位置生成投射物
	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);

	// 停止动作
	StopAction(InstigatorCharacter);
}
