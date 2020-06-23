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
	// 이 컴포넌트가 무언가에 겹칠 때에 대한 알림을 구성
	// https://docs.unrealengine.com/ko/Gameplay/ClassCreation/CodeOnly/index.html
	// 이득우씨 책 331p OnComponentBeginOverlap 델리게이트는 멀티캐스트 다이내믹 델리게이트
	// 유형과 인자를 모두 복사해 매크로 설정과 동일한 멤버함수 선언. 이를 해당 델리게이트에 바인딩하면
	// Overlap 이벤트가 발생할 때마다 바인딩한 멤버 함수가 호출.

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

