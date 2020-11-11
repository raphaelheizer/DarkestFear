// Copyright Epic Games, Inc. All Rights Reserved.

#include "DarkestFearCharacter.h"


#include "DarkestFearProjectile.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "Item.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ADarkestFearCharacter

ADarkestFearCharacter::ADarkestFearCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

    // Use distance
    UseLineDistance = 200.f;

    // Movement modifiers
    MovementFactor = .6f;
    StrafeMovementFactor = .5f;

    // set our turn rates for input
    BaseTurnRate = 45.f;
    BaseLookUpRate = 45.f;

    // Create a CameraComponent	
    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
    FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
    FirstPersonCameraComponent->bUsePawnControlRotation = true;

    // Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
    Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
    Mesh1P->SetOnlyOwnerSee(true);
    Mesh1P->SetupAttachment(FirstPersonCameraComponent);
    Mesh1P->bCastDynamicShadow = false;
    Mesh1P->CastShadow = false;
    Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
    Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

    // Create VR Controllers.
    R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
    R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
    R_MotionController->SetupAttachment(RootComponent);
    L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
    L_MotionController->SetupAttachment(RootComponent);

    // Create Ghosting Mesh
    ActiveItemGhost = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemGhosting"));

    // Uncomment the following line to turn motion controllers on by default:
    //bUsingMotionControllers = true;
}

void ADarkestFearCharacter::BeginPlay()
{
    // Call the base class  
    Super::BeginPlay();

    ActiveItemGhost->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

    // Show or hide the two versions of the gun based on whether or not we're using motion controllers.
    if (bUsingMotionControllers)
    {
        Mesh1P->SetHiddenInGame(true, true);
    }
    else
    {
        Mesh1P->SetHiddenInGame(false, true);
    }
}

void ADarkestFearCharacter::Tick(float DeltaTime)
{
    if (bIsPlacing)
    {
        ADarkestFearCharacter::DisplayPlacementPivot();
    }
}

//////////////////////////////////////////////////////////////////////////
// Input

void ADarkestFearCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    // set up gameplay key bindings
    check(PlayerInputComponent);

    // Bind jump events
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    // Bind fire event
    PlayerInputComponent->BindAction("PrimaryUse", IE_Pressed, this, &ADarkestFearCharacter::OnPrimaryUse);
    PlayerInputComponent->BindAction("SecondaryUse", IE_Pressed, this, &ADarkestFearCharacter::OnSecondaryUse);

    // Bind to Pick Up Item use event
    PlayerInputComponent->BindAction("PickUpItem", IE_Pressed, this, &ADarkestFearCharacter::PickUpItem);

    // Bind to Select Item Slot @ InvCursor
    PlayerInputComponent->BindAction("UseItemSlot0", IE_Pressed, this, &ADarkestFearCharacter::OnUseSlot0);
    PlayerInputComponent->BindAction("UseItemSlot1", IE_Pressed, this, &ADarkestFearCharacter::OnUseSlot1);
    PlayerInputComponent->BindAction("UseItemSlot2", IE_Pressed, this, &ADarkestFearCharacter::OnUseSlot2);

    // Bind to Active item use event
    PlayerInputComponent->BindAction("UseActiveItem", IE_Pressed, this, &ADarkestFearCharacter::OnUseActiveItem);

    // Drop/put item action
    PlayerInputComponent->BindAction("PlaceItem", IE_Pressed, this, &ADarkestFearCharacter::OnBeginPlace);
    PlayerInputComponent->BindAction("PlaceItem", IE_Released, this, &ADarkestFearCharacter::OnFinishPlace);

    // Mouse Wheel Action
    PlayerInputComponent->BindAction("MouseWheelUp", IE_Pressed, this, &ADarkestFearCharacter::OnMouseWheelUp);
    PlayerInputComponent->BindAction("MouseWheelDown", IE_Pressed, this, &ADarkestFearCharacter::OnMouseWheelDown);

    // Enable touchscreen input
    EnableTouchscreenMovement(PlayerInputComponent);

    PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ADarkestFearCharacter::OnResetVR);

    // Bind movement events
    PlayerInputComponent->BindAxis("MoveForward", this, &ADarkestFearCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ADarkestFearCharacter::MoveRight);

    // We have 2 versions of the rotation bindings to handle different kinds of devices differently
    // "turn" handles devices that provide an absolute delta, such as a mouse.
    // "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
    PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis("TurnRate", this, &ADarkestFearCharacter::TurnAtRate);
    PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("LookUpRate", this, &ADarkestFearCharacter::LookUpAtRate);
}

