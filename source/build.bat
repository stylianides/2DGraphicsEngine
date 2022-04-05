@echo off

set CompilerFlags=-EHa- -FC  -GR- -nologo -Od -Zi -Fm
set WarningFlags=-WX -W4 -wd4201 -wd4100 -wd4189 -wd4505
set LinkerFlags=-opt:ref -incremental:no
set ProgramFlags=-DSLOW_BUILD -DDEBUG

if not exist "../build/" mkdir "../build/"

pushd "../build/"

cl -LD %CompilerFlags% %WarningFlags% %ProgramFlags% ../source/engine.cpp /link %LinkerFlags% -EXPORT:EngineUpdateAndRender -EXPORT:EngineOutputSound

cl %CompilerFlags% %WarningFlags% %ProgramFlags% ../source/win32_engine.cpp /link %LinkerFlags% user32.lib gdi32.lib winmm.lib

popd

REM -LD     Construct DLL
REM -EHa-   Turns off exception handling
REM -FC     Full paths for error jumping
REM -Fm     Create map file with name Engine
REM -GR-    Disables Runtime information
REM -nologo Microsoft logo is not displayed
REM -Od     Disables Optimizations
REM -WX     Treat all warnings as errors
REM -W4     Warning Level 4
REM -Z7     Complete Debugging Info, 7.0 Compatible

REM -opt:ref        Discards unecessary functions in our build
REM -incremental:no Disables incremental link