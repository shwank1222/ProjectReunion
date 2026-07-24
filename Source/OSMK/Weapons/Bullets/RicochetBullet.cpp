// Fill out your copyright notice in the Description page of Project Settings.


#include "RicochetBullet.h"

#include "GameFramework/ProjectileMovementComponent.h"

ARicochetBullet::ARicochetBullet()
{
	ProjectileMovement->bShouldBounce = true;
}

