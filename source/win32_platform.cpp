#include <windows.h>
#include "win32_platform.h"

global int GlobalRunning;

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

internal void Win32ResizeDIBSection(win32_image_buffer *Buffer, int32 Width, int32 Height)
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
    
    // NOTE(stylia): Negative means top down
    Buffer->BmpInfo.bmiHeader.biHeight = -Height; 
    Buffer->BmpInfo.bmiHeader.biPlanes = 1;
    Buffer->BmpInfo.bmiHeader.biBitCount = 8*BYTES_PER_PIXEL;
    Buffer->BmpInfo.bmiHeader.biCompression = BI_RGB;
    Buffer->BmpInfo.bmiHeader.biSize = sizeof(Buffer->BmpInfo.bmiHeader);
    Buffer->Pitch = Width*BYTES_PER_PIXEL;
    
    memory_index BmpSize = BYTES_PER_PIXEL*Width*Height;
    
    Buffer->Pixels = VirtualAlloc(0, BmpSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

internal void Win32CopyImageBufferToDC(win32_image_buffer *Buffer, HDC DeviceContext,
                                       int32 WindowWidth, int32 WindowHeight)
{
    PatBlt(DeviceContext, 0, 0, WindowWidth, WindowHeight, BLACKNESS);
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
    LARGE_INTEGER PerfFrequency;
    if(!QueryPerformanceFrequency(&PerfFrequency))
    {
        // TODO(stylia): Older system failure?
        DWORD Error = GetLastError();
        OutputDebugString("Could get performance frequency \n");
        return(-1);
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
        // TODO(stylia): Think about error handling
        DWORD Error = GetLastError();
        OutputDebugString("Could not register class: Error \n");
        return(-1);
    }
    
    // TODO(stylia): Think about the window size
    // Target Res is 1024x768 
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
    Win32ResizeDIBSection(&Win32ImageBuffer, 1024, 768);
    
    if(!Win32ImageBuffer.Pixels)
    {
        OutputDebugString("DIB Section could not be initialized \n");
        return(-1);
    }
    
    GlobalRunning = true;
    while(GlobalRunning)
    {
        // TODO(stylia): Better msg loop
        MSG Message = {0};
        TranslateMessage(&Message);
        DispatchMessageA(&Message);
        
        win32_window_dim WindowDim = Win32GetWindowDim(Window);
        
        Win32CopyImageBufferToDC(&Win32ImageBuffer, WindowDC, 
                                 WindowDim.Width, WindowDim.Height);
    }
    
    return(0);
}
