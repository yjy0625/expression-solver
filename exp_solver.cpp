/*

exp_solver.cpp

Author: Jingyun Yang
Date First Created: 1/16/15
Date Last Modified: 10/2/16

Description: Implementation file for ExpSolver 
class that takes in algebraic expressions as 
strings and solves them.

*/

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <iomanip>
#include "exp_solver.h"

using namespace std;

// ******************** //
// * Public Functions * //
// ******************** //

// Constructor
ExpSolver::ExpSolver() {
	addPredefined();
}

// Solves expression of input string
string ExpSolver::solveExp(string exp) {
	// Discard all spaces in the expression
	exp = discardSpaces(exp);
	
	// Find out if the expression includes variable declaration
	bool isDeclaration = false;
	string newVarName = "";
	bool declarationValid = checkDeclaration(exp, newVarName, isDeclaration);
	if(!declarationValid) {
		string output = "Calculation aborted. ";
		return output;
	}
	
	// Group the expression into substrings
	// and calculate the bracket level of each substring
	groupExp(exp);

	// Recursively solve the expression
	Value result = calculateExp(exp, 0, blocks.size());
	
	// Create output string
	string output;
	if(result.getCalculability()) {
		// Case: expression includes variable declaration
		if(isDeclaration) {
			bool variableCanBeDeclared = true;
			
			// Check if there is a constant with the same name
			for(int i = 0; i < constants.size(); i++) {
				// If found, then notice name conflict and terminate
				if(newVarName.compare(constants[i].name) == 0) {
					variableCanBeDeclared = false;
					cerr << "Constant \"" << newVarName << "\" cannot be declared! ";
					output = "Calculation aborted. ";
					return output;
				}
			}
			
			bool variableDeclaredBefore = false;
			
			// Find if the variable is already declared
			for(int i = 0; i < variables.size(); i++) {
				// If found, redeclare it
				if(newVarName.compare(variables[i].name) == 0) {
					variableDeclaredBefore = true;
					variables[i] = Variable(newVarName, result);
				}
			}
			// If not, push it into the variable stack
			if(!variableDeclaredBefore) {
				variables.push_back(Variable(newVarName, result));
			}
			cout << newVarName << " = " << result.printValue();
		}
		// Case: expression calculation only
		else {
			// Record answer for this calculation to support "ans" feature
			constants[constants.size()-1] = Variable("ans", result);
			
			// Create output string
			output = "Ans = " + result.printValue();
		}
	}
	else {
		output = "Calculation aborted. ";
	}
	
	// Clean up
	blocks.clear();
	
	return output;
}

// ********************* //
// * Private Functions * //
// ********************* //

// Add predefined constants and functions
void ExpSolver::addPredefined() {
	constants.push_back(Variable("e",Value(M_E)));
	constants.push_back(Variable("pi",Value(M_PI)));
	constants.push_back(Variable("ans",Value()));
	functions.push_back(Function("sin",sin));
	functions.push_back(Function("cos",cos));
	functions.push_back(Function("tan",tan));
	functions.push_back(Function("exp",exp));
	functions.push_back(Function("sqrt",sqrt));
	functions.push_back(Function("floor",floor));
	functions.push_back(Function("ln",log));
	functions.push_back(Function("log",log10));
}

// Discard spaces in the input string
string ExpSolver::discardSpaces(string str) {
	string newStr = "";
	for(int i = 0; i < str.length(); i++) {
		if(!isspace(str[i])) newStr += str[i];
	}
	return newStr;
}

// Check whether the input includes variable declaration
// If so, extract the lhs and rhs of the equal sign
bool ExpSolver::checkDeclaration(string &exp, string &newVarName, bool &isDec) {
	// Find '='
	int found = exp.find('=');
	
	// If '=' found...
	if(found != string::npos) {
		newVarName = exp.substr(0,found);
		isDec = true;
		exp = exp.substr(found + 1);
		
		// Check if more than one '=' exists
		if(exp.find('=') != string::npos) {
			cerr << "Syntax error: Too many '='! ";
			return false;
		}
		
		// Check variable name validity
		if(newVarName.length() == 0 || !isalpha(newVarName[0])) {
			cerr << "Variable name invalid! ";
			return false;
		}
		for(int i = 1; i < newVarName.length(); i++) {
			if(!(isalnum(newVarName[i]) || newVarName[i] == '_')) {
				cerr << "Variable name invalid! ";
				return false;
			}
		}
	}
	
	return true;
}