void ADarkestFearCharacter::OnPrimaryUse()
{
    AItem* Item = Cast<AItem>(ADarkestFearCharacter::TraceLine().GetActor());

    if (Item != nullptr)
    {
        Item->Use(this);
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan,
                                         FString::Printf(TEXT("No Usable Item clicked")));
    }
}

void ADarkestFearCharacter::OnSecondaryUse()
{
    AItem* Item = Cast<AItem>(ADarkestFearCharacter::TraceLine().GetActor());

    if (Item != nullptr)
    {
        Item->AlternateUse(this);
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red,
                                         FString::Printf(TEXT("No Usable Item clicked with right button")));
    }
}

void ADarkestFearCharacter::PickUpItem()
{
    AItem* Item = Cast<AItem>(ADarkestFearCharacter::TraceLine().GetActor());

    if (Item != nullptr)
    {
        AItem* PickedUpItem = Item->Pickup(this);

        if (PickedUpItem != nullptr)
        {
            Inventory.Emplace(PickedUpItem);
            SetActiveItem(Inventory.Num() - 1);
        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Emerald,
                                         FString::Printf(TEXT("No item could be picked up!")));
    }
}

void ADarkestFearCharacter::OnBeginPlace()
{
    bIsPlacing = true;

    if (ActiveItem != nullptr)
    {
        ActiveItemGhost->SetStaticMesh(ActiveItem->MeshComponent->GetStaticMesh());
        ActiveItemGhost->SetWorldScale3D(FVector(0.05f, 0.05f, 0.05f));
        ActiveItemGhost->SetRelativeScale3D(FVector(0.05f, 0.05f, 0.05f));
        // (ActiveItem->MeshComponent->GetRelativeScale3D());
        ActiveItemGhost->SetHiddenInGame(false);

        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow,
                                         FString::Printf(TEXT("Item scale is: X: %f, Y: %f, Z: %f"),
                                                         ActiveItem->MeshComponent->GetRelativeScale3D().X,
                                                         ActiveItem->MeshComponent->GetRelativeScale3D().Y,
                                                         ActiveItem->MeshComponent->GetRelativeScale3D().Z
                                         ));
    }
}

void ADarkestFearCharacter::DisplayPlacementPivot()
{
    if (ActiveItem == nullptr)
        return;

    FString HitPointInfo;
    const FHitResult HitResult = ADarkestFearCharacter::TraceLine();

    if (HitResult.Location.IsZero())
    {
        HitPointInfo = "Nothing";
        ActiveItemGhost->SetHiddenInGame(true);
    }
    else
    {
        ActiveItemGhost->SetHiddenInGame(false);
        HitPointInfo = TEXT(
            "X: " + FString::FromInt(HitResult.Location.X) +
            ", Y: " + FString::FromInt(HitResult.Location.Y) +
            " , Z: " + FString::FromInt(HitResult.Location.Z));

        if (ActiveItemGhost != nullptr)
        {
            ActiveItemGhost->SetWorldTransform(FTransform(HitResult.Location));
            ActiveItemGhost->SetWorldRotation(GhostMeshPivotRotation);
        }
    }


    GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Yellow,
                                     FString::Printf(TEXT("You are facing: %s"),
                                                     *HitPointInfo
                                     ));
}

