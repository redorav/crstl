# CRSTL
An alternative or complementary STL that focuses on fast compile times and runtime, focusing on fixed size alternatives for containers and functionality. **This is a work in progress and will be for the time being**. Some of the guiding goals are:

- No RTTI
- No exceptions thrown or exception handling
- Fast to compile: no dependencies on external headers. C++20 module provided as well when available
- Fast debug code. Avoids deep function nesting so that unoptimized code runs fast
- Containers that don't do heap allocations (**fixed_**)
- Containers that only use the stack (**stack_**)
- Simple C++ code as clear and unobfuscated as possible
- Will add functionality from newer standards, compatible with C++11
- New language features used where available
- Decoupled and granular. A single include has a small impact

## Rationale

- I come mainly from a game development background. Exceptions are rarely used in such an environment because they introduce a lot of complexity, they are restrictive, and ultimately not good for performance. Since this library has performance-oriented patterns as one of its main goals, they are not allowed
- Codebases are getting more bloated every day. Whether it's from the include mechanism or template metaprogramming, compilation can take hours on the fastest professional machines the market has to offer. This codebase makes no compromises to achieve a near-perfect avoidance of external dependencies. The code is arguably uglier, but faster to compile
- Debug code performance is important. Some codebases and the STL itself have deep, nested callstacks that make debug performance worse than is desirable. To that end this codebase will manually expand code or use macros as necessary
- Fixed containers should be used in many circumstances if the requirements allow it. They will not access the heap at any point and can take advantage of the knowledge of the length to save memory. They impose an upper bound on certain operations so make sure your requirements are clear from the beginning
- The STL, Boost and other libraries aim to be as generic as possible and offer a wide range of functionality. Their codebases are large and complicated to try to achieve that. CRSTL does not have that aim. Instead, uses as little template code as it can get away with, and tries to use simple constructs to achieve it
- The STL is tied to C++ revisions, which makes upgrading the language and accesing STL features the same operation. CRSTL aims to offer functionality available in later C++ revisions to C++11 compatible compilers. This means codebases can upgrade without upgrading their C++ version
- Modern language features such as constexpr can be very useful in certain scenarios. CRSTL doesn't shy away from using them as appropriate while providing fallbacks for older compilers

## Additional Design Goals

- The include philosophy differs. Where possible, we forward declare dependencies. It is the user's responsibility to include everything necessary for their usage. For example, vector can be implicitly cast to span, but we don't include span in vector. If this automatic behavior is desired, one can create a new header where both are included and use that.

- Template metaprogramming is avoided as much as possible, while acknowledging the fact that there are many instances where it is necessary. For example, unique_ptr for arrays can be done with a single class and enable_if, or with two classes that share functionality. The second approach is preferred in this library.

- The STL in general is well designed. Therefore we maintain an interface similar to it and other libraries such as Boost or EASTL where it makes sense (e.g. function naming such as push_back or size). In many cases we'll include additional useful functions
