
#include <windows.h>
#include <stdio.h>
#include <xinput.h>

#include "win32_home_engine.h"
#include "home_engine_platform.h"

global bool32 GlobalRunning;
global bool32 GlobalPause;
global int64 GlobalTicksPerSecond;
global win32_image_buffer GlobalImageBuffer;

#define X_INPUT_GET_STATE(Name) DWORD Name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);

X_INPUT_GET_STATE(XInputGetStateStub)
{
    return(ERROR_DEVICE_NOT_CONNECTED);
}
global x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define X_INPUT_SET_STATE(Name) DWORD Name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);

X_INPUT_SET_STATE(XInputSetStateStub)
{
    return(ERROR_DEVICE_NOT_CONNECTED);
}

global x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

internal 
DEBUG_PLATFORM_READ_FILE(Win32DEBUGReadFile)
{
    file_contents Result = {};
    
    HANDLE FileHandle = CreateFileA(Filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    
    Assert(FileHandle != INVALID_HANDLE_VALUE);
    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD FileSizeHigh = 0;
        DWORD FileSizeLow = GetFileSize(FileHandle, &FileSizeHigh);
        Assert((FileSizeHigh == 0) && FileSizeLow > 0);
        
        Result.Size = FileSizeLow;
        Result.Contents = VirtualAlloc(0, Result.Size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        Assert(Result.Contents);
        
        if(Result.Contents)
        {
            DWORD BytesRead = 0;
            int32 Error = ReadFile(FileHandle, Result.Contents, Result.Size, &BytesRead, 0);
            
            Assert(Error);
            Assert(BytesRead == Result.Size);
        }
        
        CloseHandle(FileHandle);
    }
    
    return(Result);
}

internal
DEBUG_PLATFORM_WRITE_FILE(Win32DEBUGWriteFile)
{
    HANDLE FileHandle = CreateFileA(Filename, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, 0, 0);
    
    Assert(FileHandle != INVALID_HANDLE_VALUE);
    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD Error = SetFilePointer(FileHandle, 0, 0, FILE_END);
        Assert(Error != INVALID_SET_FILE_POINTER);
        
        DWORD BytesWritten = 0;
        WriteFile(FileHandle, BytesToWrite, Size, &BytesWritten, 0);
        Assert(Size == BytesWritten);
        
        Error = CloseHandle(FileHandle);
        Assert(Error);
    }
}

internal
DEBUG_PLATFORM_FREE_FILE(Win32DEBUGFreeFile)
{
    int32 Error = VirtualFree(FileContents.Contents, 0, MEM_RELEASE);
    Assert(Error);
}

internal FILETIME Win32GetFileLastWriteTime(char *Filename)
{
    FILETIME Result = {};
    
    HANDLE FileHandle;
    FileHandle = CreateFileA(Filename, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    
    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        GetFileTime(FileHandle, 0, 0, &Result);
        CloseHandle(FileHandle);
    }
    
    return(Result);
}

internal void
Win32ConcatString(char *Dest, uint32 DestSize, char *Str)
{
    while((*Str) && (DestSize > 0))
    {
        *Dest++ = *Str++;
        DestSize--;
    }
}

internal void 
Win32ContructDLLPath(char *Dest, int32 DestSize, char *DLLPath)
{
    GetCurrentDirectory(DestSize, Dest);
    
    char *Current = Dest;
    uint32 CurrentSize = DestSize;
    
    char *LastSlash = 0;
    uint32 LastSlashSize = 0;
    
    while((*Current) && (CurrentSize > 0))
    {
        if(*Current == '\\')
        {
            LastSlash = Current;
            LastSlashSize = CurrentSize;
        }
        
        CurrentSize--;
        Current++;
    }
    
    Win32ConcatString(LastSlash, LastSlashSize, DLLPath);
}

