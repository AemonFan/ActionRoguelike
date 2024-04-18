// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction.h"

void USAction::StartAction_Implementation(AActor* InstigatorActor)
{
	UE_LOG(LogTemp, Warning, TEXT("%s Runnning Action %s"), *GetNameSafe(InstigatorActor), *GetNameSafe(this));
}

void USAction::StopAction_Implementation(AActor* InstigatorActor)
{
	UE_LOG(LogTemp, Warning, TEXT("%s Stop Action %s"), *GetNameSafe(InstigatorActor), *GetNameSafe(this));
}

UWorld* USAction::GetWorld() const
{
	// 获取当前对象绑定的组件，通过组件GetWorld
	UActorComponent* Comp = Cast<UActorComponent>(GetOuter());
	if(Comp)
	{
		return Comp->GetWorld();
	}
	return nullptr;
}
