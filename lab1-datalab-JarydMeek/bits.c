/* 
 * CS:APP Data Lab 
 * 
 * Jaryd Meek - jame7931
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
 * bitNor - ~(x|y) using only ~ and & 
 *   Example: bitNor(0x6, 0x5) = 0xFFFFFFF8
 *   Legal ops: ~ &
 *   Max ops: 8
 *   Rating: 1
 */
int bitNor(int x, int y)
{
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
int fitsShort(int x)
{
  //If a number is less than or equal to 16 bits (signed), bit number 15 (from the right, start at 0) will match the bits above it (since ints are 32 bits)
  //So, if we wipe out those bits and have them match bit 15, if the number still matches the original number, the number is less than or equal to 16 bits.
  //To do so, we shift the bits 16 times to the left, then 16 to the right.
  //Thanks to arithemetic filling this will make bits 16-31 match whatever bit 15 is.
  //now we compare to see if there are ANY cases where the original number and the number differ after this by using XOR.
  //if they DO differ, XOR will leave a 1 at some point in the number, meaning that the number doesn't fit in the 16 bits
  //Since we want to return 1 when the number fits, and 0 when it doesn't, we need to negate the output.

  return !(((x << 16) >> 16) ^ x);
}
/* 
 * thirdBits - return word with every third bit (starting from the LSB) set to 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 1
 */
int thirdBits(void)
{
  //First, we need to start the pattern with a 9
  int a = 9;
  //1001

  //Second, shift 6 times to the left to move our "added" pattern to the left
  int b = a << 6;
  //1001000000

  //Third, we need to make the new zeros match the pattern again. The OR operation will "copy" any bit that was a 1 before while leaving any zeros alone
  int c = b | a;
  //1001000000
  //0000001001
  //= 1001001001

  //Fourth, do that again to extend the pattern... Since we now have the pattern longer then when we started, we can use that longer pattern to avoid using operators
  int d = c << 12;
  //1001001001000000000000

  //OR again
  int e = d | c;
  //1001001001000000000000
  //0000000000001001001001
  //= 1001001001001001001001

  //So, Now if we continue at our current exponential pace, we will create a pattern that is 36 bits long (24+12)...
  //That's more than the 32 bits allocated by ints, so we will do less this time to complete the pattern
  //has to be multiple of three to continue the pattern correctly
  int f = e << 18;
  //Final OR comparison
  int g = f | e;

  //BIG NOTE -
  //I know I can combine these operations a bunch, and use less variables, etc. I left it here to make it more clear for my understanding and the presentation.
  return g;
}
/* 
 * anyEvenBit - return 1 if any even-numbered bit in word set to 1
 *   Examples anyEvenBit(0xA) = 0, anyEvenBit(0xE) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int anyEvenBit(int x)
{
  //This one generally follows the pattern of thirdbits, but only does it for every other bit instead of every third.
  //create mask of 01010101
  int mask = 0x55;
  //move the mask 8 bits to the left
  //0101010100000000
  int maskShifted = mask << 8;
  //combine the first mask and second mask
  //0101010101010101
  int halfMask = maskShifted | mask;
  //again, move the mask 16 bits to the left
  int topMask = halfMask << 16;
  //combine the left half mask with the right half to make a whole mask
  //01010101010101010101010101010101
  int fullMask = topMask | halfMask;
  //and it with the original value to find if the original value has any bits set to one in an even spot
  //if the original number has a 1 in an even number spot, the and will return a 1 somewhere
  //perform a negation which will make the value 0 if there is a 1 anywhere, or 1 if there are no 1s
  //negate again to flip the above to make it fit the form of the result
  return !!(x & fullMask);
}
/* 
 * copyLSB - set all bits of result to least significant bit of x
 *   Example: copyLSB(5) = 0xFFFFFFFF, copyLSB(6) = 0x00000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int copyLSB(int x)
{
  //Arithmetic filling will copy the previous bit if we move to the right, so we need to move all the way so the lsb is at the left, then move it back to the right.
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
int implication(int x, int y)
{
  //Truth Tables :)
  //P Q P>Q | ~P ~P|Q
  //T T  T  |  F   T
  //T F  F  |  F   F
  //F T  T  |  T   T
  //F F  T  |  T   T
  //We have two operators that would narrow it down to 3:1 (T:F), since that is what we have in the truth table for if
  //And (1:3) and OR (3:1). We can't use And, so we must use or.
  //in order for us to use or to find the answer, we must invert P (as shown above)
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
int bitMask(int highbit, int lowbit)
{
  //create a mask of all 1s
  int mask = ~0;
  int high = mask << highbit << 1;
  int low = mask << lowbit;

  //now we have a value where 1s stop at the high bit, and where 1s start at the low bit,
  //to find where they intersect, we need to invert the high bit, then we can & it with the low bit to only get 1s in the bitMask.
  return ~high & low;
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
int ezThreeFourths(int x)
{
  //multiplying by 3/4 is the same as multiplying by 3 then dividing by 4

  //so first, we multiply by 3. 3 can be written as 2 + 1 so we can mutliply the value by 2 then add the value again to get 3
  //(left shift one multiplies by 2 :D )
  int multiply = (x << 1) + x;

  //if the value was negative, we need to add 1 because of the fact that there is one more negative number on signed numbers
  int sign = multiply >> 31;
  // 3 = 011
  //since if the msb is 1 and we right shift it a bunch, we will end up with all 1's (11111111....111) and we only want to add 1, we and it with 3
  //Textbook page 135 -
  //x + (1 << k) -1 yields the value x/2^k
  //since we will be dividing by 4, k = 2, x + (1 << 2) -1 = x + "100" -1 = x + "011" = x + 3
  //we and it with 3 below to allow us to only apply the bias when the msb is 1 (negative number)
  int bias = sign & 3;
  int divide = (multiply + bias) >> 2;
  return divide;
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
int satMul3(int x)
{
  //if we are multiplying by 3 (3 = x + x + x) there are 2 different places where overflow can occur, so we need to check both for overflow.

  //First, create mask of sign (1 if positive, 0 if negative)
  int sign = x >> 31;

  //then, multiply by 2 (by adding x together twice)
  int multiply2 = x + x;
  //then create a mask of the sign after multiplying by 2
  int multiply2Sign = (((multiply2) >> 31));

  //multiply by 3
  int multiply3 = multiply2 + x;
  //create a mask of the sign after multiplying by 3
  int multiply3Sign = (((multiply3) >> 31));

  // if overflow occurs, the sign of the number will be different (could happen after x+x or 2x+x)
  //we use xor to check if there are ANY places where the bits don't match
  int mult2Overflow = (sign ^ multiply2Sign);
  int mult3Overflow = (multiply2Sign ^ multiply3Sign);

  //since we are comparing 3 numbers, we need to combine the values of the checks together, we do this with a normal or
  //overflow will be all the same digit here.
  int overflow = mult2Overflow | mult3Overflow; //-1 if overflow, 0 if it doesn't

  //so, now we know if overflow occured.
  //so if overflow occured, we need to return tmax or tmin
  //to make tmax/tmin, we need to create a mask with the sign bit opposite all other bits.
  int tminMask = (1 << 31); //mask with 10000.000
  int oppositeSign = ~sign; //inverted sign mask

  //we have a tmin mask, but IF we need to make a tmax mask, we need to flip all the bits.
  //we can check this with xor, it will flip all the bits (since oppositesign is a full mask)
  int finalOverflow = oppositeSign ^ tminMask;

  //if overflow is 0, will be all 0s (thanks to negation), otherwise it will be multiply3 (cause overflow didn't happen)
  // no overflow (or) overflow
  //overflow =  000 (or) 111
  //!overflow =  111 (or) 000
  //multiply3 = 101 (or) 101
  //finalvalue = 101 (or) 000
  int finalValue = (~overflow) & multiply3;
  //if overflow is 0, will return final if overflow is 1, will return
  // no overflow (or) overflow
  //overflow =      000 (or) 111
  //finalOverflow = 100 (or) 100
  //tminOrMax =     000 (or) 100
  int tminOrMax = (overflow & finalOverflow);
  //At this point, ONE of the values will be 0s, and the other will either me tmax/tmin or mult3
  //it will return whichever value isn't all 0s
  return tminOrMax | finalValue; // If overflow, return tmax or tmin, otherwise return mult3.
}
/*
 * bitParity - returns 1 if x contains an odd number of 0's
 *   Examples: bitParity(5) = 0, bitParity(7) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int bitParity(int x)
{
  //XOR will return a 1 if and ONLY if there is both a zero and a 1 in the same bit position with two numbers
  //so we can use xor to compare values, until we've checked every value

  //we can make this efficient by "folding" the number into iself, so each time we are checking half the bits, until we have one bit to check
  //Xor only removes bits (removes by converting 1s to 0s) if it removes 2 at the same time, keeping the number of bits even.
  //example
  //x = 10101010
  //y = x >> 4 = 11111010
  //compares right half with left half
  //x2 = x ^ y = 01010000 = xxxx0000
  //now we only need to check the right half
  //y2 = x2 >> 2 = 00101000
  //compare the right quarters
  //x3 = x2 ^ y2 = 01111000 = xxxxxx00
  //now we only need to check the right 2 bits
  //y3 = x3 >> 1 = 00111100
  //compare them.
  //final value = x3 ^ y3 = 01000100 = xxxxxxx0
  //lsb is a 0 which matches, since we have an even number of 0s in the original number

  //this process removes any parity bits in the "folded" part of the number, but leaves useless info in the upper bits, so we need to
  //&1 to clear out the garbage since we only care about the final bit

  //apply the above pattern to below
  int halfShift = x >> 16;
  int halfValue = x ^ halfShift;

  int quarterShift = halfValue >> 8;
  int quarterValue = halfValue ^ quarterShift;

  int eighthShift = quarterValue >> 4;
  int eighthValue = quarterValue ^ eighthShift;

  int sixteenthShift = eighthValue >> 2;
  int sixteenthValue = eighthValue ^ sixteenthShift;

  int finalShift = sixteenthValue >> 1;
  int finalValue = sixteenthValue ^ finalShift;

  return finalValue & 1;
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x)
{
  //to complete this, we just need to find the position of the msb and return that.
  //how we do this is like a tree (ayyy, 2270 comes back!)
  //example - 36
  //       00100000          we break the full thing in half
  //      *0010*  0000       we check the top half, since it has a bit, we mark down the top half's num of bits, so we put a value in the 4 value place = 100
  //    *00*  10  00 00      we then break the half in half and check the top half, mark down same way, 100
  //0  0  *1*  0  0  0  0  0 we then break the final pair in half and check each half, mark same way,101

  //create tracker
  int msb = 0;
  //for some reason it's throwing errors if I initialize variables as I go, so I'll initialize all here
  int clear16, clear8, clear4, clear2, clear1;

  //to start, shift the bits 16 times to the right, then see if there is still anything in the int
  //will be 1 if there is number in the top 16 bits, 0 if there isn't
  clear16 = !!(x >> 16);
  //makes msb tracker '16 bit value' true if there is a number greater than 16 bits
  msb = msb + (clear16 << 4);

  //same idea, checks the top 8 bits of the entire int, if previous was true, or top of the half if the previous wasn't
  clear8 = !!(x >> (msb + 8));
  //if there was a value in the top 8 of the entire int, add 1 to the 8 area, otherwise add nothing.
  msb = msb + (clear8 << 3);

  //same thing now for 4
  clear4 = !!(x >> (msb + 4));
  //if there was a value in this section, add a 1 to the output
  msb = msb + (clear4 << 2);

  //repeat for the final positions
  clear2 = !!(x >> (msb + 2));
  msb = msb + (clear2 << 1);

  clear1 = !!(x >> (msb + 1));
  msb = msb + clear1;

  return msb;
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
  //save sign of x
  int signMask = x >> 31;
  //we need to save the last 2 bits to have for later (the remainder)
  int last2 = x & 3;
  //divide by 4
  int divide4 = x >> 2;
  //multiply by 3
  int multiply3 = (divide4 << 1) + divide4;
  //multiply the remainder by 3
  int remainderMultiply3 = (last2 << 1) + last2;

  //if the original value is negative, we need to add 1 to it, since rounding to 0 on a negative number is rounding up, not down.
  //so value is either 0 or 3 (3 because 3 is 1 * 3 and we will divide by 4 below)
  int bias = signMask & 3;

  //remainder multiplied by 3 (plus bias to round correctly) /4
  int carry = (remainderMultiply3 + bias) >> 2;

  //(value minus remainder) * 3/4 + (remainder * 3/4) +bias
  return multiply3 + carry;
}
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
unsigned float_neg(unsigned uf)
{
  //we need to flip the leftmost bit
  //save the exponent values
  int nanMask = (255 << 23);
  //now we need to save the fractional values
  //create mask to do so
  int fractionMask = 0x7fffff;
  //check if the float is NAN by comparing the numbers in the exponent to the mask and making sure there is something in the fractional component
  if ((nanMask & uf) == nanMask && (fractionMask & uf))
  {
    return uf;
  }
  //if we're still here, we are a number, so just flip the signmake bit.
  return uf ^ (1 << 31);
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
unsigned float_i2f(int x)
{
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
unsigned float_twice(unsigned uf)
{
  return 2;
}
