class Digital_Module {
public:
	Digital_Module();
	Digital_Module(byte _pin);
	Digital_Module(byte _pin, bool isInput);
	Digital_Module(byte _pin, bool isInput, bool _invert);
	Digital_Module(byte _pin, bool isInput, bool _invert, bool InternalPullup);

	void Setup(byte _pin);
	void Setup(byte _pin, bool isInput);
	void Setup(byte _pin, bool isInput, bool _invert);
	void Setup(byte _pin, bool isInput, bool _invert, bool InternalPullup);

	bool getState();
	void setState(bool val);
	void PWM(int val);
	void PWMPercent(byte val);


	static void SetPWMMax(int val);
private:
	bool invert;
	byte pin;
	
	static int PWMMaxVal;
};

int Digital_Module::PWMMaxVal = 255;

void Digital_Module::SetPWMMax(int val){
	PWMMaxVal = val;
}

Digital_Module::Digital_Module() {
}

Digital_Module::Digital_Module(byte _pin) {
	Setup(_pin, false, false, false);
}

Digital_Module::Digital_Module(byte _pin, bool isInput) {
	Setup(_pin, isInput, false, false);
}

Digital_Module::Digital_Module(byte _pin, bool isInput, bool _invert) {
	Setup(_pin, isInput, _invert, false);
}

Digital_Module::Digital_Module(byte _pin, bool isInput, bool _invert, bool InternalPullup) {
	Setup(_pin, isInput, _invert, InternalPullup);
}

void Digital_Module::Setup(byte _pin) {
	Setup(_pin, false, false, false);
}

void Digital_Module::Setup(byte _pin, bool isInput) {
	Setup(_pin, isInput, false, false);
}

void Digital_Module::Setup(byte _pin, bool isInput, bool _invert) {
	Setup(_pin, isInput, _invert, false);
}

void Digital_Module::Setup(byte _pin, bool isInput, bool _invert, bool InternalPullup) {
	pin = _pin;
	invert = _invert;
	if (isInput) {
		if (!InternalPullup) pinMode(pin, INPUT);
		else pinMode(pin, INPUT_PULLUP);
	}
	else {
		pinMode(pin, OUTPUT);
		digitalWrite(pin, LOW);
	}
}

bool Digital_Module::getState() {
	bool state = digitalRead(pin);
	return invert ? !state : state;
}

void Digital_Module::setState(bool val) {
	val = invert ? !val : val;
	digitalWrite(pin, val);
}
void Digital_Module::PWM(int val) {
	analogWrite(pin, val);
}

void Digital_Module::PWMPercent(byte val) {
	val = constrain(val, 0, 100);
	int pwmval = map(val, 0, 100, 0, Digital_Module::PWMMaxVal);
	PWM(pwmval);
}


