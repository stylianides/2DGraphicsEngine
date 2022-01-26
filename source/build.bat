@echo off

set CompilerFlags=-EHa- -FC -FeHomeEngine -FmHomeEngine -FoHomeEngine -GR- -nologo -Od -Oi -Z7
set WarningFlags=-WX -W4 -wd4201 -wd4100 -wd4189 -wd4505
set LinkerFlags=-opt:ref -incremental:no user32.lib gdi32.lib
set ProgramFlags=-DSLOW

if not exist "../build/" mkdir "../build/"
pushd "../build/"
cl %CompilerFlags% %WarningFlags% ../source/win32_platform.cpp /link %LinkerFlags%
popd

REM -EHa-   Turns off exception handling
REM -FC     Full paths for error jumping
REM -Fe     Sets the executable name
REM -Fo	 Sets the object file name
REM -Fm     Create map file with name styliaEngine
REM -GR-    Disables Runtime information
REM -nologo Microsoft logo is not displayed
REM -Oi     Enables intrinsic compiler optimizations
REM -Od     Disables Optimizations for compile times
REM -WX     Treat all warnings as errors
REM -W4     Warning Level 4
REM -Z7     Complete Debugging Info, 7.0 Compatible

REM -opt:ref        Discards unecessary functions in our build
REM -incremental:no Disables incremental link