internal void 
Win32ReloadEngineDLL(win32_engine_code *EngineCode)
{
    if(EngineCode->EngineDLL_Loaded)
    {
        FreeLibrary(EngineCode->EngineDLL_Loaded);
        EngineCode->EngineDLL_Loaded = 0;
        EngineCode->EngineUpdateAndRender = 0;
        EngineCode->EngineOutputSound = 0;
    }
    
    if(CopyFileA(EngineCode->EngineDLLPath, EngineCode->EngineDLLPath_Loaded, FALSE))
    {
        EngineCode->EngineDLL_Loaded = LoadLibraryA(EngineCode->EngineDLLPath_Loaded);
        
        if(EngineCode->EngineDLL_Loaded)
        {
            EngineCode->EngineUpdateAndRender = (engine_update_and_render *)GetProcAddress(EngineCode->EngineDLL_Loaded, "EngineUpdateAndRender");
            EngineCode->EngineOutputSound = (engine_output_sound *)GetProcAddress(EngineCode->EngineDLL_Loaded, "EngineOutputSound");
            
            EngineCode->EngineDLL_LastWriteTime = Win32GetFileLastWriteTime(EngineCode->EngineDLLPath);
        }
    }
}

internal void
Win32StartRecording(win32_replay_stream *ReplayStream)
{
    DeleteFile(ReplayStream->Filename);
    
    ReplayStream->IsRecording = true;
    ReplayStream->RecordingIndex = 0;
}

internal void
Win32RecordInput(win32_replay_stream *ReplayStream, engine_input *Input)
{
    DWORD BytesWritten = 0;
    
    ReplayStream->RecordFile = CreateFileA(ReplayStream->Filename, GENERIC_READ|GENERIC_WRITE, 0, 0, OPEN_ALWAYS, 0, 0);
    
    Assert(ReplayStream->RecordFile);
    DWORD Error = SetFilePointer(ReplayStream->RecordFile, 0, 0, FILE_END);
    Assert(Error != INVALID_SET_FILE_POINTER);
    
    Error = WriteFile(ReplayStream->RecordFile, Input, sizeof(engine_input), &BytesWritten, 0);
    Assert(sizeof(engine_input) == BytesWritten);
    
    if(Error)
    {
        ReplayStream->RecordingIndex++;
    }
    
    CloseHandle(ReplayStream->RecordFile);
}

internal void
Win32StopRecording(win32_replay_stream *ReplayStream)
{
    ReplayStream->IsRecording = false;
    CloseHandle(ReplayStream->RecordFile);
}

internal void
Win32StartPlayback(win32_replay_stream *ReplayStream)
{
    ReplayStream->IsPlayingBack = true;
    ReplayStream->PlayingIndex = 0;
}

internal engine_input
Win32GetPlayBackInput(win32_replay_stream *ReplayStream)
{
    engine_input Result = {};
    
    if(ReplayStream->IsPlayingBack)
    {
        DWORD BytesRead = 0;
        
        HANDLE FileHandle = CreateFileA(ReplayStream->Filename, GENERIC_READ, FILE_SHARE_READ, 
                                        0, OPEN_EXISTING, 0, 0);
        
        
        if(ReplayStream->PlayingIndex == ReplayStream->RecordingIndex)
        {
            ReplayStream->PlayingIndex = 0;
        }
        
        DWORD Error = SetFilePointer(FileHandle, 
                                     sizeof(engine_input)*ReplayStream->PlayingIndex, 
                                     0, FILE_BEGIN);
        Assert(Error != INVALID_SET_FILE_POINTER);
        
        Error = ReadFile(FileHandle, &Result, 
                         sizeof(engine_input), &BytesRead, 0);
        
        Assert(Error);
        Assert(BytesRead == sizeof(engine_input));
        
        ReplayStream->PlayingIndex++;
        
        CloseHandle(FileHandle);
    }
    
    return(Result);
}

internal void
Win32StopPlayback(win32_replay_stream *ReplayStream)
{
    ReplayStream->IsPlayingBack = false;
}

internal void Win32LoadXInput()
{
    HMODULE XInputLib = LoadLibraryA("xinput1_4.dll");
    
    if(!XInputLib)
    {
        XInputLib = LoadLibraryA("xinput1_3.dll");
    }
    
    if(XInputLib)
    {
        XInputGetState = (x_input_get_state *)GetProcAddress(XInputLib, "XInputGetState");
        XInputSetState = (x_input_set_state *)GetProcAddress(XInputLib, "XInputSetState");
        
        if(!XInputGetState)
        {
            XInputGetState = XInputGetStateStub;
        }
        
        if(!XInputSetState)
        {
            XInputSetState = XInputSetStateStub;
        }
    }
    else
    {
        XInputGetState = XInputGetStateStub;
        XInputSetState = XInputSetStateStub;
    }
}