// This is similar to lexical analysis in a compiler
// Partition an expression into blocks of different types
void ExpSolver::groupExp(string exp) {
	// Initialize a new block that is expected to be pushed 
	// into the stack
	Block newBlock;
	
	// Record the start of next push and current level
	int start = 0, level = 0;
	
	// Record type of the last character
	BlockType currentType = Nil;
	
	
	for(int i = 0; i <= exp.length(); i++) {
		// Record type of the just inspected character
		BlockType thisType = charType(exp[i]);
		
		// Check if new block needs to be pushed into stack
		bool needNewBlock = false;
		needNewBlock |= (currentType == BracL);
		needNewBlock |= (currentType == BracR);
		needNewBlock |= (currentType == Sym);
		needNewBlock |= (thisType != currentType);
		needNewBlock |= (i == exp.length());
		needNewBlock &= !(thisType == Num && currentType == Func);
		
		if(needNewBlock) {
			// Label string as function, constant or variable
			if(currentType == Func) {
				currentType = analyzeStrType(exp.substr(start, i-start));
			}
			
			// Push new block into stack
			// if it's not the case where there is '(' at start
			if(i != 0) {
				newBlock = Block(start, i, level, currentType);
				blocks.push_back(newBlock);
			}
			
			// Lower level after right brackets are pushed into stack
			if(currentType == BracR) level--;
			
			// Update currentType
			currentType = thisType;
			start = i;
		}
		
		// Upper level before left brackets are pushed into stack
		if(thisType == BracL) level++;
	}
	
	// Throw error if brackets are not paired
	// (diagonized by inspecting variable level)
	if(level != 0) cerr << "Syntax error: Brackets not paired! ";
}

// Analyze whether a string Block is of BlockType Func, Constant or Var
BlockType ExpSolver::analyzeStrType(string str) {
	for(int i = 0; i < functions.size(); i++) {
		if(str.compare(functions[i].name) == 0) return Func;
	}
	for(int i = 0; i < constants.size(); i++) {
		if(str.compare(constants[i].name) == 0) return Constant;
	}
	for(int i = 0; i < variables.size(); i++) {
		if(str.compare(variables[i].name) == 0) return Var;
	}
	cerr << "Syntax error: String \"" + str + "\" not recognized! ";
	return Nil;
}

// Determine the type of one single character
BlockType ExpSolver::charType(char c) {
	if(c == '_' || isalpha(c)) return Func;
	else if(c == '.' || isdigit(c)) return Num;
	else if(c == '(') return BracL;
	else if(c == ')') return BracR;
	else if(c == '+' || c == '-' || c == '*' || c == '/' || c == '^') return Sym;
	else return Nil;
}

