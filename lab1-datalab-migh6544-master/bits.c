/* 
 * CS:APP Data Lab 
 * 
 * <Michael Ghattas - MIGH6544>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
        
/*  
    ------------------------------------------------------------------------------
    Please note that some of the code used in solving the below problems has been
    inspired by similar online problems and solutions. Some of the sites include
    GitHub, geeks4geeks and google searches. Thank you, Michael Ghattas.
    ------------------------------------------------------------------------------
*/

/* 
 * bitNor - ~(x|y) using only ~ and & 
 *   Example: bitNor(0x6, 0x5) = 0xFFFFFFF8
 *   Legal ops: ~ &
 *   Max ops: 8
 *   Rating: 1
 */
int bitNor(int x, int y) {
/*
    The function distributes the ~ on (x | y).
*/
    return (~x & ~y);
}
/* 
 * fitsShort - return 1 if x can be represented as a 
 *   16-bit, two's complement integer.
 *   Examples: fitsShort(33000) = 0, fitsShort(-32768) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 1
 */
int fitsShort(int x) {
/*
    The function moves x 16 bits to the right to clear the bits, then moves x back
    by 16 bits to LSB, to be able to checks if there is a remainder of any other 
    bits past the 16 bit mark with the XOR operation, thus returning 0 if
    incompatible and 1 for a match!
*/
    return !(((x << 16) >> 16) ^ x);
}
/* 
 * thirdBits - return word with every third bit (starting from the LSB) set to 1
 *   Legl ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 1
 */
int thirdBits(void) {
/*
    The function creates a mask with every 3rd bit set to 1, by first creating 
    the sequence for the first byte and then copying it to to keep scaling up 
    by doubling in size till 32 bit.
*/
    int nibbleMaske = 0x8; 
    int byteMask = ((nibbleMaske << 3) | nibbleMaske) + 1;
    int wordMask = (byteMask << 9) | byteMask;
    return ((wordMask << 15) | wordMask);
}
/* 
 * anyEvenBit - return 1 if any even-numbered bit in word set to 1
 *   Examples anyEvenBit(0xA) = 0, anyEvenBit(0xE) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int anyEvenBit(int x) {
/*
    The function creates a mask with every nibble set to 0101 and then copys
    it to to keep scaling up till 32 bit. The mask set to 1 at every index with
    an even value strting from 0 to 31. This allows it to apply the mask to the
    value of x. The double negation filters to check if the multiplication of the
    mask with the value of x returned any 1s, then return 1 if a bit is found.
*/
    int byteMask = 0x55;
    int wordMask = byteMask << 8 | byteMask;
    return !!(x & ((wordMask << 16) | wordMask));
}
/* 
 * copyLSB - set all bits of result to least significant bit of x
 *   Example: copyLSB(5) = 0xFFFFFFFF, copyLSB(6) = 0x00000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int copyLSB(int x) {
/*
    The function takes the LSB and moves it to become the MSB, then applies an 
    arithmetic right, moving the MSB to become the LSB while copying the value of
    the now LSB to all the bits as it shifts right.
*/
    return ((x << 31) >> 31);
}
/* 
 * implication - return x -> y in propositional logic - 0 for false, 1
 * for true
 *   Example: implication(1,1) = 1
 *            implication(1,0) = 0
 *   Legal ops: ! ~ ^ |
 *   Max ops: 5
 *   Rating: 2
 */
int implication(int x, int y) {
/*
    The function implements the "implication" logic by adding
    the negation of x to y. Thus returning the binary value of x -> y.
*/
    return ((!x) | y);
}
/* 
 * bitMask - Generate a mask consisting of all 1's 
 *   lowbit and highbit
 *   Examples: bitMask(5,3) = 0x38
 *   Assume 0 <= lowbit <= 31, and 0 <= highbit <= 31
 *   If lowbit > highbit, then mask should be all 0's
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int bitMask(int highbit, int lowbit) {
/*
    THe function creats a mask for lowbit, then creats a mask of 1s for highbit that 
    can be combined with lowbit. Thus shifting a negative mask of highbit,
    then multipling it with lowbits.
*/
    return ((~0 << lowbit) & ~(~0 << highbit << 1));
}
/*
 * ezThreeFourths - multiplies by 3/4 rounding toward 0,
 *   Should exactly duplicate effect of C expression (x*3/4),
 *   including overflow behavior.
 *   Examples: ezThreeFourths(11) = 8
 *             ezThreeFourths(-9) = -6
 *             ezThreeFourths(1073741824) = -268435456 (overflow)
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 3
 */
