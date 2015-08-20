/*
 * UltrasonicDisplayOnTerm.ino
 * Example sketch for ultrasonic ranger
 *
 * Copyright (c) 2012 seeed technology inc.
 * Website    : www.seeed.cc
 * Author     : LG, FrankieChu
 * Create Time: Jan 17,2013
 * Change Log :

/***************************************************************************/
//	Function: Measure the distance to obstacles in front and print the distance
//			  value to the serial terminal.The measured distance is from
//			  the range 0 to 400cm(157 inches).
//	Hardware: Grove - Ultrasonic Ranger
//	Arduino IDE: Arduino-1.0
/*****************************************************************************/
#include <Console.h>
#include "Ultrasonic.h"

Ultrasonic ultrasonic(7);

void setup()
{
	Bridge.begin();
	Console.begin();

	// Wait for Console port to connect
    while (!Console); 
}
void loop()
{
	long RangeInInches;
	long RangeInCentimeters;
	RangeInInches = ultrasonic.MeasureInInches();
	RangeInCentimeters = ultrasonic.MeasureInCentimeters();
	Console.println("The distance to obstacles in front is: ");
	Console.print(RangeInInches);//0~157 inches
	Console.println(" inch");
	Console.print(RangeInCentimeters);//0~400cm
	Console.println(" cm");
	delay(100);
}