internal void
Win32InputProcessButton(engine_input_button *Button,  bool32 Pressed)
{
    Button->Press = (Pressed) ? Button->Press + 1 : 0;
}

internal inline
int64 Win32GetTime()
{
    LARGE_INTEGER Ticks = {};
    
    QueryPerformanceCounter(&Ticks);
    Assert(Ticks.QuadPart != 0);
    
    return(Ticks.QuadPart);
}

inline win32_window_dim
Win32GetWindowDim(HWND Window)
{
    win32_window_dim Result;
    
    RECT Rect;
    GetClientRect(Window, &Rect);
    Result.Width = Rect.right - Rect.left;
    Result.Height = Rect.bottom - Rect.top;
    
    return(Result);
}


internal void 
Win32ResizeImageBuffer(win32_image_buffer *Buffer, uint32 Width, uint32 Height)
{
    if(Buffer->Pixels)
    {
        VirtualFree(Buffer->Pixels, 0, MEM_RELEASE);
        Buffer->Pixels = 0;
    }
    
    Buffer->Width = Width;
    Buffer->Height = Height;
    
    Buffer->BmpInfo = {0};
    Buffer->BmpInfo.bmiHeader.biWidth = Width;
    // NOTE(stylia): Negative height means top down bitmap
    Buffer->BmpInfo.bmiHeader.biHeight = -(int32)Height; 
    Buffer->BmpInfo.bmiHeader.biPlanes = 1;
    Buffer->BmpInfo.bmiHeader.biBitCount = 8*BYTES_PER_PIXEL;
    Buffer->BmpInfo.bmiHeader.biCompression = BI_RGB;
    Buffer->BmpInfo.bmiHeader.biSize = sizeof(Buffer->BmpInfo.bmiHeader);
    Buffer->Pitch = Width*BYTES_PER_PIXEL;
    
    mem_index BmpSize = BYTES_PER_PIXEL*Width*Height;
    
    Buffer->Pixels = VirtualAlloc(0, BmpSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    Assert(Buffer->Pixels);
}

internal void Win32CopyImageBufferToDC(win32_image_buffer *Buffer, 
                                       HDC DC, uint32 DCWidth, uint32 DCHeight, 
                                       uint32 OffsetX = 10, uint32 OffsetY = 10)
{
    Assert((DCWidth > Buffer->Width + OffsetX) &&
           (DCHeight > Buffer->Height + OffsetY));
    
    if((DCWidth > Buffer->Width + OffsetX) &&
       (DCHeight > Buffer->Height + OffsetY))
    {
        PatBlt(DC, 0, 0, OffsetX, DCHeight, BLACKNESS);
        PatBlt(DC, 0, 0, DCWidth, OffsetY, BLACKNESS);
        PatBlt(DC, OffsetX + Buffer->Width, 0, DCWidth, DCHeight, BLACKNESS);
        PatBlt(DC, 0, OffsetY + Buffer->Height, DCWidth, DCHeight, BLACKNESS);
        
        uint32 DestX = OffsetX;
        uint32 DestY = OffsetY;
        uint32 DestWidth = Buffer->Width;
        uint32 DestHeight = Buffer->Height;
        
        uint32 SourceX = 0;
        uint32 SourceY = 0;
        uint32 SourceWidth = Buffer->Width;
        uint32 SourceHeight = Buffer->Height;
        
        StretchDIBits(DC, DestX, DestY, DestWidth, DestHeight,
                      SourceX, SourceY, SourceWidth, SourceHeight, 
                      Buffer->Pixels, &Buffer->BmpInfo,
                      DIB_RGB_COLORS, SRCCOPY);
    }
}

internal LRESULT CALLBACK
Win32WindowProc(HWND Window, 
                UINT Message, 
                WPARAM WParam, 
                LPARAM LParam)

{
    LRESULT Result = 0;
    switch(Message)
    {
        case WM_QUIT:
        case WM_DESTROY:
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            InvalidCodePath;
        }break;
        
        case WM_ACTIVATEAPP:
        {
            OutputDebugString("WM_ACTIVATE\n");
        }break;
        
        case WM_PAINT:
        {
            PAINTSTRUCT Paint = {0};
            HDC DC = BeginPaint(Window, &Paint);
            win32_window_dim WindowDim = Win32GetWindowDim(Window);
            Win32CopyImageBufferToDC(&GlobalImageBuffer, DC, 
                                     WindowDim.Width, WindowDim.Height);
            EndPaint(Window, &Paint);
        }break;
        
        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        }break;
    };
    
    return(Result);
}

