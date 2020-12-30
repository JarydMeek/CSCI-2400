## PERF LAB

**Original Score - 1087, 1301**

*Loop Reorder*

So, First things first

Starting in FilterMain.cpp

We can see the main 5 loops that we are encountering

The first 3 loops iterate throught the columns, row, and plane.

If we look inside the loops we can see that we always access each pixel using plane, then row, then column (order of access, but it actually goes through every column,then row, then plane)

The first issue I see is the ordering of the 3 outer loops. For most efficient performance, we need to have plane be the outside loop, then row, then column, since that is the order they are on the data that we're accessing.

So, set the order of the loops and test

**After Loop Reorder - 822, 795**

*Loop Unrolling*

Next, The 2 inside loops are not needed and slower than accessing the addresses directly.

So, lets get rid of them

There are 2 loops that go through a 3x3 subset of the image and sum together the values. So instead of running through 2 loops 3 times, we can just run through the 9 values directly.

So replace the two loops with accessing the 3 addresses directly

**AFTER LOOP UNROLLING - 606, 567**

*Optimization Options*

Now, we can play with optimization options. If we test all the options available to us, we get the following values for each

O0 = 606, 567 (Default)
01 = 279, 234
02 = 278, 241
03 = 281, 224
Os = 282, 227
Ofast = 278, 222


**AFTER OPTIMIZATION OPTIONS (03) - 281, 224**

*Reducing calls*

Calls are very processor intensive, as it is having to completely jump through different areas of the program, so by removing them, we boost our performance a ton.

Lets reduce the get() call that we are running 9 times in each loop.
(Replaced the get in each call with an array)

1. After reducing get() call - 226, 165

Now, we can also reduce the calls to getDivisor
(Move it outside the loop since it doesn't change)

2. After reducing getDivisor - 135, 96

Reduce calls to input -> width and height
(moved outside the loop, don't expect to make big difference, but hey)

3. After reducing calls to h/w - 139, 91


**AFTER REDUCING CALLS - 139, 91**

*Changing Variable Size*

Now, lets make the size of our array smaller, since we are saving values 0->255 in our array, we don't need to use ints, we can use shorts, so lets do that.

In cs1300bmp.h -> int color -> short color

**AFTER CHANGING INTS TO SHORTS - 142, 79**

*OPENMP*

Time for openmp

Add the import tag and the command line argument, and we can start testing different options for omp

The best option (I found) is omp parallel for collapse(3) schedule(static)

This calls omp, tells it to process in parallel on the different threads, then it tells it to collapse the 3 for loops as much as possible, and how to schedule the threads(although this option didn't greatly effect the result)


**AFTER ADDING OPENMP - 46, 29**


## FINAL CPES -> 46, 29