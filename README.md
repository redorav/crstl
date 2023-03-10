# CRSTL
An alternative or complementary STL that focuses on fast compiles and runtime, focusing on fixed size alternatives for containers and functionality. Some of the guiding goals are:

- No RTTI
- No exceptions thrown or exception handling
- Fast to compile: there are no dependencies on external headers
- Containers that don't do heap allocations (prefixed fixed)
- Containers that only use the stack (prefixed stack)
- Simple C++ code as clear and unobfuscated as possible
- Will add functionality from newer standards, in a C++11 compatible way

## Additional Design Goals

- The include philosophy differs. Where possible, we forward declare dependencies. It is the user's responsibility to include everything necessary for their usage. For example, vector can be implicitly cast to span, but we don't include span in vector. If this automatic behavior is desired, one can create a new header where both are included and use that.

- Template metaprogramming is avoided as much as possible, while acknowledging the fact that there are many instances where it is necessary. For example, unique_ptr for arrays can be done with a single class and enable_if, or with two classes that share functionality. The second approach is preferred in this library.

This is a work in progress and will be for the time being
