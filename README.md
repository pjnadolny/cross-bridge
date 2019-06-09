# cross-bridge
General solution to the bridge-crossing problem

## Files

- `cross-bridge.cpp` - C++11 code to read a YAML file of people and compute the shortest time to cross the bridge
- `people-*.yaml` - Sample test data files, ranging from 0 to 8 people

## Notes

The code was compiled and tested on macOS HighSierra 10.13.6.

The code relies on the C++ library [yaml-cpp](https://github.com/jbeder/yaml-cpp).

The compiler was g++, Apple LLVM version 10.0.0 (clang-1000.11.45.5)

## Sample output
```
$ ./cross-bridge --people people-4.yaml 
Running...

List of all people:
Person 0 -  Name: A  Speed: 1
Person 1 -  Name: B  Speed: 2
Person 2 -  Name: C  Speed: 5
Person 3 -  Name: D  Speed: 10

Fastest overall person: (A,1)

Naive sequence of bridge crossings:
(B,2) and (A,1) cross
(A,1) returns
(C,5) and (A,1) cross
(A,1) returns
(D,10) and (A,1) cross

The naive fastest total time is: 19

Optimal sequence of bridge crossings:
(B,2) and (A,1) cross
(A,1) returns
(D,10) and (C,5) cross
(B,2) returns
(B,2) and (A,1) cross

The optimal fastest total time is: 17
```
