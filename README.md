[![AppVeyor](https://ci.appveyor.com/api/projects/status/7ig4jp76wia7xn1k?svg=true)](https://ci.appveyor.com/project/redorav/crstl)
<div align="justify">

# CRSTL
An alternative or complementary STL that focuses on fast compile times and runtime, focusing on fixed size alternatives for containers and functionality. **This is a work in progress and will be for the time being**. Some of the guiding goals are:

- No RTTI
- No exceptions
- Fast to compile: minimal dependencies on external headers, and C++20 module provided when available
- Fast debug code. Avoid deep function nesting so that unoptimized code runs fast
- Containers that don't do heap allocations (**fixed_**)
- Containers that only use the stack (**stack_**)
- Simple C++ code as clear and unobfuscated as possible
- Will add functionality from newer standards, compatible with older versions of C++
- Decoupled and granular. A single include has a small impact

## Rationale

- Exceptions are rarely used in constrained environments because they introduce a lot of complexity, are restrictive, and ultimately not good for performance. Since this library has performance-oriented goals, they are not allowed
- Codebases get more bloated every day, and compilation in some popular codebases can take hours. This codebase tries as much as possible to avoid external dependencies
- Debug performance is important. Some codebases and the STL itself have deep, nested callstacks that make debug performance much worse than desirable. To that end this codebase will manually expand code or use macros as necessary
- Fixed containers should be used in many circumstances if the requirements allow it. They will not access the heap at any point and can take advantage of the knowledge of the length to save memory. They impose an upper bound on certain operations so make sure requirements are clear from the beginning
- The STL, Boost and other libraries aim to be as generic as possible and offer a wide range of functionality. Their codebases are large and complicated to try to achieve that. CRSTL does not have that aim. Instead, it uses as little template code as it can get away with, and tries to use simple constructs to achieve it
- The STL is tied to C++ revisions, which makes upgrading the language and accesing STL features the same operation. CRSTL aims to offer functionality available in later C++ revisions to older compilers. This means codebases can upgrade without upgrading their C++ version
- Modern language features such as constexpr, inline variables or modules can be very useful in certain scenarios. CRSTL doesn't shy away from using them as appropriate while providing fallbacks

## Additional Design Goals

- The include philosophy differs. Where possible, we forward declare dependencies. It is the user's responsibility to include everything necessary for their usage. For example, vector can be implicitly cast to span, but we don't include span in vector. If this automatic behavior is desired, one can create a new header where both are included and use that.

- Template metaprogramming is avoided as much as possible, while acknowledging the fact that there are many instances where it is necessary. For example, unique_ptr for arrays can be done with a single class and enable_if, or with two classes that share functionality. The second approach is preferred in this library.

- The STL interface is well known, as is Boost or EASTL. We maintain an interface that is familiar where it makes sense (e.g. function naming such as push_back() or size()), but very often we'll include additional performance oriented functions such as push_back_uninitialized() or resize_uninitialized()

</div>
