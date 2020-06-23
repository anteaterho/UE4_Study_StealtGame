// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSLunchPad.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AFPSLunchPad::AFPSLunchPad()
{
	OverlapComp = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComp"));
	OverlapComp->SetBoxExtent(FVector(75, 75, 50));
	RootComponent = OverlapComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	//Bind to Event
	// �� ������Ʈ�� ���𰡿� ��ĥ ���� ���� �˸��� ����
	// https://docs.unrealengine.com/ko/Gameplay/ClassCreation/CodeOnly/index.html
	// �̵�쾾 å 331p OnComponentBeginOverlap ��������Ʈ�� ��Ƽĳ��Ʈ ���̳��� ��������Ʈ
	// ������ ���ڸ� ��� ������ ��ũ�� ������ ������ ����Լ� ����. �̸� �ش� ��������Ʈ�� ���ε��ϸ�
	// Overlap �̺�Ʈ�� �߻��� ������ ���ε��� ��� �Լ��� ȣ��.

	OverlapComp->OnComponentBeginOverlap.AddDynamic(this, &AFPSLunchPad::OverlapLaunchPad);

	LaunchStrength = 1500;
	LaunchPitchAngle = 35.0f;

}

// Called when the game starts or when spawned
//void AFPSLunchPad::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}

void AFPSLunchPad::OverlapLaunchPad(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, 
	UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	FRotator LaunchDirection = GetActorRotation();
	LaunchDirection.Pitch += LaunchPitchAngle;
	FVector LaunchVelocity = LaunchDirection.Vector() * LaunchStrength;

	ACharacter* OtherChracter = Cast<ACharacter>(OtherActor);
	if (OtherChracter)
	{
		// Launch Player! Both booleans give consistent launch velocity by ignoring the current player velocity
		OtherChracter->LaunchCharacter(LaunchVelocity, true, true);

		// Spawn FX
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ActiveLaunchPadEffect, GetActorLocation());
	}
	else if (OtherComp && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulse(LaunchVelocity, NAME_None, true);

		//Sapwn FX
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ActiveLaunchPadEffect, GetActorLocation());
	}
}

