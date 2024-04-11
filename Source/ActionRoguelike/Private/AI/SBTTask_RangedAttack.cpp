// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_RangedAttack.h"

#include "AIController.h"
#include "SAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/ControlChannel.h"
#include "GameFramework/Character.h"


USBTTask_RangedAttack::USBTTask_RangedAttack()
{
	BulletRangePitch = 2.0f;

	BulletRangeYaw = 8.0f;
}

EBTNodeResult::Type USBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if(ensure(AIController))
	{
		ACharacter* AICharacter = Cast<ACharacter>(AIController->GetPawn());
		if(AICharacter == nullptr)
		{
			return EBTNodeResult::Failed;
		}
		
		if(!USAttributeComponent::IsActorAlive(AICharacter))
		{
			return EBTNodeResult::Failed;
		}
		
		AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorKey.SelectedKeyName));
		if(TargetActor == nullptr)
		{
			return  EBTNodeResult::Failed;
		}

		FVector HandLocation = AICharacter->GetMesh()->GetSocketLocation("Muzzle_01");
		FRotator HandRotator = (TargetActor->GetActorLocation() - HandLocation).Rotation();
		HandRotator.Pitch += FMath::RandRange(0.0f, BulletRangePitch); // 上下
		HandRotator.Yaw += FMath::RandRange(BulletRangeYaw * (-1), BulletRangeYaw);  // 左右

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.Instigator = AICharacter; // Instigator : 对生成的Actor造成的伤害负责的 APawn

		AActor* Projectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, HandLocation, HandRotator, Params);

		return Projectile ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}
	
	return EBTNodeResult::Failed;
}