int CALLBACK WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, PSTR CmdLine, INT CmdShow)
{
    LARGE_INTEGER PerformanceCounter;
    if(!QueryPerformanceFrequency(&PerformanceCounter))
    {
        // TODO(stylia): Proper error handling
        // TODO(stylia): Older system failure?
        DWORD Error = GetLastError();
        OutputDebugString("Could get performance frequency \n");
        return(-1);
    }
    
    GlobalTicksPerSecond = PerformanceCounter.QuadPart;
    real32 TargetFPS = 60;
    real32 TargetSecondsPerFrame = 1.0f / TargetFPS;
    int64 TargetTicksPerFrame = int64(TargetSecondsPerFrame * GlobalTicksPerSecond);
    
    // NOTE(stylia): Set the best granularity for timers, 
    //               so the task scheduler can be more accurate
    //               waking up the app after it sleeps
    uint32 MSTimerGranularity = 1;
    while(timeBeginPeriod(MSTimerGranularity) == TIMERR_NOCANDO)
    {
        MSTimerGranularity++;
        if(MSTimerGranularity == 10)
        {
            break;
        }
    }
    
    WNDCLASSA WindowClass = {0};
    WindowClass.style = CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc = (WNDPROC)Win32WindowProc;
    WindowClass.hInstance = Instance;
    WindowClass.hCursor = LoadCursor(0, IDC_ARROW);
    WindowClass.lpszClassName = "StyliaEngineWindow";
    
    // TODO(stylia): Design Icon
    //    WindowClass.hIcon;
    
    if(!RegisterClassA(&WindowClass))
    {
        DWORD Error = GetLastError();
        OutputDebugString("Could not register class: Error \n");
        return(-1);
    }
    
    // TODO(stylia): Think about the window size and target res
    // WindowWidth = 1024 + 64
    // WindowHeight = 768 + 64
    HWND Window = CreateWindowA("StyliaEngineWindow", "StyliaEngine",
                                WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT, CW_USEDEFAULT, 
                                1088, 832, 0, 0, Instance, 0);
    if(!Window)
    {
        OutputDebugString("Window could not be crated\n");
        return(-1);
    }
    
    ShowWindow(Window, CmdShow);
    HDC WindowDC = GetDC(Window);
    
    
    win32_state WinState = {};
    // TODO(stylia): Think about minimum requirments
    WinState.PermanentMemorySize = MegaBytes(100);
    WinState.TransientMemorySize = GigaBytes(1);
    
    WinState.Memory = VirtualAlloc(0, WinState.PermanentMemorySize + 
                                   WinState.TransientMemorySize
                                   , MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    if(!WinState.Memory)
    {
        OutputDebugString("Could not initialize program memory \n");
        return(-1);
    }
    
    // TODO(stylia): Multiple replay streams
    Win32ConcatString(WinState.ReplayStream.Filename, MAX_PATH, ".\\home_engine_1.hei");
    
    
    
    win32_engine_code EngineCode = {};
    GetCurrentDirectory(256, EngineCode.EngineDLLPath);
    Win32ContructDLLPath(EngineCode.EngineDLLPath, 256, "\\build\\home_engine.dll\0");
    Win32ContructDLLPath(EngineCode.EngineDLLPath_Loaded, 256, "\\build\\home_engine_loaded.dll");
    Win32ReloadEngineDLL(&EngineCode);
    
    // TODO(stylia): Think about the DIB size
    Win32ResizeImageBuffer(&GlobalImageBuffer, 1024, 768);
    
    if(!GlobalImageBuffer.Pixels)
    {
        OutputDebugString("DIB Section could not be initialized \n");
        return(-1);
    }
    
    engine_state EngineState = {};
    EngineState.DEBUGPlatformReadFile = Win32DEBUGReadFile;
    EngineState.DEBUGPlatformWriteFile = Win32DEBUGWriteFile;
    EngineState.DEBUGPlatformFreeFile = Win32DEBUGFreeFile;
    
    Win32LoadXInput();
    
    GlobalRunning = true;
    while(GlobalRunning)
    {
        int64 BeginFrame = Win32GetTime();
        
        FILETIME DLLWriteTimeNow = Win32GetFileLastWriteTime(EngineCode.EngineDLLPath); 
        
        if(CompareFileTime(&DLLWriteTimeNow, &EngineCode.EngineDLL_LastWriteTime) == FILE_TIME_LATER)
        {
            Win32ReloadEngineDLL(&EngineCode);
        }
        
        engine_input EngineInput = {};
        MSG Message;
        while(PeekMessageA(&Message, 0, 0, 0, PM_REMOVE))
        {
            engine_input_controller *Keyboard = &EngineInput.Keyboard;
            
            switch(Message.message)
            {
                case WM_KEYDOWN:
                case WM_KEYUP:
                case WM_SYSKEYDOWN:
                case WM_SYSKEYUP:
                {
                    bool32 IsDown = ((Message.message == WM_KEYUP) ||
                                     (Message.message == WM_SYSKEYUP)) ? false : true;
                    
                    // TODO(stylia): Think about button layout, this is temporary
                    switch(Message.wParam)
                    {
                        case W_KEY:
                        {
                            Win32InputProcessButton(&Keyboard->Buttons.Up, IsDown);
                        }break;
                        
                        case S_KEY:
                        {
                            Win32InputProcessButton(&Keyboard->Buttons.Down, IsDown);
                        }break;
                        
                        case A_KEY:
                        {
                            Win32InputProcessButton(&Keyboard->Buttons.Left, IsDown);
                        }break;
                        
                        case D_KEY:
                        {
                            Win32InputProcessButton(&Keyboard->Buttons.Right, IsDown);
                        }break;
                        
                        case SPACE_KEY:
                        {
                            Win32InputProcessButton(&Keyboard->Buttons.Jump, IsDown);
                        }break;
                        
                        case E_KEY:
                        {
                            Win32InputProcessButton(&Keyboard->Buttons.Attack, IsDown);
                        }break;
                        
                        case F_KEY:
                        {
                            Win32InputProcessButton(&Keyboard->Buttons.Power, IsDown);
                        }break;
                        
                        case ESC_KEY:
                        {
                            Win32InputProcessButton(&Keyboard->Buttons.Start, IsDown);
                        }break;
                        
                        case P_KEY:
                        {
                            GlobalPause = (GlobalPause) ? false : true;
                        }break;
                        
                        case L_KEY:
                        {
                            if(IsDown)
                            {
                                if(!WinState.ReplayStream.IsPlayingBack)
                                {
                                    if(!WinState.ReplayStream.IsRecording)
                                    {
                                        Win32StartRecording(&WinState.ReplayStream);
                                    }
                                    else
                                    {
                                        Win32StopRecording(&WinState.ReplayStream);
                                        Win32StartPlayback(&WinState.ReplayStream);
                                    }
                                }
                                else
                                {
                                    Win32StopPlayback(&WinState.ReplayStream);
                                    
                                }
                            }
                        }break;
                    }
                }break;
                
                case WM_DESTROY:
                case WM_QUIT:
                {
                    GlobalPause = true;
                    GlobalRunning = false;
                }break;
                
                default:
                {
                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                }break;
            }
        }
        
        if(!GlobalPause)
        {
            if(!WinState.ReplayStream.IsPlayingBack){
                
                for(uint32 ControllerIndex = 0;
                    ControllerIndex < Min(MAX_PLAYERS, MAX_CONTROLLERS);
                    ++ControllerIndex)
                {
                    XINPUT_STATE ControllerState;
                    
                    if(XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
                    {
                        EngineInput.Controllers[ControllerIndex].IsConnected = true;
                        
                        XINPUT_GAMEPAD *Gamepad = &ControllerState.Gamepad;
                        
                        bool32 Up = Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_UP;
                        bool32 Down = Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
                        bool32 Left = Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
                        bool32 Right = Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
                        bool32 A = Gamepad->wButtons & XINPUT_GAMEPAD_A;
                        bool32 X = Gamepad->wButtons & XINPUT_GAMEPAD_X;
                        bool32 RightShoulder = Gamepad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
                        bool32 Start = Gamepad->wButtons & XINPUT_GAMEPAD_START;
                        
                        engine_input_controller *Controller = &EngineInput.Controllers[ControllerIndex];
                        Win32InputProcessButton(&Controller->Buttons.Up, Up);
                        Win32InputProcessButton(&Controller->Buttons.Down, Down);
                        Win32InputProcessButton(&Controller->Buttons.Left, Left);
                        Win32InputProcessButton(&Controller->Buttons.Right, Right);
                        Win32InputProcessButton(&Controller->Buttons.Jump, A);
                        Win32InputProcessButton(&Controller->Buttons.Attack, X);
                        Win32InputProcessButton(&Controller->Buttons.Power, RightShoulder);
                        Win32InputProcessButton(&Controller->Buttons.Start, Start);
                        
                        real32 StickX = Gamepad->sThumbLX;
                        real32 StickY = Gamepad->sThumbLY;
                        
                        if(StickX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
                        {
                            Controller->StickX = StickX / -32768.0f;
                        }
                        else if(StickX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
                        {
                            Controller->StickX = StickX / 32767.0f;
                        }
                        else
                        {
                            Controller->StickX = 0.0f;
                        }
                        
                        if(StickY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
                        {
                            Controller->StickY = StickY / -32768.0f;
                        }
                        else if(StickY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
                        {
                            Controller->StickY = StickY / 32767.0f;
                        }
                        else
                        {
                            Controller->StickY = 0.0f;
                        }
                        
                        Controller->IsAnalog = 
                        (Controller->StickX || Controller->StickY) ? true : false;
                    }
                    else
                    {
                        EngineInput.Controllers[ControllerIndex].IsConnected = false;
                    }
                }
            }
            else
            {
                EngineInput = Win32GetPlayBackInput(&WinState.ReplayStream);
            }
            
            if(WinState.ReplayStream.IsRecording)
            {
                Win32RecordInput(&WinState.ReplayStream, &EngineInput);
            }
            
            if(EngineCode.EngineOutputSound)
            {
                engine_sound EngineSound = {};
                
                EngineCode.EngineOutputSound(&EngineState, &EngineSound);
            }
            
            if(EngineCode.EngineUpdateAndRender)
            {
                engine_image EngineImageBuffer = {};
                EngineImageBuffer.Width = GlobalImageBuffer.Width;
                EngineImageBuffer.Height = GlobalImageBuffer.Height;
                EngineImageBuffer.Pitch = BYTES_PER_PIXEL*EngineImageBuffer.Width;
                EngineImageBuffer.Pixels = GlobalImageBuffer.Pixels;
                
                EngineCode.EngineUpdateAndRender(&EngineState, &EngineInput, &EngineImageBuffer);
            }
            
            win32_window_dim WindowDim = Win32GetWindowDim(Window);
            Win32CopyImageBufferToDC(&GlobalImageBuffer, WindowDC, 
                                     WindowDim.Width, WindowDim.Height);
        }
        
        int64 TicksElapsed = Win32GetTime() - BeginFrame;
        while(TicksElapsed < TargetTicksPerFrame)
        {
            int64 RemainingTicks = TargetTicksPerFrame - TicksElapsed;
            int32 RemainingMS = int32(1000.f * ((real64)RemainingTicks / (real64)GlobalTicksPerSecond));
            Sleep(RemainingMS);
            TicksElapsed = Win32GetTime() - BeginFrame;
        }
        
        real64 SecondsElapsed = (real64)TicksElapsed / (real64)GlobalTicksPerSecond;
        real64 FPS = 1.0f / SecondsElapsed;
        
#if DEBUG
        char FPSBuffer[256] = {};
        snprintf(FPSBuffer, 256, "FPS: %f \n", FPS);
        
        OutputDebugString(FPSBuffer);
#endif
    }
    
    timeEndPeriod(MSTimerGranularity);
    
    return(0);
}