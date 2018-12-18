# Concurrent Stack With STM and Immutable List #

## Several Producers and Several Consumers ##

This code demonstrates a solution for multiple consumers, multiple producers
problem using approach inspired by *functional programming*.

The stack stores integers produced by three threads (Each thread produces
    numbers from 0 to 100). Three consumers are reading these
numbers and add them together. The result of running is four numbers: sum from
each consumer and total sum of those sums.

Notice that sum from each consumer might change from run to run, the program is
not deterministic. However total sum is always the same. Which means that no
number is lost.

## Compilation Note ##

For compiling the code you will need `g++-7` compiler.

## Links ##

The implementation of STM I used: https://github.com/graninas/cpp_stm_free

The immutable list implementation:
https://github.com/BartoszMilewski/Okasaki/blob/master/List/List.h
