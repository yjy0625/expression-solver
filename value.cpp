/*

value.cpp

Author: Jingyun Yang
Date Created: 10/2/16

Description: Implementation of Value Class.

*/

#include "value.h"

Value::Value() 
	: isDecimal(false), fracValue(Fraction()), decValue(0.0), calculability(false) {}

Value::Value(Fraction fv) {
	if(fv.down == 0) {
		cerr << "Arithmatic error: Denominator is zero! ";
		Value();
		return;
	}
	isDecimal = false;
	fracValue = fv;
	decValue = (double)fv.up / fv.down;
	calculability = true;
}

Value::Value(double dv) {
	Value newValue = Value(to_string(dv));
	*this = newValue;
}

Value::Value(string str) {
	Value newValue;
	int found = str.find('.');
	if(found != string::npos) {
		string left = str.substr(0,found);
		string right = str.substr(found + 1);
		if(left.length() >= 10) {
			cerr << "Arithmatic error: Number too large! ";
			*this = Value();
			return;
		}
		while(right.length() > 0 && right[right.length()-1] == '0') {
			right = right.substr(0,right.length()-1);
		}
		if(right.length() == 0) {
			newValue = Value(Fraction(stoi(left), 1));
			*this = newValue;
			return;
		}
		else if(right.length() <= 5){
			if(right.find('.') != string::npos) {
				cerr << "Arithmatic Error: More than one '.' in a number! ";
				Value();
				return;
			}
			int leftNumber = stoi(left), rightNumber = stoi(right);
			int multiplier = pow(10.0,(floor)(log10(rightNumber)) + 1.0);
			newValue = Value(Fraction(leftNumber*multiplier+rightNumber,multiplier));
			*this = newValue;
			return;
		}
		else {
			isDecimal = true;
			fracValue = Fraction();
			decValue = stod(str);
			calculability = true;
			return;
		}
	}
	newValue = Value(Fraction(stoi(str),1));
	*this = newValue;
	return;
}

bool Value::getDecimal() const {
	return isDecimal;
}

Fraction Value::getFracValue() const {
	return fracValue;
}

double Value::getDecValue() const {
	return decValue;
}

bool Value::getCalculability() const {
	return calculability;
}

string Value::printValue() const {
	if(!calculability) return "";
	if(isDecimal) {
		return to_string(decValue);
	}
	else if(fracValue.down == 1 || fracValue.up == 0) {
		return to_string(fracValue.up);
	}
	else {
		return to_string(fracValue.up) + "/" + to_string(fracValue.down);
	}
}

Value& Value::operator+=(const Value z) {
	if(!calculability || !z.getCalculability()) {
		*this = Value();
		return *this;
	}
	if(isDecimal || z.getDecimal()) {
		*this = Value(decValue+z.getDecValue());
	}
	else {
		*this = Value(Fraction(fracValue.up*z.getFracValue().down
			+fracValue.down*z.getFracValue().up
			,z.getFracValue().down*fracValue.down));
	}
	return *this;
}

Value& Value::operator-=(const Value z) { 
	if(!calculability || !z.getCalculability()) {
		*this = Value();
		return *this;
	}
	if(isDecimal || z.getDecimal()) {
		*this = Value(decValue-z.getDecValue());
	}
	else {
		*this = Value(Fraction(fracValue.up*z.getFracValue().down
			-fracValue.down*z.getFracValue().up
			,z.getFracValue().down*fracValue.down));
	}
	return *this;
}

Value& Value::operator*=(const Value z) { 
	if(!calculability || !z.getCalculability()) {
		*this = Value();
		return *this;
	}
	if(isDecimal || z.getDecimal()) {
		*this = Value(decValue*z.getDecValue());
	}
	else {
		*this = Value(Fraction(z.getFracValue().up*fracValue.up
			,z.getFracValue().down*fracValue.down));
	}
	return *this;
}

Value& Value::operator/=(const Value z) {
	if(!calculability || !z.getCalculability()) {
		*this = Value();
		return *this;
	}
	if(isDecimal || z.getDecimal()) {
		*this = Value(decValue/z.getDecValue());
	}
	else {
		*this = Value(Fraction(z.getFracValue().down*fracValue.up
			,z.getFracValue().up*fracValue.down));
	}
	return *this;
}

Value& Value::powv(const Value z) {
	if(!calculability || !z.getCalculability()) {
		*this = Value();
		return *this;
	}
	if(decValue < 0 && z.getDecValue() != (double)(int(z.getDecValue()))) {
		cerr << "Arithmatic error: Can't power a negative number by a non-integer! ";
		*this = Value();
		return *this;
	}
	if(!isDecimal && z.getDecValue() == (double)(int(z.getDecValue()))) {
		fracValue.up = pow(fracValue.up,fabs(z.getDecValue()));
		fracValue.down = pow(fracValue.down,fabs(z.getDecValue()));
        if(z.getDecValue() < 0) {
            swap(fracValue.up,fracValue.down);
            if(fracValue.down < 0) {
                fracValue.up *= -1;
                fracValue.down *= -1;
            }
        }
	}
	else {
		*this = Value(pow(decValue,z.getDecValue()));
	}
	return *this;
}