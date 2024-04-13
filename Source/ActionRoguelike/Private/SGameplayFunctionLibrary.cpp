// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameplayFunctionLibrary.h"

#include "SAttributeComponent.h"

bool USGameplayFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float Damage)
{
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(TargetActor);
	if(AttributeComp)
	{
		return AttributeComp->ApplyHealthChange(DamageCauser, Damage);
	}

	return false;
}

bool USGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float Damage,	const FHitResult& HitResult)
{
	if(ApplyDamage(DamageCauser, TargetActor, Damage))
	{
		UPrimitiveComponent* HitComp = HitResult.GetComponent();
		if(HitComp && HitComp->IsSimulatingPhysics(HitResult.BoneName))
		{
			HitComp->AddImpulseAtLocation(HitResult.ImpactNormal * (-1) * 30000.0f, HitResult.ImpactPoint, HitResult.BoneName);
		}
		return true;
	}

	return false;
}