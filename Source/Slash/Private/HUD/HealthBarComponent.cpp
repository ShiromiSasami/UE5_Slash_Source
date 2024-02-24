// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"

#include "Components/ProgressBar.h"
#include "HUD/HealthBar.h"

void UHealthBarComponent::SetHealthPercent(float Percent)
{
	if (!HealthBarWidght)
	{
		HealthBarWidght = Cast<UHealthBar>(GetUserWidgetObject());
	}
	
	if (HealthBarWidght && HealthBarWidght->HealthBar)
	{
		HealthBarWidght->HealthBar->SetPercent(Percent);
	}
}
