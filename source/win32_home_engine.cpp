#include "home_engine_platform.h"

#include <windows.h>
#include <stdio.h>
#include <xinput.h>

#include "win32_home_engine.h"
#include "home_engine.h"

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

internal void
DEBUGWin32FillImageBuffer(win32_image_buffer *Buffer, uint32 Colour)
{
    uint32 *Pixel = (uint32 *)Buffer->Pixels;
    
    for(uint32 Y = 0;
        Y < Buffer->Height;
        ++Y)
    {
        for(uint32 X = 0;
            X < Buffer->Width;
            ++X)
        {
            *(Pixel++) = Colour;
        }
    }
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
    real32 TargetFPS = 60.0f;
    
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
    
    win32_state ProgramState = {};
    
    // TODO(stylia): Think about the DIB size
    Win32ResizeImageBuffer(&GlobalImageBuffer, 1024, 768);
    
    if(!GlobalImageBuffer.Pixels)
    {
        OutputDebugString("DIB Section could not be initialized \n");
        return(-1);
    }
    
    DEBUGWin32FillImageBuffer(&GlobalImageBuffer, 0x00FF00FF);
    
    Win32LoadXInput();
    
    // TODO(stylia): Think about minimum requirments
    mem_index PermanentMemorySize = MegaBytes(100);
    mem_index TransientMemorySize = GigaBytes(1);
    
    ProgramState.Memory = VirtualAlloc(0, PermanentMemorySize + TransientMemorySize
                                       , MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    if(!ProgramState.Memory)
    {
        OutputDebugString("Could not initialize program memory \n");
        return(-1);
    }
    
    engine_state EngineState = {};
    
    EngineState.Memory = ProgramState.Memory;
    EngineState.MemorySize = PermanentMemorySize + TransientMemorySize;
    
    engine_input EngineInput = {};
    
    GlobalRunning = true;
    while(GlobalRunning)
    {
        int64 BeginFrame = Win32GetTime();
        
        MSG Message;
        while(PeekMessageA(&Message, 0, 0, 0, PM_REMOVE))
        {
            engine_input_controller *Keyboard = &EngineInput.Keyboard;
            
            switch(Message.message)
            {
                case WM_KEYDOWN:
                case WM_SYSKEYDOWN:
                case WM_KEYUP:
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
                        
                        default:
                        {
                            TranslateMessage(&Message);
                            DispatchMessageA(&Message);
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
            
            win32_window_dim WindowDim = Win32GetWindowDim(Window);
            Win32CopyImageBufferToDC(&GlobalImageBuffer, WindowDC, 
                                     WindowDim.Width, WindowDim.Height);
            
            int64 ElapsedTicks = Win32GetTime() - BeginFrame; 
            real64 SecondsForFrame = (real64)ElapsedTicks / (real64)GlobalTicksPerSecond;
            real64 FPS = 1.0f / SecondsForFrame;
            
#if DEBUG
            char FPSBuffer[256] = {};
            snprintf(FPSBuffer, 256, "FPS: %f \n", FPS);
            
            OutputDebugString(FPSBuffer);
#endif
        }
    }
    
    return(0);
}