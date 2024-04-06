// Fill out your copyright notice in the Description page of Project Settings.

#include "SMagicProjectile.h"

#include "SAttributeComponent.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Damage = -10.0f;
}

// Called when the game starts or when spawned
void ASMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASMagicProjectile::OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpluse, const FHitResult& Hit)
{
	Super::OnActorHit(HitComp, OtherActor, OtherComp, NormalImpluse, Hit);
	
	if(OtherActor && OtherActor != GetInstigator()) // 确保角色自己生成的投射物不会对自己造成伤害
	{
		USAttributeComponent* OtherAttributeCom = Cast<USAttributeComponent>(OtherActor->GetComponentByClass(USAttributeComponent::StaticClass()));
		if(OtherAttributeCom)
		{
			OtherAttributeCom->ApplyHealthChange(Damage);
		}
	}
}

void ASMagicProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if(OtherActor && OtherActor != GetInstigator()) // 确保角色自己生成的投射物不会对自己造成伤害
	{
		USAttributeComponent* OtherAttributeCom = Cast<USAttributeComponent>(OtherActor->GetComponentByClass(USAttributeComponent::StaticClass()));
		if(OtherAttributeCom)
		{
			Explode();
	
			OtherAttributeCom->ApplyHealthChange(Damage);
		}
	}
}

// Called every frame
void ASMagicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
