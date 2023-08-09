// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainCharacter.h"

void UMainAnimInstance::NativeInitializeAnimation()
{
	UE_LOG(LogTemp, Warning, TEXT("here %f"), MovementSpeed);
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			MainCharacter = Cast<AMainCharacter>(Pawn);
		}
	}

}

void UMainAnimInstance::UpdateAnimationProperties()
{
	UE_LOG(LogTemp, Warning, TEXT("second %f"), MovementSpeed);
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}
	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();

		UE_LOG(LogTemp, Warning, TEXT("third %f"), MovementSpeed);

		

		if (MainCharacter == nullptr)
		{
			MainCharacter = Cast<AMainCharacter>(Pawn);
		}
	}


}