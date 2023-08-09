// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimBlueprint.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraInitialPosition= FVector(0.0f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 0;// Camera follows at this distance
	CameraBoom->bUsePawnControlRotation = true; //Rotate as Based on controller

	CameraBoom->SetRelativeLocation(FVector(GetCapsuleComponent()->GetScaledCapsuleRadius(), 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));

	

	GetMesh()->SetRelativeLocation(FVector(0, 0, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	GetMesh()->SetRelativeRotation(FRotator(0, -90.0f, 0));
	

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//Attach the camera to the end of the boom and let the boom adjust to match 
	// The controller oriantation 
	FollowCamera->bUsePawnControlRotation = false;

	//Set our turn rates for input 
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	RunningSpeed = 950.f;
	WalkingSpeed = 650.f;
	CrouchingSpeed = 350.0f;
	
	//	Character movement initial movement status and direction
	//	direction->idle
	//	movement->normal(without sprinting or crouching)
	MovementStatus = EPlayerMovementStatus::EMS_Normal;
	MovementDirection = EPlayerMovementDirection::EMS_MAX;

	IndexCurrentCharacter = 0;

	UserIventoryUIStatus = false;
	ItemTouch = false;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	CameraInitialPosition = CameraBoom->GetRelativeLocation();
	
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	const FVector InitialRaycastVector = FollowCamera->GetComponentTransform().GetLocation();
	const FVector EndRaycastVector =InitialRaycastVector + FollowCamera->GetComponentTransform().GetRotation().GetForwardVector()*1000;
	FHitResult ElementHit;
	FCollisionQueryParams Params;

	GetWorld()->LineTraceSingleByChannel(ElementHit, InitialRaycastVector, EndRaycastVector, ECollisionChannel::ECC_MAX, Params);
	DrawDebugLine(GetWorld(),InitialRaycastVector,EndRaycastVector, FColor::Red,false,0.0f);

	if (ElementHit.bBlockingHit) 
	{
		
		AItem* Item = Cast<AItem>(ElementHit.GetActor());
		if (Item)
		{
			UE_LOG(LogTemp, Warning, TEXT("im an Element"));
			ItemTouch = true;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("im not an element"));
			ItemTouch = false;
		}
		
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("False pa"));
	}

}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMainCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("StopMovementForward", IE_Released, this, &AMainCharacter::StopMovement);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AMainCharacter::ChangetoRun);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AMainCharacter::ChangeToNormal);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMainCharacter::ChangeToCrouch);

	PlayerInputComponent->BindAction("ChangeCharacter", IE_Pressed, this, &AMainCharacter::ChangeCharacter);
	
	PlayerInputComponent->BindAction("ManageInventory", IE_Pressed, this, &AMainCharacter::ManageInventory);

}

void AMainCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && !UserIventoryUIStatus)
	{
		// Find out witch way is m forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		if (Value > 0) 
		{
			SetPlayerMovementDirection(EPlayerMovementDirection::EMS_Forward);
		}
		if (Value < 0)
		{
			SetPlayerMovementDirection(EPlayerMovementDirection::EMS_Backward);
		}

		AddMovementInput(Direction, Value);
		
	}

	

}

void AMainCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f) && !UserIventoryUIStatus)
	{
		// Find out witch way is m forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		if (Value > 0)
		{
			SetPlayerMovementDirection(EPlayerMovementDirection::EMS_Right);
		}
		if (Value < 0)
		{
			SetPlayerMovementDirection(EPlayerMovementDirection::EMS_Left);
		}
		
		AddMovementInput(Direction, Value);
	}


}

void AMainCharacter::TurnAtRate(float Rate)
{
	if (!UserIventoryUIStatus)
	{
		AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
	
}

void AMainCharacter::LookUpAtRate(float Rate)
{
	if (!UserIventoryUIStatus)
	{
		AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
	
}

void AMainCharacter::StopMovement()
{
	SetPlayerMovementDirection(EPlayerMovementDirection::EMS_MAX);
}
void AMainCharacter::ChangetoRun()
{
	SetPlayerMovementStatus(EPlayerMovementStatus::EMS_Sprinting);
}
void AMainCharacter::ChangeToCrouch()
{
	if (MovementStatus == EPlayerMovementStatus::EMS_Crouching)
	{
		SetPlayerMovementStatus(EPlayerMovementStatus::EMS_Normal);
	}
	else
	{
		SetPlayerMovementStatus(EPlayerMovementStatus::EMS_Crouching);
	}	
}
void AMainCharacter::ChangeToNormal()
{
	SetPlayerMovementStatus(EPlayerMovementStatus::EMS_Normal);
}

void AMainCharacter::SetPlayerMovementStatus(EPlayerMovementStatus Status)
{
	MovementStatus = Status;
	if (MovementStatus == EPlayerMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
		CameraBoom->SetRelativeLocation(FVector(CameraInitialPosition.X, CameraInitialPosition.Y, CameraInitialPosition.Z));
	}
	else
	{
		if (MovementStatus == EPlayerMovementStatus::EMS_Normal)
		{
			GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
			CameraBoom->SetRelativeLocation(FVector(CameraInitialPosition.X, CameraInitialPosition.Y, CameraInitialPosition.Z));
		}
		else 
		{
			GetCharacterMovement()->MaxWalkSpeed = CrouchingSpeed;
			CameraBoom->SetRelativeLocation(FVector(CameraInitialPosition.X + 20.0f, CameraInitialPosition.Y + 30.0f, CameraInitialPosition.Z/2));
		}
		
	}
}


void AMainCharacter::SetPlayerMovementDirection(EPlayerMovementDirection Direction)
{
	MovementDirection = Direction;
}

void AMainCharacter::ChangeCharacter() {

	if (AvaliableMeshes.Num() > 0 && AnimationsClasesOptions.Num() > 0 && !UserIventoryUIStatus)
	{
		if (IndexCurrentCharacter + 1 >= AvaliableMeshes.Num())
		{
			IndexCurrentCharacter = 0;

			GetMesh()->SetSkeletalMesh(AvaliableMeshes[IndexCurrentCharacter]);
			GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
			GetMesh()->SetAnimClass(AnimationsClasesOptions[IndexCurrentCharacter]->GeneratedClass);
		}
		else {
			IndexCurrentCharacter = IndexCurrentCharacter + 1;

			GetMesh()->SetSkeletalMesh(AvaliableMeshes[IndexCurrentCharacter]);
			GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
			GetMesh()->SetAnimClass(AnimationsClasesOptions[IndexCurrentCharacter]->GeneratedClass);
		}
	}

}

void AMainCharacter::ManageInventory()
{
	if (UserIventoryUIStatus)
	{
		UserIventoryUIStatus = false;
	}
	else {
		UserIventoryUIStatus = true;
	}
	
}