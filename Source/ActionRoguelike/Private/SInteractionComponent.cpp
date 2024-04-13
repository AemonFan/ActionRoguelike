// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"

#include "DrawDebugHelpers.h"
#include "SGamePlayInterface.h"

static TAutoConsoleVariable<bool> CVarShowDrawDebug(TEXT("su.ShowDrawDebug"), false, TEXT("Interaction ShowDrawDebug Switch"), ECVF_Cheat);

USInteractionComponent::USInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USInteractionComponent::PrimaryInteract()
{
	AActor* MyOwner = GetOwner();

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation); // 获取Character的Eye位置和角度
	FVector End = EyeLocation + (EyeRotation.Vector() * 1000); // 从眼睛的位置开始到眼睛方向的1000单位

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic); // 设置碰撞查询
	
	bool bUseLogic1 = false;
	bool bDrawDebug = CVarShowDrawDebug.GetValueOnGameThread();
	
	if (bUseLogic1)
	{
		FHitResult Hit;
		const bool bIsHitObject = GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjectQueryParams); // 获取碰撞信息

		AActor* HitActor = Hit.GetActor(); // 获取碰撞对象
		if (HitActor)
		{
			// 判断碰撞对象是否实现了交互接口
			if (HitActor->Implements<USGamePlayInterface>())
			{
				APawn* MyPawn = Cast<APawn>(MyOwner);
				ISGamePlayInterface::Execute_Interact(HitActor, MyPawn); // 执行交互逻辑
			}
		}
		
		const FColor HitColor = bIsHitObject ? FColor::Green : FColor::Red;

		if(bDrawDebug)
		{
			DrawDebugLine(GetWorld(), EyeLocation, End, HitColor, false, 2.0f, 0, 1.0f);
		}
	}
	else
	{
		TArray<FHitResult> Hits;
		float Radius = 50.0f;
		FCollisionShape Shape;
		Shape.SetSphere(Radius);

		// 以Shape为检测范围碰撞检测
		bool bHitObject = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);

		FColor HitColor = bHitObject ? FColor::Green : FColor::Red;

		// 遍历碰撞到的所有对象
		for(FHitResult Hit : Hits)
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActor)
			{
				if(HitActor->Implements<USGamePlayInterface>())
				{
					APawn* MyPawn = Cast<APawn>(MyOwner);
					ISGamePlayInterface::Execute_Interact(HitActor, MyPawn);

					if(bDrawDebug)
					{
						DrawDebugSphere(GetWorld(), Hit.ImpactPoint, Radius, 32, HitColor, false, 2.0f);
					}
					break;
				}
			}
		}

		if(bDrawDebug)
		{
			DrawDebugLine(GetWorld(), EyeLocation, End, HitColor, false, 2.0f, 0, 1.0f);
		}
	}
}