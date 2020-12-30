
## PHASE 1

In phase 1 one of the first things that occurs is the calling of the "strings_not_equal" function.

I assume that, as the name implies it checks to see if two strings are equal or not. In order to do so, it either needs two strings passed to the function or one of the strings will be declared in the function, so the first step is to see if anything is in the 1st and 2nd argument registers.

"x/s $rdi" will check the first argument register which then returns our super intricate test string "test."

obviously that isn't the right register, so we continue to check the 2nd argument register. 

"x/s $rsi" will return anything in the 2nd argument register which happens to be the string that we are looking for...

"Verbosity leads to unclear, inarticulate things."

This is the string that our input string compares to to see if they match, so we now know that the solution to phase 1 is the above phrase.


## PHASE 2

We again pass a garbage string to the phase to allow us to look through registers more effectively.

The first thing that occurs in this phase is the calling of the function "read_six_numbers" function. Let's go into that, and see what's happening.

We can see that the "sscanf" function is being called in phase 2
after a quick google search, we can see that sscanf takes in 2 arguments, one being the string to be tested, and one being a formatting string (similar to printf)

let's step until right before we enter the function, so we can check the argument strings and try to figure out what format the bomb wants our answer to be in

Right before we enter the scanf function we can check the registers
function -> read_six_numbers
1st arg -> "x/s $rdi" = "test" (Our test string)
2nd arg -> "x/s $rsi" = "%d %d %d %d %d %d"

Sweet. We got a format string, so we now know our bomb is looking for 6 integers (maybe I could've inferred that with the function name "read_six_numbers" but we don't mess around with bombs here)

let's backout and enter 6 numbers so we can make it past this sscanf function
let's put in a test function of 6 random numbers.

so, we're back in phase_2 and at a compare function.

A quick check shows that rsp is our first digit entered to the bomb, and it's checking to see if the number that we entered is equal to zero. (line +14)

So we know our first digit is *0*

next time we move onto the next compare statement

so, rsp is the stack pointer, and adding 4 bits to the stack moves the stack forward by 1 integer, so now we are comparing 1 to the second integer, and if they are equal we don't go boom.

so, we can see here that our second digit needs to be **1**

Let's keep interpreting. 

here we enter a loop where the function adds the previous two numbers together, and then compares that value against the value you entered.
So the function kinda looks like this

0 1 (1+0) (1+1) (2+1) (3+2)
or
0 1 1 2 3 5
Also known as the fibbonacci sequence. 

So, we now know think that we have the six numbers we need, so we can test them, and they work!

## PHASE 3

First thing we can immediately see when we begin to look at phase 3 is that we have our old friend sscanf again, so we can do the same process to see what type of input we need to provide the function is

the first argument was our test string, and the second argument is "%d %c %d" so we know that the phase is looking for an integer, a character and then another integer

First, a comparison occurs
cmp    $0x2,%eax

which checks if the return of sscanf is greater than 2, if it is, it continues, if not it explodes, if it is, that means we entered at least 3 elements

The next comparison then occurs
cmpl   $0x7,0xc(%rsp)
which checks if the first value is less than 7, if it isn't we go boom.

So, we can see that we are about to enter a switch statement. since we entered 1, we can continue and see where we go.

The next thing to compare is 
cmpl   $0x148,0x8(%rsp) 

This is checking if the 3rd entry to the phase is equal to 0x148 (328 in decimal)

if it is, we continue.

so, the third value is 328

finally, we jump to a comparison that is 
cmp    %al,0x7(%rsp)

this checks the first character (2nd argument) to see if it matches $al

if we "p $al" al = 107

107 binary in ascii is k

so, we know that char = k

so the final answer is 1 k 328


## PHASE 4

0xc(%rsp) - first int
0x8(%rsp) - second int

sscanf = "%d %d"

need something less than 14

14 into edx (3rd param)
0 into esi (2nd param)
first value into edi (1st param)

func4(firstVal, 0, 14)

immediately after calling function 4, checks if eax is not equal to 37


func4
 
 14 -> ebx
 subtract 0
 ebx shift right (divide by 2)
 add 0

if value > ebx {
subtract 1
}
if value < ebx {
add 1
}

func4(x,y,z) {
    a = z
    a = ((a-y)/2)+y

    if (x < a) {
        a = a - 1
        return a + func4(x, y, a)
    } else if (x > a) {
        a = a + 1
        return a + func4(x, a, z)
    } else if (x == a) {
        return a;
    }
}

we need 37 from this function, so we can work backwards from that.

The first time the function runs, it will always return 7 (average of 14 and 0)

37-7 = 30

we still need to take off a bunch, so lets attempt the higher option
func4(x,8,14)

a = 7+1 = 8

average of 8 and 14 = 11

30 - 11 = 19 to go

