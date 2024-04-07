// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"

#include "PhysXInterfaceWrapperCore.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SInteractionComponent.h"
#include "SAttributeComponent.h"
#include "SMagicProjectile.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComp");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");
	
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;

	AttackAnimDelay = 0.2f;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("TurnAround", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::JumpStart);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASCharacter::JumpEnd);

	PlayerInputComponent->BindAction("OpenTreasure", IE_Pressed, this, &ASCharacter::OpenTreasureChest);
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
  
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthValueChanged);
}

void ASCharacter::OnHealthValueChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,
	float Delta, float MaxHealth, float DangerousHealth)
{
	if(Delta < 0.0f)
	{
		// Hit Flash Material
		GetMesh()->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->GetTimeSeconds());		
	}
	
	//const bool bAlive = AttributeComp->IsCharacterAlive();
	if (NewHealth <= 0.0f && Delta < 0.0f)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);
	}
}

void ASCharacter::MoveForward(float value)
{
	FRotator ControlRot = GetControlRotation();
	//ControlRot.Yaw
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	AddMovementInput(ControlRot.Vector(), value);
	// AddMovementInput(GetActorForwardVector(), value);
}

void ASCharacter::MoveRight(float value)
{
	FRotator ControlRot = GetControlRotation();
	// Yaw 左右摇摆; Pitch 上下摇摆; Roll 左右翻转
	ControlRot.Pitch = 0.0f; 
	ControlRot.Roll = 0.0f;

	// 虚幻世界坐标系为左手坐标系，即：
	// x = Forward (Red); Y = Right (Green);  Z = Up (Blue);
	const FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y); // 获取旋转矩阵ControlRot在Y轴上的向量
	AddMovementInput(RightVector, value);
}

void ASCharacter::JumpStart()
{
	bPressedJump = true;
}

void ASCharacter::JumpEnd()
{
	bPressedJump = false;
}

void ASCharacter::OpenTreasureChest()
{
	if (InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}

void ASCharacter::SpawnProjectile(TSubclassOf<AActor> ClassToSpawn)
{
	// ensure ： 确保 ProjectileClass 存在，否则出发断言，仅在第一次执行时出发
	if(ensure(ClassToSpawn))
	{
		const FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

		// 指定投射物生成方式
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this; // Instigator : 对生成的Actor造成的伤害负责的 APawn

		// 碰撞追踪形状
		FCollisionShape Shape;
		Shape.SetSphere(20.0f);

		// 碰撞查询忽略当前Character本身
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		
		// 添加碰撞查询类型
		FCollisionObjectQueryParams ObjParams;
		ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjParams.AddObjectTypesToQuery(ECC_Pawn);
		
		// 追踪起点为摄像机的位置，终点为当前摄像机位置的5000米
		FVector TraceStart = CameraComp->GetComponentLocation();
		FVector TraceEnd = CameraComp->GetComponentLocation() + (GetControlRotation().Vector() * 5000);

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
		GetWorld()->SpawnActor<AActor>(ClassToSpawn, SpawnTM, SpawnParams);
	}
}

void ASCharacter::PrimaryAttack()
{
	PlayAnimMontage(AttackAnim);

	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::PrimaryAttack_TimerElapsed, AttackAnimDelay);

	// 角色死亡后应该清理定时器句柄Handle
	// GetWorldTimerManager().ClearTimer(AttackTimerHandle);
}

void ASCharacter::PrimaryAttack_TimerElapsed()
{
	SpawnProjectile(ProjectileClass);
}

void ASCharacter::BlackHoleAttack()
{
	PlayAnimMontage(AttackAnim);

	GetWorldTimerManager().SetTimer(TimerHandle_BlackHoleAttack, this, &ASCharacter::BlackHole_TimeElapsed, AttackAnimDelay);
}

void ASCharacter::BlackHole_TimeElapsed()
{
	SpawnProjectile(BlackHoleProjectileClass);
}

void ASCharacter::Dash()
{
	PlayAnimMontage(AttackAnim);

	GetWorldTimerManager().SetTimer(TimerHanle_Dash, this, &ASCharacter::Dash_TimeElapsed, AttackAnimDelay);
}

void ASCharacter::Dash_TimeElapsed()
{
	SpawnProjectile(DashProjectileClass);
}