/*

exp_solver.h

Author: Jingyun Yang
Date Created: 1/16/15

Description: Header file for ExpSolver class
that takes in algebraic expressions as strings
and solves them.

*/

#include <string>
#include <vector>
#include <stack>
#include "value.h"

using namespace std;

#ifndef EXP_SOLVER_H
#define EXP_SOLVER_H

struct Variable {
	string name;
	Value value;
	Variable(string nm, Value val)
		: name(nm), value(val) {}
};

struct Function {
	string name;
	double (*func)(double);
	Function(string nm, double (*f)(double))
		: name(nm), func(f) {}
};

enum BlockType {
	Num, Sym, Func, Constant, Var, BracL, BracR, Nil
};

struct Block {
	int start, end, level;
	BlockType type;
	Block(): start(0), end(0), level(0), type(Nil) {}
	Block(int s, int e, int l, BlockType tp) 
		: start(s), end(e), level(l), type(tp) {}
};

class ExpSolver {
public:
	
	// Constructor to initialize the ExpSolver object
	ExpSolver(void);
	
	// Inputs a string of expression and outputs the result 
	string solveExp(string);
	
private:
	
	// The partition of expression that the object is 
	// currently working on
	vector<Block> blocks;
	
	// Vector of variables, constants and functions
	// that might be used in calculations
	vector<Variable> variables;
	vector<Variable> constants;
	vector<Function> functions;
	
	// Add predefined constants and functions
	void addPredefined(void);
	
	// Discard spaces in the input string
	string discardSpaces(string str);
	
	// Check whether the input includes variable declaration
	// If so, extract the lhs and rhs of the equal sign
	bool checkDeclaration(string &exp, string &newVarName, bool &isDec);
	
	// This is similar to lexical analysis in a compiler
	// Partition an expression into blocks of different types
	bool groupExp(string exp);
	
	// Analyze whether a string Block is of BlockType Func, Constant or Var
	BlockType analyzeStrType(string str);
	
	// Determine the type of one single character
	BlockType charType(char c);
	
	// Replace every "-" as negative sign by "0-"
	void dealWithNegativeSign(string &exp);

	// Calculate expression in block range [startBlock,endBlock)
	Value calculateExp(string exp, int startBlock, int endBlock);
	
	// Given the block id of ')', find the block id of corresponding '('
	int findIndexOfBracketEnding(int blockId);
	
	// This is for debug: print out the contents in the stacks
	void printStacks(stack<Value> values,stack<char> ops);
};

#endif