sum = 7 + 11 = 18

run again, if we go above 11, we'll overshoot.
func4(x, 8, 10)

average = 9

19-9 = 10 to go < 14 so this should be our answer.

done with func4

next we compare the output of that function to 37 (which we saw above)

then we compare our second int (0x8(%rsp)) to 37 as well, so we have to enter 37 as our second number

test with 

10 37

## Phase 5

First thing that happens is string_length gets called, and as you would guess. it counts the length of the string. immediately after the function gets called it checks the return to make sure it equals six. so our string must be of length 6.

so, next we run into a string getting moved into a register

it is a "random" string of chars then a message about termination "maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you?"

"maduiersnfotvbyl" is the "random" string I mentioned... I have a feeling that may be useful later. It's stored in rcx

rbx is our input string

they move the string after the garbage into rcx 

ok, there's another string here after a loop
the string is "sabres"
I think is what we will need to get, but just entering it won't work

back to the loop. I think this is a code type thing, think enigma machine, so we will enter a string, and through this loop it will get mapped to an output string.

ok, going past the loop and looking at what gets passed to strings_not_equal we see our "testin" string got converted to "ieuify"

We have two options at this point. Create a key by just testing abcdef, the ghijklm, and so on and see what each letter is and then backwards convert by hand
OR we can try to figure out the key through the assembly

As much as I'd like to just trial and error a key, I'm going to stay in the spirit of this lab and solve it properly

so, the loop happens six times, then it jumps to the strings_not_equal function so we can assume that each time it loops it's encoding a character in our string.

so, to solve it, we just need to figure out what it's doing each loop.

The thing that it does is take the ascii value of the character in the current loop, ANDs it with 0xf (00001111) and then uses that value as the offset in the array of "random" characters to generate our new character. This then gets added to a string, which when gets compared and if they match our code is good and we move on!

so, we first need to figure out what offset each of the letters in "sabres" is.

maduiersnfotvbyl

s a b  r e s
7 1 13 6 5 7

so those are the offsets that we need, convert each number to binary to find the ascii value we need to input

7 = 0111
1 = 0001
13 = 1101
6 = 0110
5 = 0101

So, ascii values will either be 0110 or 0111 for the leading bits of a lowercase number, so we can use this to just add the above values to 0110

so, 0110 + 0111 = g and so on

*Fun Fact* This phase will have multiple correct strings since you can start with either 0110 or 0111.

create the answer string

gamfeg

test it
it works!


## PHASE 6

First thing thjat really happens is that we all read six numbers with our test string, so as we learned in phase 2, we're looking for 6 numbers :)

first, we go through a loop to make sure each digit is less than 6

the next thing that occurs is that we enter nested loops
and the line 
cmp    %eax,0x0(%rbp)
checks to see if any of our numbers are the same as any others in the string, basically making sure we entered each number once, and only once.


then it sets rdx equal to the address of something called node1

From data structures, any object they called "node" was likely a data structure, so we can see if this is true by running x/30w to see if we have other nodes (we use w here since we expect there to be at least a word of data if any addresses show up)

0x555555758630 <node1>: 0x00000352      0x00000001      0x55758640      0x00005555
0x555555758640 <node2>: 0x000001df      0x00000002      0x55758650      0x00005555
0x555555758650 <node3>: 0x000000ee      0x00000003      0x55758660      0x00005555
0x555555758660 <node4>: 0x00000065      0x00000004      0x55758670      0x00005555
0x555555758670 <node5>: 0x00000126      0x00000005      0x55758120      0x00005555

BINGPOT

there's five nodes here and they each have 3 values. (even though they report 4 values, the last one is the same for every node so we can probably ignore that)

the first and second column look like numbers of some sort, but the 3rd column has something interesting in it, it looks like an address, but not the address of the node we are on. Since there is one address, we can safely assume that this is singly-linked list.

the second column seems to match the node number so I assume that is all it is

the first column is a value of some sort, so I assume that is what the node is storing.

the last thing to note is that since the nodes point to the next one, and node 5 doesn't point to nullptr, that it points to something

0x555555758120 <node6>: 0x00000351      0x00000006      0x00000000

and there's the final node. 

so now what do we do with the nodes since we have all of them?

well, we hit line 218 and it starts checking that our current node's value is greater than or equal to the previous node, then it loops and checks again, if not. boom

so we need to order our nodes from greatest to least


node  address      value   num    ptr
1   0x555555758630 0x352   0x01   0x55758640
2   0x555555758640 0x1df   0x02   0x55758650
3   0x555555758650 0x0ee   0x03   0x55758660
4   0x555555758660 0x065   0x04   0x55758670
5   0x555555758670 0x126   0x05   0x55758120
6   0x555555758120 0x351   0x06   null

1 6 2 5 3 4

test if that works,

It Does!