// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicPC.h"
#include "BasicPCM.h"

ABasicPC::ABasicPC()
{
	PlayerCameraManagerClass = ABasicPCM::StaticClass();
}
