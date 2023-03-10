# CRSTL
An alternative or complementary STL that focuses on fast compiles and runtime, often focusing on fixed size alternatives for containers and functionality. Some of the guiding goals are:

- No RTTI
- No exceptions thrown or exception handling
- Fast to compile: there are no dependencies on external headers
- Containers that don't do heap allocations (called fixed)
- Containers that only use the stack (called stack)
- Simple C++ code as clear and unobfuscated as possible
- Will add functionality from newer standards, in a C++11 compatible way

There is a different include philosophy as well. Where possible, we forward declare dependencies. It is the user's responsibility to include everything necessary for their usage. For example, vector can be implicitly cast to span, but we don't include span in vector. The compile error will suggest that span is needed. If this behavior is desired, one can create a new header where both are included and include that.

This is a work in progress and will be for the time being
