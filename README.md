# Number Tool Language
The project is to implement a command line program that can interpret simple NTlang expressions for working with different number bases and bit widths. This tool will also be handy doing various types of number conversions and bit twiddling.  

## Sample Usage
```
./ntlang -e expression -b base -w width -u
```
* base: the base of the output (supports 2, 10, 16)
* width: the disired width of the output
* -u: unsigned (only supports base of 10)
```
$ ./ntlang -e "1 + 1"
2
$ ./ntlang -e "10" -b 16
0x0000000A
$ ./ntlang -e "10 + 1"
11
$ ./ntlang -e "10 + 1" -b 16
0x0000000B

$ ./ntlang -e "10" -b 16 -w 16
0x000A
$ ./ntlang -e "10" -b 2
0b00000000000000000000000000001010
$ ./ntlang -e "10" -b 2 -w 4
0b1010
$ ./ntlang -e "0x0A" -b 10
10
$ ./ntlang -e "0x0A" -b 2 -w 8
0b00001010

$ ./ntlang -e "((1 + 1) * 1)" -b 16 -w 16
0x0002
$ ./ntlang -e "((1 + 1) * 1)" -b 2 -w 8
0b00000010

$ ./ntlang -e "(1 << 16)" -b 10 -w 32
65536
$ ./ntlang -e "(1 << 16)" -b 10 -w 16
0
$ ./ntlang -e "(1 << 16)" -b 16 -w 32
0x00010000

$ ./ntlang -b 10 -w 8 -e "(2 * (0b1111 & 0b1010))"
20

$ ./ntlang -b 10 -w 8 -e "0b00001000"
8
$ ./ntlang -b 10 -w 4 -e "0b00001000"
-8
$ ./ntlang -b 10 -u -w 4 -e "0b00001000"
8

$ ./ntlang -b 10 -w 8 -e "-128 >- 2"
-32
$ ./ntlang -e "(((((~((-(2 * ((1023 + 1) / 4)) >- 2) << 8)) >> 10) ^ 0b01110) & 0x1E) | ~(0b10000))"
-1

$ ./ntlang -e "0xffffffff"
-1
$ ./ntlang -e "0xffffffff1"
overflows uint32_t: ffffffff1
$ ./ntlang -e "0x000000000ffffffff"
-1
```
### Scanner
The scanner reads the expression from the command line and create a data structure of tokens.
The EBNF grammar (micro syntax) for the scanner is:
```
whitespace  ::=  (' ' | '\t') (' ' | '\t')*

tokenlist  ::= (token)*
token      ::= intlit | hexlit | binlit | symbol
symbol     ::= '+' | '-' | '*' | '/' | '>>' | '>-' | '<<' | '~' | '&' | '|' | '^'
intlit     ::= digit (digit)*
hexlit     ::= '0x' hexdigit (hexdigit)*
binlit     ::= '0b' ['0', '1'] (['0', '1'])*
hexdigit   ::= 'a' | ... | 'f' | 'A' | ... | 'F' | digit
digit      ::= '0' | ... | '9'

# Ignore
whitespace ::= ' ' | '\t' (' ' | '\t')*
```
### Parser
The parser generates a tree (AST) from the scanned tokens that represent valid NTLang programs.
The parser adds the following elements to the EBNF grammar:
```
# Parser

program    ::= expression EOT

expression ::= operand (operator operand)*

operand    ::= intlit
             | hexlit
             | binlit
             | '-' operand
             | '~' operand
             | '(' expression ')'

operator   ::= '+' | '-' | '*' | '/' | '>>' | '<<' | '&' | '|' | '^' | '>-' | '~'
```
The bitwise operators do the same thing as their counterparts in C:
```
Binary operators:

>>   logical shift right
>-   arithmetic shift right (note that C does not have this operator)
<<   logical shift left
&    bitwise and
|    bitwise or
^    bitwise xor

Unary operators:

~    bitwise not
```
### Interpreter
The interpreter walks the AST depth-first, evaluating the expressions defined by the nodes, and printing the results.

The intermediate results in a C uint32_t (unsigned int), which makes conversion to binary or hexadecimal easy.
