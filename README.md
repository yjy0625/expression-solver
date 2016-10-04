# expression-solver

## Introduction
Expression Solver is a C++ program that solves algebraic expressions.

## Functionalities

Calculate basic expressions using symbols ['+' '-' '*' '/' '^'], brackets ['(' ')'] and numbers
> Example: `1+((2-3*4)/5)^6`  
> Output: `Ans = 65`

Maintain values in fractions until non-fractions break in
> Example: `0.5+1/3`  
> Output: `Ans = 5/6`

Recognize constants ["pi" "e"] and functions ["sin" "cos" "tan" "exp" "sqrt" "floor" "ln" "log"]
> Example: `floor(ln(exp(e))+cos(2*pi))`  
> Output: `Ans = 3`

Load result of previous calculation ["ans"]
> Example: `ans-3`  
> Output: `Ans = 0`

Declare variables (names must start with a letter and only contain letters, numbers and '_'; spaces are ignored)
> Example: `my_variable_1 = ln(2)`  
> Output: `my_variable_1 = 0.693147`

Perform calculations that use user-defined variables
> Example: `exp(my_variable_1)`  
> Output: `Ans = 2`

## Error Handling

Expression validation
> Example: `1_number_at_front = 2`  
> Output: `Variable name invalid!`

> Example: `undefined`  
> Output: `String "undefined" not recognized!`

> Example: `sqrt()`  
> Output: `Invalid expression!`

Arithmatic errors
> Example: `10/(floor(pi)-3)`  
> Output: `Arithmatic error: Denominator is zero!`  

> Example: `(-1)^1.5`  
> Output: `Arithmatic error: Can't power a negative number by a non-integer!`

> Example: `sqrt(-1)`  
> Output: `Arithmatic error: Cannot square root a negative number!`

Syntax errors
> Example: `2=2=2`  
> Output: `Syntax error: Too many '='!`  

> Example: `sqrt(exp(2+log(10))`  
> Output: `Syntax error: Brackets not paired!`  

> Example: `sqrt+3`  
> Output: `Syntax Error: Need brackets after function name!`

## Limitations

Precision error
> Example: `sin(pi*2)`  
> Output: `Ans = 0.000001`

Out-of-bound error
> Example: `10^10`  
> Output: `-2147483648`

Any other error that I happen to miss
> Example: `<Some magical expression>`  
> Output: `<Some magical output>`

## Notes

Input is case sensitive
> Example: `Pi`  
> Output: `String "Pi" not recognized!`

Spaces are ignored
> Example: `s c o r e = 1 0 0`  
> Output: `score = 100`