// Calculate expression in block range [startBlock,endBlock)
Value ExpSolver::calculateExp(string exp, int startBlock, int endBlock) {
	// Output error and return if expression contains nothing
	if(exp.substr(blocks[startBlock].start,
		blocks[endBlock-1].end-blocks[startBlock].start) == "") {
		cerr << "Syntax error: Invalid expression! ";
		return Value();
	}
	
	// 1st place that deals with negative signs
	bool negativeSignBeforeNumber = false;
	if(exp[blocks[startBlock].start] == '-') {
		if(endBlock <= startBlock+1) {
			cerr << "Syntax error: Invalid expression! ";
			return Value();
		}
		else if(blocks[startBlock+1].type == BracL) {
			return Value(-1)*calculateExp(exp,startBlock+1, endBlock);
		}
		else if(blocks[startBlock+1].type == Num) {
			// Leaves the negative sign before number case to later
			negativeSignBeforeNumber = true;
		}
		else {
			cerr << "Syntax error: Invalid expression! ";
			return Value();
		}
	}
	
	// Create stacks that stores operands and operators
	stack<Value> values;
	stack<char> ops;
	
	for(int i = endBlock-1; i >= startBlock; i--) {
		// Record the content of the current block
		string blockStr = exp.substr(blocks[i].start,blocks[i].end-blocks[i].start);
		
		// Variable to prepare for skipping items inside the for loop
		// When this method is recursively called to calculate in-bracket contents
		// this variable is modified
		int iIncrement = 0;
		
		// Push numbers to stack
		if(blocks[i].type == Num) {
			// 2nd place that deals with negative signs
			// Resolve the negative sign before number issue
			if(negativeSignBeforeNumber && i == startBlock+1) {
				values.push(Value(blockStr));
				ops.push('-');
				values.push(Value(0));
				break;
			}
			else {
				values.push(Value(blockStr));
			}
		}
		
		// Throw error if function names occur without brackets
		else if(blocks[i].type == Func) {
			cerr << "Syntax Error: Need brackets after function name! ";
			return Value();
		}
		
		// Replace constants with Value
		else if(blocks[i].type == Constant) {
			Value constValue;
			for(int i = 0; i < constants.size(); i++) {
				if(blockStr.compare(constants[i].name) == 0) {
					constValue = constants[i].value;
					
					// If constant doesn't have a value
					// that means that 'ans' is not defined
					if(!constValue.getCalculability()) {
						cerr << "Bad access: \"ans\" not defined currently! ";
						return Value();
					}
					
					break;
				}
			}
			values.push(constValue);
		}
		
		// Replace values with Value
		else if(blocks[i].type == Var) {
			Value varValue;
			for(int i = 0; i < variables.size(); i++) {
				if(blockStr.compare(variables[i].name) == 0) {
					varValue = variables[i].value;
					break;
				}
			}
			values.push(varValue);
		}
		
		// Recursively solve expression inside brackets
		// and modify iIncrement to skip in-bracket loop items
		else if(blocks[i].type == BracR) {
			// Corresponding Block ID
			int corBlock = findIndexOfBracketEnding(i);
			if(corBlock != 0 && blocks[corBlock-1].type == Func) {
				// Find the function and calculate the result of the function.
				double (*funcToUse)(double);
				for(int i = 0; i < functions.size(); i++) {
					string contentToCompare = 
						exp.substr(blocks[corBlock-1].start, blocks[corBlock-1].end-blocks[corBlock-1].start);
					if(contentToCompare.compare(functions[i].name) == 0) {
						funcToUse = functions[i].func;
						break;
					}
				}
				double funcResult = (*funcToUse)(calculateExp(exp, corBlock+1, i).getDecValue());
				
				// Convert to Value and push to stack.
				Value newValue = Value(funcResult);
				values.push(newValue);
				
				iIncrement -= i - corBlock + 1;
			}
			else {
				values.push(calculateExp(exp, corBlock+1, i));
				iIncrement -= i - corBlock;
			}
		}
		
		// Push symbols into stack and calculate
		// from highest calculation priority down
		else if(blocks[i].type == Sym) {
			// '*' & '/' 
			// Here '^' can be calculated
			if(blockStr[0] == '*' || blockStr[0] == '/') {
				if(values.size() != ops.size()+1) {
					cerr << "Syntax error: Expression invalid! ";
					return Value();
				}
				while(!ops.empty()) {
					char lastOp = ops.top();
					if(lastOp == '^') {
						ops.pop();
						Value op1 = values.top(); values.pop();
						Value op2 = values.top(); values.pop();
						values.push(powv(op1,op2));
					}
					else {
						break;
					}
				}
			}
			
			// '+' & '-'
			// Here '^' '*' '/' can be calculated
			else if(blockStr[0] == '+' || blockStr[0] == '-') {
				if(values.size() != ops.size()+1) {
					cerr << "Syntax error: Expression invalid! ";
					return Value();
				}
				while(!ops.empty()) {
					char lastOp = ops.top();
					if(!(lastOp == '+' || lastOp == '-')) {
						ops.pop();
						Value op1 = values.top(); values.pop();
						Value op2 = values.top(); values.pop();
						if(lastOp == '*') values.push(op1*op2);
						else if(lastOp == '/') values.push(op1/op2);
						else if(lastOp == '^') values.push(powv(op1,op2));
					}
					else {
						break;
					}
				}
			}
			ops.push(blockStr[0]);
		}
		else {
			cerr << "Encountered unknown character! ";
			return Value();
		}
		
		i += iIncrement;
	}
	
	// Final calculation not containing any bracket
	if(values.size() != ops.size()+1) {
		cerr << "Syntax error: Expression invalid! ";
		return Value();
	}
	while(!ops.empty()) {
		char lastOp = ops.top();
		ops.pop();
		Value op1 = values.top(); values.pop();
		Value op2 = values.top(); values.pop();
		if(lastOp == '+') values.push(op1+op2);
		else if(lastOp == '-') values.push(op1-op2);
		else if(lastOp == '*') values.push(op1*op2);
		else if(lastOp == '/') values.push(op1/op2);
		else if(lastOp == '^') values.push(powv(op1,op2));
	}
	if(values.size() > 1) {
		cerr << "Syntax error: Invalid expression! ";
		Value ret = Value();
		return ret;
	}
	
	// Record return value
	Value returnValue = values.top();
	
	// Clean up
	while(!ops.empty()) { ops.pop(); }
	while(!values.empty()) { values.pop(); }
	
	return returnValue;
}

// Given the block id of ')', find the block id of corresponding '('
int ExpSolver::findIndexOfBracketEnding(int blockId) { 
	int levelToFind = blocks[blockId].level-1, currentBlockId = blockId;
	while(blocks[currentBlockId].level != levelToFind) {
		if(currentBlockId == 0) return currentBlockId;
		currentBlockId--;
	}
	return currentBlockId+1;
}

// This is for debug: print out the contents in the stacks
void ExpSolver::printStacks(stack<Value> values,stack<char> ops) {
	cout << left << setw(8) << "Values:";
	vector<Value> valuesv;
	while(!values.empty()) {
		valuesv.push_back(values.top());
		values.pop();
	}
	for(int i = valuesv.size()-1; i >= 0; i--) {
		cout << setw(12) << valuesv[i];
	}
	cout << endl;
	cout << setw(8) << "Ops:";
	vector<char> opsv;
	while(!ops.empty()) {
		opsv.push_back(ops.top());
		ops.pop();
	}
	for(int i = opsv.size()-1; i >= 0; i--) {
		cout << setw(12) << opsv[i];
	}
	cout << endl;
}
