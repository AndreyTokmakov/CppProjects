# Return Value Optimization (RVO and NRVO)

C++ is often criticized for copying large objects when returning by value, but modern C++ compilers have optimizations 
to eliminate those copies. Return Value Optimization (RVO) and Named RVO (NRVO) refer to situations where the 
compiler can construct the return value directly in the caller’s space, avoiding any copy or move.

    struct Big {                           Big makeBig() {
        int data[1000];                        Big b; 
    };                                         // ... fill b ...
                                               return b; // NRVO can eliminate the copy here
                                            }
In the code above, one might think b is copied to the caller. 
However, compilers typically perform NRVO: they will construct b directly in the memory location where the 
function’s return value should go. Thus, no actual copy happens. 

Similarly, if you return Big{}; (an unnamed temporary),
that’s the classic RVO case where the temporary is constructed in place at the call site.


C++17 mandates copy elision in certain cases. 
Specifically, when returning a prvalue (temporary) of the same type as the function return type, 
the compiler must omit the copy and construct in place.

For named return values (NRVO), the standard doesn’t guarantee it, but compilers 
are allowed to do it and generally will if they can


1. Bad example 1

    Big f() {
        return Big(); // guaranteed RVO in C++17, no copy at all
    }

    Big g() {
        Big x;
        // ...
        if(condition) {
            return x;  // NRVO (optional, but most compilers do it)
        } else {
            return Big();
        }
    }

In function g, NRVO might not apply if there are multiple return paths (the compiler might not be able to 
prove one construction can be used for both). 
But the return of a temporary Big() will be RVO’d.

2. Bad example 2

    Big makeBig(bool flag) {
        Big a, b;
        if(flag) 
            return a;
       else  
           return b;
    }

The compiler cannot apply NRVO here because two different local objects might be returned.
It will likely do a move (C++11 onward) which is cheaper than copy, but if Big was not movable, it might copy. 
In scenarios like this, if it matters, you could refactor (perhaps use a single object and set it conditionally, then return).