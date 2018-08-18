#ifndef HeartBeatNode_h
#define HeartBeatNode_h

#include <HomieNode.hpp>
#include <Tasker.h>
#include <UTIL.h>

class HeartBeatNode {
public:
	HeartBeatNode();
	void setup(Tasker* tasker, int TimeOutPeriod = 1000);
	bool onHeartBeat(const HomieRange& range, const String& value);
	void respondHeartBeat();

	static void onHBTimeoutCallback();
private:
	Tasker * tasker;
	int timeoutPeriod;
	HomieNode heartbeatNode;

};
HeartBeatNode HeartBeat;


HeartBeatNode::HeartBeatNode() : heartbeatNode("heartbeat", "switch") {

}

void HeartBeatNode::setup(Tasker* tasker, int TimeOutPeriod) {
	this->tasker = tasker;
	this->timeoutPeriod = TimeOutPeriod;
	heartbeatNode.advertise("value").settable([this](const HomieRange& range, const String& value) -> bool {
		return this->onHeartBeat(range, value);
	});
}

bool HeartBeatNode::onHeartBeat(const HomieRange& range, const String& value) {
	bool val = util.toBool(value);
	if (val) {
		tasker->setTimeout(onHBTimeoutCallback, timeoutPeriod);
	}
	heartbeatNode.setProperty("value").send(String(val));
	return true;
}

void HeartBeatNode::respondHeartBeat() {
	heartbeatNode.setProperty("value").overwriteSetter(true).send(String(false));
}

void HeartBeatNode::onHBTimeoutCallback() {
	HeartBeat.respondHeartBeat();
}

#endif
