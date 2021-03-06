/*
 * Integer.cpp
 *
 *  Created on: Mar 21, 2014
 *      Author: Brett
 */

#include "Integer.h"
#include <typeinfo>
#include <sstream>
#include <vector>
#include "Number.h"
#include "Exponent.h"
#include "Placeholder.h"
#include "Log.h"
#include "Root.h"
#include "Variable.h"

Integer::Integer(int toContain) {
	this->intContainer = toContain;
}

Integer::~Integer() {
	// TODO Auto-generated destructor stub
}

Number& Integer::operator+(Number& rhs) {
	std::stringstream ss;

	if (Integer * rhsCast = dynamic_cast<Integer*>(&rhs)) {
		int answer = this->intContainer + rhsCast->intContainer;
		delete rhsCast;

		Number * integer = new Integer(answer);
		return *integer;
	} else {
		delete rhsCast;

		if (typeid(rhs) == typeid(Placeholder)) {
			return rhs + *this;
		}

		else {
			std::vector<Number*> numbers;
			std::vector<char> operators;
			numbers.push_back(this);
			numbers.push_back(&rhs);
			operators.push_back('+');
			Number * placeholder = new Placeholder(numbers, operators);
			return *placeholder;
		}
	}
}

Number& Integer::operator-(Number& rhs) {
	std::stringstream ss;

	if (Integer * intCast = dynamic_cast<Integer*>(&rhs)) {
		int answer = this->intContainer - intCast->intContainer;
		delete intCast;

		Number * integer = new Integer(answer);
		return *integer;
	} else {
		delete intCast;

		if (typeid(rhs) == typeid(Placeholder)) {
			return rhs - *this;
		}

		else {
			std::vector<Number*> numbers;
			std::vector<char> operators;
			numbers.push_back(this);
			numbers.push_back(&rhs);
			operators.push_back('-');
			Number * placeholder = new Placeholder(numbers, operators);
			return *placeholder;
		}
	}
}

Number& Integer::operator*(Number& rhs) {
	if (Integer * rhsCast = dynamic_cast<Integer*>(&rhs)) {
		int answer = this->intContainer * rhsCast->intContainer;
		delete rhsCast;

		Number * integer = new Integer(answer);
		return *integer;
	} else {
		delete rhsCast;

		if (typeid(rhs) == typeid(Placeholder)) {
			return rhs * *this;
		}

		else {
			std::vector<Number*> numbers;
			std::vector<char> operators;
			numbers.push_back(this);
			numbers.push_back(&rhs);
			operators.push_back('*');
			Number * placeholder = new Placeholder(numbers, operators);
			return *placeholder;
		}
	}
}

Number& Integer::operator/(Number& rhs) {
	if (Integer * rhsCast = dynamic_cast<Integer*>(&rhs)) {
		if (this->intContainer % rhsCast->intContainer == 0) {
			int answer = this->intContainer / rhsCast->intContainer;
			delete rhsCast;

			Number * integer = new Integer(answer);
			return *integer;
		} else {
			int gcdResult = gcd(this->intContainer, rhsCast->intContainer);
			int numerator = this->intContainer / gcdResult;
			int denominator = rhsCast->intContainer / gcdResult;
			Number * intNum = new Integer(numerator);
			Number * intDenom = new Integer(denominator);

			std::vector<Number*> numbers;
			std::vector<char> operators;
			numbers.push_back(intNum);
			numbers.push_back(intDenom);
			operators.push_back('/');
			Number * placeholder = new Placeholder(numbers, operators);
			return *placeholder;
		}
	} else {
		delete rhsCast;

		if (typeid(rhs) == typeid(Placeholder)) {
			Placeholder * placeholder = dynamic_cast<Placeholder*>(&rhs);
			bool canSimplify = true;
			for (int i = 0; i < placeholder->getOperators().capacity(); i++) {
				if (placeholder->getOperators()[i] == '+' || placeholder->getOperators()[i] == '-') {
					canSimplify = false;
				}
			}

			if (canSimplify) {
				for (int i = 0; i < placeholder->getNumbers().capacity(); i++) {
					if (typeid(placeholder->getNumbers()[i]) == typeid(Integer) && ((i != 0 && placeholder->getOperators()[i-1] == '*') || (i == 0 && placeholder->getOperators()[0] == '*'))) {
						Number * intAnswer = &(*this / *placeholder->getNumbers()[i]);

						if (typeid(intAnswer) == typeid(Integer)) {
							placeholder->getOperators().erase(placeholder->getOperators().begin() + (i-1));
							placeholder->getNumbers().erase(placeholder->getNumbers().begin() + i);
							placeholder->getNumbers().insert(placeholder->getNumbers().begin(), intAnswer);
							placeholder->getOperators().insert(placeholder->getOperators().begin(), '/');
							return *placeholder;
						}

						else {
							Placeholder * tempPlaceholder = dynamic_cast<Placeholder*>(intAnswer);
							placeholder->getNumbers()[i] = tempPlaceholder->getNumbers()[1];
							tempPlaceholder->getNumbers()[1] = placeholder;
							return *tempPlaceholder;
						}
					}

					else if (typeid(placeholder->getNumbers()[i]) == typeid(Integer) && ((i != 0 && placeholder->getOperators()[i-1] == '/') || (i == 0 && placeholder->getOperators()[0] == '/'))) {
						Number * answer = &(*this * *placeholder->getNumbers()[i]);
						return *answer;
					}
				}
			}
		}

		else {
			std::vector<Number*> numbers;
			std::vector<char> operators;
			numbers.push_back(this);
			numbers.push_back(&rhs);
			operators.push_back('/');
			Number * placeholder = new Placeholder(numbers, operators);
			return *placeholder;
		}
	}
}

