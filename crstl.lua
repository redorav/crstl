Workspace = 'workspace/'.._ACTION

-- Compilers

-- Windows x86/x64
PlatformMSVC64			= 'MSVC 64'
PlatformMSVC64Modules	= 'MSVC 64 Modules'
PlatformLLVM64			= 'LLVM 64'
PlatformLLVM64Modules	= 'LLVM 64 Modules'

-- MacOS x86/64
PlatformOSX64			= 'OSX 64'

-- Linux x86/64
PlatformLinux64_GCC_Cpp11	= 'Linux64_GCC_C++11'
PlatformLinux64_Clang_Cpp11	= 'Linux64_Clang_C++11'
PlatformLinux64_GCC_Cpp14	= 'Linux64_GCC_C++14'
PlatformLinux64_Clang_Cpp14	= 'Linux64_Clang_C++14'

-- NEON
PlatformARM 			= 'MSVC ARM'
PlatformARM64 			= 'MSVC ARM64'
PlatformAndroidARM 		= 'Android ARM'
PlatformAndroidARM64 	= 'Android ARM64'

UnitTestProject = 'unit_tests'
AndroidProject = 'crstl_android'

isMacBuild = _ACTION == 'xcode4'
isLinuxBuild = _ACTION == 'gmake2'
isWindowsBuild = not isMacBuild and not isLinuxBuild

supportsARMBuild = _ACTION >= 'vs2017'
supportsModules = _ACTION >= 'vs2022'

cppDefaultDialect = 'C++11'

-- Directories
srcDir = 'unit_tests'
includeDir = 'include'
moduleDir = 'module'

workspace('crstl')
	configurations { 'Debug', 'Release' }
	location (Workspace)
	warnings('extra')

	filter('toolset:msc*')
		flags
		{
			'multiprocessorcompile',
		}
		
	filter {}
	
	includedirs
	{
		includeDir,
	}
	
	vectorextensions ('sse4.1')
	cppdialect(cppDefaultDialects)
	flags { 'fatalcompilewarnings' }
	
	if(isMacBuild) then
	
		platforms { PlatformOSX64 }
		toolset('clang')
		architecture('x64')
		buildoptions { '-std=c++11' }
		linkoptions { '-stdlib=libc++' }
		
	elseif(isLinuxBuild) then
	
		platforms { PlatformLinux64_GCC_Cpp11, PlatformLinux64_Clang_Cpp11, PlatformLinux64_GCC_Cpp14, PlatformLinux64_Clang_Cpp14 }
		architecture('x64')

		filter { 'platforms:'..PlatformLinux64_GCC_Cpp11 }
			toolset('gcc')
		buildoptions { '-std=c++11' }
		
		filter { 'platforms:'..PlatformLinux64_Clang_Cpp11 }
			toolset('clang')
			buildoptions { '-std=c++11' }
			
		filter { 'platforms:'..PlatformLinux64_GCC_Cpp14 }
			toolset('gcc')
			buildoptions { '-std=c++14' }
		
		filter { 'platforms:'..PlatformLinux64_Clang_Cpp14 }
			toolset('clang')
			buildoptions { '-std=c++14' }
		
	else
	
		platforms
		{
			PlatformMSVC64,
			PlatformLLVM64,
		}

		-- Add modules platform if environment supports them
		if(supportsModules) then

			platforms
			{
				PlatformMSVC64Modules,
				PlatformLLVM64Modules
			}

		end

		-- Add ARM platform if environment supports them
		if(supportsARMBuild) then

			platforms
			{
				PlatformARM, 
				PlatformARM64, 
				PlatformAndroidARM, 
				PlatformAndroidARM64
			}

		end
	
		local llvmToolset;
		
		if (_ACTION == 'vs2015') then
			llvmToolset = 'msc-llvm-vs2014';
		elseif(_ACTION == 'vs2017') then
			llvmToolset = 'msc-llvm';
		else
			llvmToolset = 'msc-clangcl';
		end
		
		startproject(UnitTestProject)

		filter { 'platforms:'..PlatformMSVC64 }
			toolset('msc')
			architecture('x64')
			vectorextensions('sse4.1')
			defines { '__SSE4_1__' }
			buildoptions { '/permissive-' }

		filter { 'platforms:'..PlatformMSVC64Modules }
			cppdialect('C++20')
			toolset('msc')
			architecture('x64')
			vectorextensions('sse4.1')
			defines { '__SSE4_1__', 'CRSTL_USE_CPP_MODULE' }
			buildoptions { '/permissive-' }
			
		filter { 'platforms:'..PlatformLLVM64Modules }
			cppdialect('C++20')
			toolset(llvmToolset)
			architecture('x64')
			vectorextensions('sse4.1')
			defines { '__SSE4_1__', 'CRSTL_USE_CPP_MODULE' }
			
		filter { 'platforms:'..PlatformLLVM64 }
			toolset(llvmToolset)
			architecture('x64')
			vectorextensions('avx')
			buildoptions { '-Wno-unused-variable -mavx' }
			
		filter { 'platforms:'..PlatformARM }
			architecture('arm')
			vectorextensions ('neon')
			
		filter { 'platforms:'..PlatformARM64 }
			architecture('arm64')
			vectorextensions ('neon')
			
		filter { 'platforms:'..PlatformAndroidARM }
			system('android')
			architecture('arm')
			vectorextensions('neon')
			buildoptions { '-Wno-unused-variable' }
			linkoptions { '-lm' } -- Link against the standard math library
			
		filter { 'platforms:'..PlatformAndroidARM64 }
			system('android')
			architecture('arm64')
			vectorextensions('neon')
			buildoptions { '-Wno-unused-variable' }
			linkoptions { '-lm' } -- Link against the standard math library
			
		filter{}
	end
	
	filter { 'configurations:Debug' }
		defines { 'DEBUG' }
		symbols ('full')
		optimize('debug')
		
	filter { 'configurations:Release' }
		defines { 'NDEBUG' }
		inlining('auto')
		optimize('speed')

project ('crstl')
	kind('utility')
	language('c++')
	files
	{
		includeDir..'/**.h',
		includeDir..'/*.natvis'
	}
	
project (UnitTestProject)
	kind('consoleapp')
	
	files
	{
		srcDir..'/*.cpp',
		srcDir..'/*.h',
	}
	
	-- Add module as compilation target for any platform that supports modules
	filter { 'platforms:'..PlatformMSVC64Modules }
		
		files
		{
			moduleDir..'/*.ixx'
		}
	
	filter { 'platforms:'..PlatformAndroidARM..' or '.. PlatformAndroidARM64 }
		kind('sharedlib')
		files
		{
			srcDir..'/android/android_native_app_glue.h',
			srcDir..'/android/android_native_app_glue.c',
		}
		
	filter{}
	
	includedirs
	{
		srcDir..'/**.h'
	}

if (supportsARMBuild) then

project (AndroidProject)
	removeplatforms('*')
	platforms { PlatformAndroidARM, PlatformAndroidARM64 }
	kind('Packaging') -- This type of project builds the apk
	links (UnitTestProject) -- Android needs to link to the main project which was built as a dynamic library
	androidapplibname(UnitTestProject)
	files
	{
		srcDir..'/android/AndroidManifest.xml',
		srcDir..'/android/build.xml',
		srcDir..'/android/project.properties',
		srcDir..'/android/res/values/strings.xml',
	}
end
