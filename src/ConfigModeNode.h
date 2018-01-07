#ifndef ConfigModeNode_h
#define ConfigModeNode_h

#include <HomieNode.hpp>

typedef void (*SleepCallback)();

class ConfigModeNode{
	public:
		bool status;
				
		ConfigModeNode();
		void setup(SleepCallback sleepCallback);
		bool onConfigMode(const HomieRange& range, const String& value);
		
		static bool onConfigModeHomieCallback(const HomieRange& range, const String& value);

	private:
		HomieNode configModeNode;
		SleepCallback callback;

};
extern ConfigModeNode ConfigMode;
ConfigModeNode ConfigMode;



ConfigModeNode::ConfigModeNode():
configModeNode("config", "switch")
{
	status = false;
}

void ConfigModeNode::setup(SleepCallback sleepCallback){
	this->callback = sleepCallback;
	configModeNode.advertise("value").settable(onConfigModeHomieCallback);
}

bool ConfigModeNode::onConfigMode(const HomieRange& range, const String& value) {
	bool val = util.toBool(value);
	if (val) {
		status = true;
	}
	else if (!val && status) {
		callback();
	}
	Serial.println("Config Mode Set to: " + String(val));
	configModeNode.setProperty("value").send(String(val));
	return true;
}

bool ConfigModeNode::onConfigModeHomieCallback(const HomieRange& range, const String& value) {
	return ConfigMode.onConfigMode(range, value);
}

#endif