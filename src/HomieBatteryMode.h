#ifndef HomieBatteryMode_h
#define HomieBatteryMode_h

// Imports
#include <Arduino.h>
#include <Homie.h>
#include <Tasker.h>

#include <UTIL.h>
#include <Battery.h>

// Types
typedef void (*Callback)();

// Function Headers
void OnSetupDevice(Callback cb);
void OnHomieConfig(Callback cb);
void OnHomieSettings(Callback cb);
void OnHomieNodes(Callback cb);
void OnSetupTasks(Callback cb);
void OnHomieSetup(Callback cb);
void OnHomieLoop(Callback cb);
void OnRun(Callback cb);
void Setup();
void Loop();
void LoopHomie(int);
void BatSetupDevice();
void BatHomieConfig();
void BatHomieSettings();
void BatHomieNodes();
void BatOnHomieEvent(const HomieEvent& event);
void BatHomieSetup();
void BatHomieLoop();
void BatSetupTasks();
void BatRun(int);
void BatOnBatteryProtection(BatteryManagerLevel level);
void BatAutoSleep(int sleepType);
void BatHomieSafeSleep();
void BatForceSceep();

// Settings
#ifndef sleepTimeout
#define sleepTimeout 10 * 1000 // 10 sec
#endif
#ifndef sleepTimeoutForce
#define sleepTimeoutForce sleepTimeout * 2 // 20 sec
#endif
#ifndef defaultSleepTime
#define defaultSleepTime 1 * 60 * 1000 * 1000 // 1 min
#endif

// Homie Nodes
#include <HeartBeatNode.h>
#include <ConfigModeNode.h>

// Homie Config
HomieSetting<long> intervalSetting("interval", "Update Frequency (Minutes 1 - 60)");

// Global Variables
Tasker tasker;
unsigned long sleepTime = defaultSleepTime;


// Callbacks

Callback onSetupDevice;
Callback onHomieConfig;
Callback onHomieSettings;
Callback onHomieNodes;
Callback onSetupTasks;
Callback onHomieSetup;
Callback onHomieLoop;
Callback onRun;

// Setters

void OnSetupDevice(Callback cb) {
  onSetupDevice = cb;
}
void OnHomieConfig(Callback cb) {
  onHomieConfig = cb;
}
void OnHomieSettings(Callback cb) {
  onHomieSettings = cb;
}
void OnHomieNodes(Callback cb) {
  onHomieNodes = cb;
}
void OnSetupTasks(Callback cb) {
  onSetupTasks = cb;
}
void OnHomieSetup(Callback cb) {
  onHomieSetup = cb;
}
void OnHomieLoop(Callback cb) {
  onHomieLoop = cb;
}
void OnRun(Callback cb) {
  onRun = cb;
}

// Functions

void Setup() {
  Serial.begin(115200);
  Homie.getLogger() << endl << endl;
  Homie.getLogger() << S("ESP Homie Device Starting Up") << endl;

  // Setup Device
  BatSetupDevice();
  // Setup Homie
  BatHomieConfig();
  // Battery Protection
  Battery.enableProtection(tasker, BatOnBatteryProtection);
  // Tasks
  BatSetupTasks();

  Homie.getLogger() << S("ESP Homie Device Started") << endl;
}

void Loop() {
  tasker.loop();
}

void LoopHomie(int) {
  Homie.loop();
}

void BatSetupDevice() {
  if (onSetupDevice != NULL)
    onSetupDevice();
}

void BatHomieConfig() {
  if (onHomieConfig != NULL)
    onHomieConfig();
  BatHomieSettings();
  BatHomieNodes();
  Homie.onEvent(BatOnHomieEvent);
  Homie.setSetupFunction(BatHomieSetup);
  Homie.setLoopFunction(BatHomieLoop);

  Homie.setup();
}

void BatHomieSettings() {
  intervalSetting.setDefaultValue(10).setValidator([] (long candidate) {
    return (candidate >= 1) && (candidate <= 60);
  });

  if (onHomieSettings != NULL)
    onHomieSettings();
}

void BatHomieNodes() {
  HeartBeat.setup(&tasker);
  ConfigMode.setup(BatHomieSafeSleep);

  if (onHomieNodes != NULL)
    onHomieNodes();
}

void BatOnHomieEvent(const HomieEvent& event) {
  switch (event.type) {
    case HomieEventType::READY_TO_SLEEP:
      BatForceSceep();
      break;
  }
}

void BatHomieSetup() {
  // Run Program in 1 sec
  tasker.setTimeout(BatRun, 1000);
  
  if (onHomieSetup != NULL)
    onHomieSetup();
}

void BatHomieLoop() {
  if (onHomieLoop != NULL)
    onHomieLoop();
}

void BatSetupTasks() {
  // Homie Loop Task
  tasker.setInterval(LoopHomie, 10);
  // Timeout incase device never connects, sleep nicely
  tasker.setTimeout(BatAutoSleep, sleepTimeout);
  // Timeout if nice sleep fails, force sleep.
  tasker.setTimeout(BatAutoSleep, sleepTimeoutForce, 1, 0);

  if (onSetupTasks != NULL)
    onSetupTasks();
}

void BatRun(int) {
  // Run Function Once
  if (onRun != NULL)
    onRun();
  // Auto Sleep in 1 sec
  tasker.setTimeout(BatAutoSleep, 1000);
}

void onBatteryProtection(BatteryManagerLevel level) {
  // time in min
  sleepTime = 0;
  switch (level) {
    case BatteryManagerLevel::GOOD:
      sleepTime = intervalSetting.get();
      break;
    case BatteryManagerLevel::OKAY:
      sleepTime = 10;
      break;
    case BatteryManagerLevel::BAD:
      sleepTime = 30;
      break;
  }
  // min to micro secconds
  sleepTime *= 60 * 1000 * 1000;
}


void BatAutoSleep(int sleepType) {
  if (sleepType == 0 && !ConfigMode.status && Homie.isConfigured())
    BatHomieSafeSleep();
  else if (sleepType == 1 && !ConfigMode.status && Homie.isConfigured())
    BatForceSceep();
}

void BatHomieSafeSleep() {
  Homie.prepareToSleep();
}

void BatForceSceep() {
  Serial.println(C("Sleep"));
  Serial.println();
  delay(500);
  // If some hpw sleepTime is 0 then set it to one min else 0 = forever ...
  if (sleepTime == 0) sleepTime = defaultSleepTime;
  ESP.deepSleep(sleepTime);
}

#endif