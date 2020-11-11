// Fill out your copyright notice in the Description page of Project Settings.


#include "Phone.h"

// Sets default values
APhone::APhone()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    // This is the realtime render target camera
    RealTimeCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Phone Camera"));
    RealTimeCamera->SetupAttachment(Super::MeshComponent);
    RealTimeCamera->SetWorldScale3D(FVector(.15f, .15f, .15f));

    // This is the plane in which our render target camera will render the camera FOV
    PhoneScreen = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Phone Screen"));
    PhoneScreen->SetupAttachment(Super::MeshComponent);
    PhoneScreen->SetCastShadow(false);
}

// Called when the game starts or when spawned
void APhone::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void APhone::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void APhone::Use(ADarkestFearCharacter* DarkestFearCharacter)
{
}

void APhone::AlternateUse(ADarkestFearCharacter* DarkestFearCharacter)
{
    
}
