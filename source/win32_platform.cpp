#include "home_platform.h"

#include <windows.h>
#include <stdio.h>

#include "win32_platform.h"

global bool32 GlobalRunning;
global int64 GlobalTicksPerSecond;

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
Win32InitializeArena(mem_arena *Arena, mem_index Size)
{
    Arena->Size = Size;
    Arena->Used = 0;
    Arena->Base = VirtualAlloc(0, Size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    Assert(Arena->Base);
}

internal void
Win32FreeArena(mem_arena *Arena)
{
    if(Arena->Base)
    {
        VirtualFree(Arena->Base, 0, MEM_RELEASE);
    }
    Arena->Size = 0;
    Arena->Used = 0;
    Arena->Base = 0;
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



// TODO(stylia): Fix this
internal LRESULT CALLBACK
Win32WindowProc(HWND Window, 
                UINT Message, 
                WPARAM WParam, 
                LPARAM LParam)

{
    LRESULT Result = 0;
    switch(Message)
    {
        case WM_CREATE:
        {
            OutputDebugString("WM_CREATE\n");
        }break;
        
        case WM_ACTIVATEAPP:
        {
            OutputDebugString("WM_ACTIVATE\n");
        }break;
        
        case WM_PAINT:
        {
            /*PAINTSTRUCT Paint = {0};
            Paint.hdc = GetDC(Window);
            Paint.fErase = false;
            RECT Rect = GetPaintArea(Window);
            BeginPaint(Window, &Paint);*/
        }break;
        
        case WM_SIZE:
        {
            OutputDebugString("WM_SIZE\n");
        }break;
        
        case WM_DESTROY:
        {
            OutputDebugString("WM_DESTROY\n");
            GlobalRunning = false;
        }break;
        
        case WM_QUIT:
        {
            OutputDebugString("WM_CLOSE\n");
            GlobalRunning = false;
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
    
    // TODO(stylia): Think about the DIB size
    win32_image_buffer Win32ImageBuffer = {};
    Win32ResizeImageBuffer(&Win32ImageBuffer, 1024, 768);
    
    if(!Win32ImageBuffer.Pixels)
    {
        OutputDebugString("DIB Section could not be initialized \n");
        return(-1);
    }
    
    // TODO(stylia): Think about minimum requirments
    game_state GameState = {};
    Win32InitializeArena(&GameState.PermanentArena, MegaBytes(100));
    Win32InitializeArena(&GameState.TransientArena, GigaBytes(1));
    
    if(!GameState.PermanentArena.Base)
    {
        OutputDebugString("Could not allocate permanent memory \n");
        return(-1);
    }
    
    if(!GameState.PermanentArena.Base)
    {
        OutputDebugString("Could not allocate transient memory \n");
        return(-1);
    }
    
    DEBUGWin32FillImageBuffer(&Win32ImageBuffer, 0x00FF00FF);
    
    GlobalRunning = true;
    while(GlobalRunning)
    {
        int64 BeginFrame = Win32GetTime();
        
        // TODO(stylia): Better msg loop
        MSG Message = {0};
        TranslateMessage(&Message);
        DispatchMessageA(&Message);
        
        win32_window_dim WindowDim = Win32GetWindowDim(Window);
        
        
        Win32CopyImageBufferToDC(&Win32ImageBuffer, WindowDC, 
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
    
    Win32FreeArena(&GameState.PermanentArena);
    Win32FreeArena(&GameState.TransientArena);
    
    return(0);
}