void ADarkestFearCharacter::OnFinishPlace()
{
    bIsPlacing = false;
    const FHitResult HitResult = ADarkestFearCharacter::TraceLine();

    if (!HitResult.Location.IsZero())
    {
        if (ActiveItem != nullptr)
        {
            ActiveItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
            // ActiveItem->SetActorLocation(HitResult.Location); // TODO: Must be able to change rotation on mousewheel
            ActiveItem->SetActorTransform(
                FTransform(FVector(HitResult.Location.X, HitResult.Location.Y, HitResult.Location.Z)));
            ActiveItem->SetActorRotation(GhostMeshPivotRotation - ActiveItem->MeshComponent->GetRelativeRotation());

            ActiveItemGhost->SetHiddenInGame(true);

            Inventory.Remove(ActiveItem);

            if (Inventory.Num() != 0)
                SetActiveItem(Inventory.Num() - 1);
            else
                ActiveItem = nullptr;
        }
    }
    else
    {
        ActiveItemGhost->SetHiddenInGame(true);
    }
}

void ADarkestFearCharacter::OnMouseWheelUp()
{
    GhostMeshPivotRotation.Add(0.0f, -10.f, 0.0f);
}

void ADarkestFearCharacter::OnMouseWheelDown()
{
    GhostMeshPivotRotation.Add(0.0f, 10.f, 0.0f);
}

void ADarkestFearCharacter::SetActiveItem(int8 Slot)
{
    if (Inventory.IsValidIndex(Slot))
    {
        ActiveItem = Inventory[Slot];
        InvCursor = Slot;
        // todo: Adicionar um struct ao Item contendo um int8 chamado SlottedIn ao invés de usar uma variável cursor??

        for (int i = 0; i < Inventory.Num(); i++)
        {
            if (Inventory[i] != ActiveItem)
                Inventory[i]->SetActorHiddenInGame(true);
            else
                Inventory[i]->SetActorHiddenInGame(false);
        }
    }
}

void ADarkestFearCharacter::OnUseSlot0()
{
    SetActiveItem(0);
}

void ADarkestFearCharacter::OnUseSlot1()
{
    SetActiveItem(1);
}

void ADarkestFearCharacter::OnUseSlot2()
{
    SetActiveItem(2);
}

void ADarkestFearCharacter::OnUseActiveItem()
{
    if (ActiveItem != nullptr)
        ActiveItem->Use(this);
}

void ADarkestFearCharacter::OnResetVR()
{
    UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ADarkestFearCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
    if (TouchItem.bIsPressed == true)
    {
        return;
    }
    if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
    {
        OnPrimaryUse();
    }
    TouchItem.bIsPressed = true;
    TouchItem.FingerIndex = FingerIndex;
    TouchItem.Location = Location;
    TouchItem.bMoved = false;
}

void ADarkestFearCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
    if (TouchItem.bIsPressed == false)
    {
        return;
    }
    TouchItem.bIsPressed = false;
}

void ADarkestFearCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void ADarkestFearCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void ADarkestFearCharacter::MoveForward(float Value)
{
    if (Value != 0.0f)
    {
        // add movement in that direction
        AddMovementInput(GetActorForwardVector(), (Value * MovementFactor));
    }
}

void ADarkestFearCharacter::MoveRight(float Value)
{
    if (Value != 0.0f)
    {
        // add movement in that direction
        AddMovementInput(GetActorRightVector(), (Value * StrafeMovementFactor));
    }
}

void ADarkestFearCharacter::TurnAtRate(float Rate)
{
    // calculate delta for this frame from the rate information
    AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ADarkestFearCharacter::LookUpAtRate(float Rate)
{
    // calculate delta for this frame from the rate information
    AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ADarkestFearCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
    if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
    {
        PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ADarkestFearCharacter::BeginTouch);
        PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ADarkestFearCharacter::EndTouch);

        //Commenting this out to be more consistent with FPS BP template.
        //PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ADarkestFearCharacter::TouchUpdate);
        return true;
    }

    return false;
}


FHitResult ADarkestFearCharacter::TraceLine()
{
    FHitResult OutHit;
    const FVector StartPoint = GetFirstPersonCameraComponent()->GetComponentLocation();
    const FVector ForwardVec = GetFirstPersonCameraComponent()->GetForwardVector();
    const FVector EndPoint = ((ForwardVec * UseLineDistance) + StartPoint);
    FCollisionQueryParams CollisionQueryParams;

    // DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor, false, 1, 0, 1);

    GetWorld()->LineTraceSingleByChannel(
        OutHit,
        StartPoint,
        EndPoint,
        ECC_Visibility,
        CollisionQueryParams);

    return OutHit;
}
