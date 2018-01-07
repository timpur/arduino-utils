#ifndef UTIL_h
#define UTIL_h

// Defines
#define S(string_literal) String(F(string_literal))
#define C(string_literal) S(string_literal).c_str()

class UTIL {
	public:
		unsigned long HashCode(const String& val);
		double RoundDP(const double val, const byte dp);
		double RountToNearest(const double val, const float base = 0.01);
		bool toBool(const String& val);
	private:
		
};
UTIL util;


unsigned long UTIL::HashCode(const String& val) {
	byte shift = 0;
	unsigned long hash = 0;
	char c;
	for (int i = 0; i < val.length(); ++i) {
		c = val[i];
		hash = hash ^ c;
		shift = (hash & 31);
		shift = constrain(shift, 1, 31);
		hash = (hash << shift) | (hash >> (32 - shift));
	}
	return hash;
}

double UTIL::RoundDP(const double val, const byte dp) {
	double temp = pow(10.0, dp);
	double rval = round(val * temp) / temp;
	return rval;
}

double UTIL::RountToNearest(const double val, const float base) {
	if(base != 0.0)
		return RoundDP((base * round(val / base)), 2);
	else 
		return 0.0;
}

bool UTIL::toBool(const String& val){
	return val.toInt() >= 1 ? true : false;
}

#endif