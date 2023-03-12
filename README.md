# CRSTL
An alternative or complementary STL that focuses on fast compile times and runtime, focusing on fixed size alternatives for containers and functionality. Some of the guiding goals are:

- No RTTI
- No exceptions thrown or exception handling
- Fast to compile: no dependencies on external headers
- Fast debug code. Avoids deep function nesting so that unoptimized code runs fast
- Containers that don't do heap allocations (**fixed_**)
- Containers that only use the stack (**stack_**)
- Simple C++ code as clear and unobfuscated as possible
- Will add functionality from newer standards, compatible with C++11
- New language features used where available
- No initializer_list<T> support. We provide braced initialization via other mechanisms

## Rationale

- I come mainly from a game development background. Exceptions are never used in such an environment because they introduce a lot of complexity, they are restrictive, and ultimately are not good for performance. Since this library has performance-oriented patterns as one of its main goals, they are not allowed
- Codebases are getting more bloated every day. Whether it's from the include mechanism or template metaprogramming, compilation can take hours on the fastest professional machines the market has to offer. This codebase makes no compromises to achieve a near-perfect avoidance of external dependencies. The code is arguably uglier, but faster to compile
- Debug code performance is important. Some codebases and the STL itself have deep, nested callstacks that make debug performance worse than it should. To that end this codebase will manually expand code or use macros if necessary
- Fixed containers should be used in many circumstances if the requirements allow it. They will not access the heap at any point and can take advantage of the knowledge of the length to save memory. The impose an upper bound on certain operations so make sure your requirements are clear from the beginning
- The STL, Boost and other libraries aim to be as generic as possible and offer a wide range of functionality. Their codebases are very large and complicated to try to achieve that. CRSTL does not have that aim. We use as little template code as we can get away with, and try to use simple constructs to achieve it
- The STL is tied to C++ revisions, which makes upgrading the language and accesing STL features the same operation. CRSTL aims to offer functionality available in later C++ revisions to C++11 compatible compilers
- Modern language features such as constexpr can be very useful in certain scenarios. We don't shy away from using them as appropriate while providing fallbacks for older compilers
- Initializer list follows the philosophy that the STL and the language are coupled together. I strongly disagree with this line of thinking. Initializer list is an implicit construct that is assumed to exist by the compiler to be able to provide things like const auto example = { A, B, C }. Since initializer_list has a lot of problems regarding extra object copies, copyable classes and lifetime issues, we don't allow them

## Additional Design Goals

- The include philosophy differs. Where possible, we forward declare dependencies. It is the user's responsibility to include everything necessary for their usage. For example, vector can be implicitly cast to span, but we don't include span in vector. If this automatic behavior is desired, one can create a new header where both are included and use that.

- Template metaprogramming is avoided as much as possible, while acknowledging the fact that there are many instances where it is necessary. For example, unique_ptr for arrays can be done with a single class and enable_if, or with two classes that share functionality. The second approach is preferred in this library.

This is a work in progress and will be for the time being
