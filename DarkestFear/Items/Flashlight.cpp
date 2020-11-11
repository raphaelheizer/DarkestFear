// Fill out your copyright notice in the Description page of Project Settings.


#include "Flashlight.h"

// Sets default values
AFlashlight::AFlashlight()
{
    /*
     * Add flashlight's spotlight without offset. Offset must be done manually.
     * Assuming the flashlight's position makes no sense at all
    */
    SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Spotlight"));
    SpotLight->SetupAttachment(Super::MeshComponent);
    SpotLight->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

    // Flashlight's default values don't override it here. Change in blueprints!
    SpotLight->InnerConeAngle = 15.f;
    SpotLight->OuterConeAngle = 30.f;
    SpotLight->Intensity = 1000.f;
    SpotLight->AttenuationRadius = 1500.f;
    SpotLight->SourceRadius = 5.f;
    SpotLight->SoftSourceRadius = 3.f;
    SpotLight->Temperature = 6500.f;

    // Add and attach any components below this line:
}

// Called when the game starts or when spawned
void AFlashlight::BeginPlay()
{
    Super::BeginPlay();
}

void AFlashlight::Use(class ADarkestFearCharacter* DarkestFearCharacter)
{
    SpotLight->ToggleVisibility();
    // todo: play click sound
}

void AFlashlight::AlternateUse(ADarkestFearCharacter* DarkestFearCharacter)
{
    
}
