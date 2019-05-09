// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUD.h"
#include <UObject/ConstructorHelpers.h>
#include <Blueprint/UserWidget.h>

AGameHUD::AGameHUD()
{
    static ConstructorHelpers::FClassFinder<UUserWidget> HUDObj(TEXT("/Game/UI/GameHUD"));
    HUDWidgetClass = HUDObj.Class;
}

void AGameHUD::DrawHUD()
{
    Super::DrawHUD();
}

void AGameHUD::BeginPlay()
{
    Super::BeginPlay();
    if (HUDWidgetClass) {
        CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

        if (CurrentWidget) {
            CurrentWidget->AddToViewport();
        }
    }
}
