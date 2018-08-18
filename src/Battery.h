#pragma once

#include <Tasker.h>

enum class BatteryManagerLevel{
	GOOD = 1,
	OKAY = 2,
	BAD = 3
};

typedef void (*ProtectionCallback)(BatteryManagerLevel level);

class BatteryManager{
	public:
		void setPin(byte pin);
		void setFactor(float factor);
		
		float getVoltage(byte count);
		float getVoltage();
		float getPercent(byte count);
		float getPercent(float voltage);
		void enableProtection(Tasker &tasker, ProtectionCallback callback);		
		
	private:
		byte pin = A0;
		float ADC_To_V = 0.000919; // ADC Counts to ADC Voltage
		float V_To_V = 5.16605166; // ADC Voltage to BatteryManager Voltage
		ProtectionCallback callback;
		
		void check();
		static void check_static();
};
extern BatteryManager Battery;
BatteryManager Battery;




void BatteryManager::setPin(byte _pin){
	pin = _pin;
}

void BatteryManager::setFactor(float factor){
	V_To_V = factor;
}

float BatteryManager::getVoltage(byte count){
	double sum = 0;
	for (byte i = 0; i < count; ++i) {
		sum += getVoltage();
	}
	return sum / (float)count;
}

float BatteryManager::getVoltage(){
	float ADCCount = analogRead(pin);
	float ADCVoltage = ADCCount * ADC_To_V;
	float BatteryVoltage = ADCVoltage * V_To_V;
	//Serial.println("Count: " + String(ADCCount) + "ADCV: " + String(ADCVoltage) + "V: " + String(BatteryVoltage));
	return BatteryVoltage;
}

float BatteryManager::getPercent(byte count){
	return getPercent(getVoltage(count));
}

float BatteryManager::getPercent(float v){
	float percent = -1.0;
	if(v >= 4.2)
		percent = 100.0;
	else if(v >= 4.1)
		percent = 95.0 + (5.0 * (v - 4.1) / 0.1);
	else if(v >= 4.0)
		percent = 85.0 + (10.0 * (v - 4.0) / 0.1);
	else if(v >= 3.9)
		percent = 75.0 + (10.0 * (v - 3.9) / 0.1);
	else if(v >= 3.8)
		percent = 65.0 + (10.0 * (v - 3.8) / 0.1);
	else if(v >= 3.7)
		percent = 55.0 + (10.0 * (v - 3.7) / 0.1);
	else if(v >= 3.6)
		percent = 25.0 + (30.0 * (v - 3.6) / 0.1);
	else if(v >= 3.5)
		percent = 15.0 + (10.0 * (v - 3.5) / 0.1);
	else if(v >= 3.4)
		percent = 10.0 + (5.0 * (v - 3.4) / 0.1);
	else if(v >= 3.3)
		percent = 5.0 + (5.0 * (v - 3.3) / 0.1);
	else if(v >= 3.2)
		percent = 2.0 + (3.0 * (v - 3.2) / 0.1);
	else if(v >= 3.1)
		percent = 1.0 + (1.0 * (v - 3.1) / 0.1);
	else if(v >= 3.0)
		percent = 0.0 + (1.0 * (v - 3.0) / 0.1);
	else
		percent = 0.0;
	return percent;
}

void BatteryManager::enableProtection(Tasker &tasker, ProtectionCallback _callback){
	callback = _callback;
	tasker.setInterval(check_static, 60 * 1000);
	check();
}

void BatteryManager::check(){
	float v = getPercent((byte)10);
	if(v >= 10.0)
		callback(BatteryManagerLevel::GOOD);
	else if(v >= 5.0 && v < 10.0)
		callback(BatteryManagerLevel::OKAY);
	else if(v < 5.0)
		callback(BatteryManagerLevel::BAD);
}


void BatteryManager::check_static(){
	Battery.check();
}