// **NOTICE**: In order to avoid circular referencing, this method must be copy and pasted as opposed to
// being inherited from the Number class.
bool Integer::operator==(Number& rhs) {
	if (typeid(this) == typeid(rhs)) {
		if (typeid(rhs) == typeid(Exponent)) {
			Exponent * rhsCast = dynamic_cast<Exponent*>(&rhs);
			Exponent * lhsCast = dynamic_cast<Exponent*>(this);
			if (rhsCast->getBase() == lhsCast->getBase() && rhsCast->getExponent() == lhsCast->getExponent()) {
				return true;
			} else {
				return false;
			}
		} else if (typeid(rhs) == typeid(Log)) {
			Log * rhsCast = dynamic_cast<Log*>(&rhs);
			Log * lhsCast = dynamic_cast<Log*>(this);
			if (rhsCast->getBase() == lhsCast->getBase() && rhsCast->getArgument() == lhsCast->getArgument()) {
				return true;
			} else {
				return false;
			}
		} else if (typeid(rhs) == typeid(Placeholder)) {
			Placeholder * rhsCast = dynamic_cast<Placeholder*>(&rhs);
			Placeholder * lhsCast = dynamic_cast<Placeholder*>(this);

			if (rhsCast->getNumbers().capacity() == lhsCast->getNumbers().capacity()) {
				for (int i = 0; i < lhsCast->getNumbers().capacity(); i++) {
					bool matched = false;

					for (int y = 0; y < rhsCast->getNumbers().capacity(); y++) {
						if (lhsCast[i] == rhsCast[y]) {
							matched = true;
						}
					}

					if (!matched) {
						return false;
					}
				}

				for (int i = 0; i < lhsCast->getOperators().capacity(); i++) {
					bool matched = false;

					for (int y = 0; y < rhsCast->getOperators().capacity(); y++) {
						if (lhsCast->getOperators()[i] == rhsCast->getOperators()[y]) {
							if (lhsCast->getOperators()[i] == '-' || lhsCast->getOperators()[i] == '/') {
								if (lhsCast->getNumbers()[i] == rhsCast->getNumbers()[y] && lhsCast->getNumbers()[i+1] == rhsCast->getNumbers()[y+1]) {
									matched = true;
								} else {
									matched = false;
								}
							} else {
								if (lhsCast->getNumbers()[i] == rhsCast->getNumbers()[y] && lhsCast->getNumbers()[i+1] == rhsCast->getNumbers()[y+1]) {
									matched = true;
								} else if (lhsCast->getNumbers()[i+1] == rhsCast->getNumbers()[y] && lhsCast->getNumbers()[i] == rhsCast->getNumbers()[y+1]) {
									matched = true;
								} else {
									matched = false;
								}
							}
						}
					}

					if (!matched) {
						return false;
					}
				}

				return true;
			} else {
				return false;
			}
		} else if (typeid(rhs) == typeid(Root)) {
			Root * rhsCast = dynamic_cast<Root*>(&rhs);
			Root * lhsCast = dynamic_cast<Root*>(this);
			if (rhsCast->getBase() == lhsCast->getBase() && rhsCast->getRoot() == lhsCast->getRoot()) {
				return true;
			} else {
				return false;
			}
		} else if (typeid(rhs) == typeid(Integer)) {
			Integer * rhsCast = dynamic_cast<Integer*>(&rhs);
			Integer * lhsCast = dynamic_cast<Integer*>(this);

			if (rhsCast->getInt() == lhsCast->getInt()) {
				return true;
			} else {
				return false;
			}
		} else if (typeid(rhs) == typeid(Variable)) {
			Variable * rhsCast = dynamic_cast<Variable*>(&rhs);
			Variable * lhsCast = dynamic_cast<Variable*>(this);

			if (lhsCast->getVariable() == rhsCast->getVariable()) {
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	} else {
		return false;
	}
}

std::string Integer::toString() {
	std::stringstream ss;
	ss << this->intContainer;

	return ss.str();
}

std::vector<std::pair<Number*, char> > Integer::simplify() {
	std::vector<std::pair<Number*, char> > vectorPair;
	Number * num = this;
	vectorPair.push_back(std::make_pair<Number*, char>(num, NULL));

	return vectorPair;
}

int Integer::getInt() {
	return intContainer;
}