int ezThreeFourths(int x) {
/*
    The function creates a mask for the multiplication of 3 by dividing by multiplining by (2 + 1),
    then creates a mask that adds 1 address a negative number rounding by moving the MSB to become the
    LSB and then combining it with 0x3. Finally it divides by 4 through shifting the multiplication by 3
    mask and the negative number adjusted mask to the right by 2.
*/
    int timesThree = ((x << 1) + x);
    int msbToLSB = timesThree >> 31;
    int negativeMask = msbToLSB & 3;
    return ((timesThree + negativeMask) >> 2);
}
/*
 * satMul3 - multiplies by 3, saturating to Tmin or Tmax if overflow
 *  Examples: satMul3(0x10000000) = 0x30000000
 *            satMul3(0x30000000) = 0x7FFFFFFF (Saturate to TMax)
 *            satMul3(0x70000000) = 0x7FFFFFFF (Saturate to TMax)
 *            satMul3(0xD0000000) = 0x80000000 (Saturate to TMin)
 *            satMul3(0xA0000000) = 0x80000000 (Saturate to TMin)
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 3
 */
int satMul3(int x) {
/*
    The function checks for the possibility of an overflow, then generates an saturation mask for overflow
    accourdingly, then completes the multiplication process and resolves overflow if it occurs.
*/
    int signedMask = x >> 31;
    int timesTwo = x + x;
    int timesThree = timesTwo + x; 
    int timesTwoS = (((timesTwo) >> 31) );
    int timesThreeS = (((timesThree) >> 31) );
//  Returns 1 for overflow:
    int overflowCheck = ((timesTwoS ^ signedMask) | (timesTwoS ^ timesThreeS)) & 1;
//  Creates the overflow mask:
    int overflowMask = (overflowCheck << 31) >> 31;
//  Creates the saturation mask:
    int saturationMask = (~signedMask) ^ (1 << 31);
//  Multiplies by 3 and resolves overflow with saturation:
    return (overflowMask & saturationMask) | ((~overflowMask) & timesThree);
}
/*
 * bitParity - returns 1 if x contains an odd number of 0's
 *   Examples: bitParity(5) = 0, bitParity(7) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int bitParity(int x) {
/*
    The function creates a 32 bit mask in sections, isloating the binary zeros with a negative mask before
    adding 1 to chack if a remaining odd 0 is present at the end.
*/
    x ^= x >> 16;
    x ^= x >> 8;
    x ^= x >> 4;
    x ^= x >> 2;
    x ^= x >> 1;
    return x & 1;
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
/*
    The function inspects the MSB and creates a mask to trim the MSB if possible, then keeps doing the same
    to trimm and hone in on the MSB. Then checks where the MSB is located to calculate its decimal value.
    This follows the logic that we can identigy the MSB and its index. From the MSB index value (which can be
    calculated as 2^x, where x is the value of the index location). We can see that the operation follows the
    the same patern and logic as as the operator log2.
*/
    int MSB, maskOne, shiftOne, maskTwo, shiftTwo, maskThree, shiftThere, maskFour, shiftFour, maskFive, shiftFive;
//  Check if MSB is above 16 bits:
    maskOne = (~0 << 16); 
//  If so, shift by 16 bits to trim
    shiftOne = (!!(x & maskOne)) << 4; 
//  Count the place of the MSB to get decimal value:
    MSB = shiftOne;
//  Shift x if MSB > 16:
    x = x >> shiftOne;
//  Check if MSB is between 8 and 16 bits using the same logic while trimming:
    maskTwo = (0xff << 8);
//  Creates the shift of (0 or 8):
    shiftTwo = (!!(x & maskTwo)) << 3; 
//  Add the shift value to MSB to get MSB Value:
    MSB += shiftTwo;
//  Shift x by (0 or 8)
    x = x >> shiftTwo;
//  Keep repeating logic till 2^x:
    maskThree = 0xf0;
    shiftThere = (!!(x & maskThree)) << 2;
    MSB += shiftThere;
    x = x >> shiftThere;
    
    maskFour = 0xc;
    shiftFour = (!!(x & maskFour)) << 1;
    MSB += shiftFour;
    x = x >> shiftFour;

    maskFive = 2;
    shiftFive =( !!(x & maskFive));
    MSB += shiftFive;
    x = x >> shiftFive;
    
    return MSB;
}
/*
 * trueThreeFourths - multiplies by 3/4 rounding toward 0,
 *   avoiding errors due to overflow
 *   Examples: trueThreeFourths(11) = 8
 *             trueThreeFourths(-9) = -6
 *             trueThreeFourths(1073741824) = 805306368 (no overflow)
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int trueThreeFourths(int x)
{
/*    
    The function firs does an arithmetic left shift to capture the sign of x, then uses a negative mask
    to get last 2 bits (MSB, MSB - 1). From there it checks that x is divisible by 4 through shifting left
    by 2. Finally multiplies by 3 by adding the value of the division three times, then puts all the pieces
    together to add the final value with remainder for the final result.
*/
//  Finding x's signed value:
    int sign = x >> 31;
//  Capturing x's value:
    int last2 = x & 3;
//  Dividing by 4:
    int div4 = x >> 2;
//  Multiplying by 3:
    int mul3 = div4 + div4 + div4;
//  Applying the sign of x, dividing its true value by 4 and multiplying it by 3:
    return (mul3 + (last2 + (last2 << 1) + (sign & 3) >> 2));
}

/*
    Extra Credit was not attempted!
/*

 * Extra credit
 */
/* 
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
 return 2;
}
/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
  return 2;
}
/* 
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
  return 2;
}
