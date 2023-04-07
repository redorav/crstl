Workspace = 'workspace/'.._ACTION

-- Compilers

-- x86/x64
PlatformMSVC64			= 'MSVC 64'
PlatformMSVC64Modules	= 'MSVC 64 Modules'
PlatformLLVM64			= 'LLVM 64'
PlatformLLVM64Modules	= 'LLVM 64 Modules'
PlatformOSX64			= 'OSX 64'
PlatformLinux64_GCC		= 'Linux64_GCC'
PlatformLinux64_Clang	= 'Linux64_Clang'

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

isMSVC2010 = _ACTION == 'vs2010'
isMSVC2015 = _ACTION == 'vs2015'
isMSVC2017 = _ACTION == 'vs2017'
isMSVC2019 = _ACTION == 'vs2019'
isMSVC2022 = _ACTION == 'vs2022'

supportsARMBuild = isMSVC2017 or isMSVC2019 or isMSVC2022
supportsModules = isMSVC2022

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

		buildoptions
		{
			'/permissive-'
		}
		
	filter {}
	
	includedirs
	{
		includeDir,
	}
	
	vectorextensions ('sse4.1')
	cppdialect(cppDefaultDialects)
	
	if(isMacBuild) then
	
		platforms { PlatformOSX64 }
		toolset('clang')
		architecture('x64')
		buildoptions { '-std=c++11 -msse4.1 -Wno-unused-variable' }
		linkoptions { '-stdlib=libc++' }
		
	elseif(isLinuxBuild) then
	
		platforms { PlatformLinux64_GCC, PlatformLinux64_Clang }
		architecture('x64')
		buildoptions { '-std=c++11 -msse4.1 -Wno-unused-variable' }
		
		filter { 'platforms:'..PlatformLinux64_GCC }
			toolset('gcc')
		
		filter { 'platforms:'..PlatformLinux64_Clang }
			toolset('clang')
		
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

		filter { 'platforms:'..PlatformMSVC64Modules }
			cppdialect('C++20')
			toolset('msc')
			architecture('x64')
			vectorextensions('sse4.1')
			defines { '__SSE4_1__' }
			
		filter { 'platforms:'..PlatformLLVM64Modules }
			cppdialect('C++20')
			toolset(llvmToolset)
			architecture('x64')
			vectorextensions('sse4.1')
			defines { '__SSE4_1__' }
			
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
	kind('staticlib')
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
