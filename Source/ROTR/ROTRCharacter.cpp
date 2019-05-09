// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ROTRCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// AROTRCharacter

AROTRCharacter::AROTRCharacter()
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // set our turn rates for input
    BaseTurnRate = 45.f;
    BaseLookUpRate = 45.f;

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
    GetCharacterMovement()->JumpZVelocity = 600.f;
    GetCharacterMovement()->AirControl = 0.2f;

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
    // are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bTickEvenWhenPaused = false;
    PrimaryActorTick.TickGroup = TG_PrePhysics;

    GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

bool AROTRCharacter::UpdateHealth(float Delta)
{
    Health += Delta;
    if (Health <= 0) { // Death
        HealthPercentage = 0.f;
        return false;
    }
    Health = FMath::Clamp(Health, 0.f, MaxHealth);
    HealthPercentage = Health / MaxHealth;
    return true;
}

bool AROTRCharacter::UpdateStamina(float Delta)
{
    Stamina += Delta;
    if (Stamina <= 0) {
        StaminaPercentage = 0.f;
        return false;
    }
    Stamina = FMath::Clamp(Stamina, 0.f, MaxStamina);
    StaminaPercentage = Stamina / MaxStamina;
    return true;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AROTRCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    // Set up gameplay key bindings
    check(PlayerInputComponent);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    PlayerInputComponent->BindAxis("MoveForward", this, &AROTRCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AROTRCharacter::MoveRight);

    // We have 2 versions of the rotation bindings to handle different kinds of devices differently
    // "turn" handles devices that provide an absolute delta, such as a mouse.
    // "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("TurnRate", this, &AROTRCharacter::TurnAtRate);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("LookUpRate", this, &AROTRCharacter::LookUpAtRate);

    // handle touch devices
    PlayerInputComponent->BindTouch(IE_Pressed, this, &AROTRCharacter::TouchStarted);
    PlayerInputComponent->BindTouch(IE_Released, this, &AROTRCharacter::TouchStopped);

    // VR headset functionality
    PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AROTRCharacter::OnResetVR);

    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AROTRCharacter::Sprint);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AROTRCharacter::StopSprinting);
    PlayerInputComponent->BindAction("CrawlToggle", IE_Released, this, &AROTRCharacter::ToggleCrawling);
}

void AROTRCharacter::Sprint()
{
    IsSprinting = true;
    GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AROTRCharacter::StopSprinting()
{
    IsSprinting = false;
    GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AROTRCharacter::ToggleCrawling()
{
    if (!GetCharacterMovement()->IsCrouching())
        Crouch();
    else
        UnCrouch();
}

void AROTRCharacter::OnResetVR()
{
    UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AROTRCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
    Jump();
}

void AROTRCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
    StopJumping();
}

void AROTRCharacter::Tick(float DeltaSeconds)
{
    float staminaDelta = StaminaRegen;
    if (GetCharacterMovement()->IsMovementInProgress())
        ;
    else if (IsSprinting)
        staminaDelta -= SprintCost;
    else
        staminaDelta -= RunCost;
    if (!UpdateStamina(staminaDelta * DeltaSeconds))
        StopSprinting();
}

void AROTRCharacter::TurnAtRate(float Rate)
{
    // calculate delta for this frame from the rate information
    AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AROTRCharacter::LookUpAtRate(float Rate)
{
    // calculate delta for this frame from the rate information
    AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AROTRCharacter::MoveForward(float Value)
{
    if ((Controller != NULL) && (Value != 0.0f))
    {
        // find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AROTRCharacter::MoveRight(float Value)
{
    if ( (Controller != NULL) && (Value != 0.0f) )
    {
        // find out which way is right
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get right vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        // add movement in that direction
        AddMovementInput(Direction, Value);
    }
}
