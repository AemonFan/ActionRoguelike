// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionEffect.h"

#include "SActionComponent.h"

USActionEffect::USActionEffect()
{
	bIsAutoStart = true;
	
	Duration = 0.0f;

	Period = 0.0f;
}

void USActionEffect::StartAction_Implementation(AActor* InstigatorActor)
{
	Super::StartAction_Implementation(InstigatorActor);

	if(Duration > 0.0f)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "StopAction", InstigatorActor);
		GetWorld()->GetTimerManager().SetTimer(Duration_TimerHandle, Delegate, Duration, false);
	}

	if(Period > 0.0f)
	{
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "ExecutePeriodicEffect", InstigatorActor);
		GetWorld()->GetTimerManager().SetTimer(Period_TimerHandle, Delegate, Period, true);
	}
}

void USActionEffect::StopAction_Implementation(AActor* InstigatorActor)
{
	if(GetWorld()->GetTimerManager().GetTimerRemaining(Period_TimerHandle) < KINDA_SMALL_NUMBER)
	{
		ExecutePeriodicEffect(InstigatorActor);
	}
	
	Super::StopAction_Implementation(InstigatorActor);

	GetWorld()->GetTimerManager().ClearTimer(Duration_TimerHandle);

	GetWorld()->GetTimerManager().ClearTimer(Period_TimerHandle);

	USActionComponent* Comp = GetOwningComponent();
	if(Comp)
	{
		Comp->RemoveAction(this);
	}
}

float USActionEffect::GetActionTimeRemaining() const
{
	float ActionEndTime = ActionStartTime + Duration;
	return ActionEndTime - GetWorld()->TimeSeconds;
}

void USActionEffect::ExecutePeriodicEffect_Implementation(AActor* InstigatorActor)
{
	
}
