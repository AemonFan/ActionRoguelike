// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"

#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetSimulatePhysics(true); // 是否受物理影响
	RootComponent = StaticMesh;

	RadialForce = CreateDefaultSubobject<URadialForceComponent>("RadialForce");
	RadialForce->SetupAttachment(RootComponent);
	RadialForce->SetAutoActivate(false);
	RadialForce->Radius = 1500.0f;  // 设置爆炸受力范围
	RadialForce->ImpulseStrength = 1000.0f; // 设置脉冲力度
	RadialForce->bImpulseVelChange = true; // 脉冲忽略物体质量的影响
	RadialForce->AddCollisionChannelToAffect(ECC_WorldDynamic); // 添加碰撞通道对象类型 为这个径向力添加一个碰撞通道以产生影响。
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 此方法在所有组件初始化之后被自动调用
void ASExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// AddDynamic：帮助宏，用于将UObject实例和成员UFUNCTION绑定到动态多播委托。
	StaticMesh->OnComponentHit.AddDynamic(this, &ASExplosiveBarrel::OnActorHit);
}

void ASExplosiveBarrel::OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpluse, const FHitResult& Hit)
{
	RadialForce->FireImpulse();
}

