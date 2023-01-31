# CRSTL
An alternative or complementary STL that focuses on fast compiles and runtime, often focusing on fixed size alternatives for containers and functionality. Some of the guiding goals are:

- No RTTI
- No exceptions thrown or exception handling
- Fast to compile: The only dependency will be stdint.h
- Containers that don't do heap allocations (called fixed)
- Containers that only use the stack (called stack)
- Simple C++ code as clear and unobfuscated as possible
- Will add functionality from newer standards, in a C++11 compatible way
- 
