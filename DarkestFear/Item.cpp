// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

// Sets default values
AItem::AItem()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    // Every item can be picked up by default, but it should not be stolen if its in player hands
    bCanPickup = true;

    ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("ItemForward"));

    if (ArrowComponent)
    {
        RootComponent = ArrowComponent;

        ArrowComponent->ArrowColor = FColor::White;
        ArrowComponent->ArrowSize = .35f;
        ArrowComponent->EditorScale = 1.0f; // ArrowComponent MUST NOT be scaled
        ArrowComponent->bIsEditorOnly = true;
        ArrowComponent->bTreatAsASprite = true;
    }

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

    if (MeshComponent)
    {
        MeshComponent->SetupAttachment(ArrowComponent);
        MeshComponent->SetRelativeLocation(FVector(0, 0, 0));
    }
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
    Super::BeginPlay();
}

void AItem::Use(class ADarkestFearCharacter* DarkestFearCharacter)
{
    // Implement only in child items
}

void AItem::AlternateUse(ADarkestFearCharacter* DarkestFearCharacter)
{
    // Implement only in child items
}

/**
 * Gives the player this item and sets its default properties.
 * Picked up items are attached to the default right hand socket by default
 * Also, every picked up item has Physics and collision disabled.
 *
 * @date 11/09/2020
 */
AItem* AItem::Pickup(ADarkestFearCharacter* DarkestFearCharacter, FName AttachmentName)
{
    if (bCanPickup)
    {
        if (DarkestFearCharacter)
        {
            /* todo 11/09/2020: Disable collision and stuff before attaching to player to avoid weird behaviors!
             *     Remember to do this by setting collision groups! It'd be nice if those groups could be made
             *     programmatically.
             */

            this->AttachToComponent(
                DarkestFearCharacter->GetMesh1P(),
                FAttachmentTransformRules::SnapToTargetNotIncludingScale, // Item should never be rescaled!
                AttachmentName
            );

            return this;
        }
    }
    return nullptr;
}
