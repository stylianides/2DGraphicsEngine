
#include <windows.h>
#include <stdio.h>
#include <xinput.h>
#include <DSound.h>

#include "win32_engine.h"
#include "engine_platform.h"

global bool32 GlobalRunning;
global bool32 GlobalPause;
global int64 GlobalTicksPerSecond;
global win32_image GlobalImageBuffer;

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

#define DIRECT_SOUND_CREATE(Name) HRESULT WINAPI Name(LPGUID lpGuid, LPDIRECTSOUND* ppDS, LPUNKNOWN  pUnkOuter);
typedef DIRECT_SOUND_CREATE(direct_sound_create);

internal 
DEBUG_PLATFORM_READ_FILE(Win32DEBUGReadFile)
{
    file_contents Result = {};
    
    HANDLE FileHandle = CreateFileA(Filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    
    Assert(FileHandle != INVALID_HANDLE_VALUE);
    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        
        // TODO(stylia): Handle file size high
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

internal 
FILETIME Win32GetFileLastWriteTime(char *Filename)
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
Win32HandleDebug(engine_memory *Memory)
{
#if DEBUG
    OutputDebugString("DEBUG:\n");
    ClockTimer *Timers = Memory->Timers;
    
    for(uint32 Index = 0;
        Index < ArrayCount(Memory->Timers);
        ++Index)
    {
        ClockTimer *CurrentTimer = Timers + Index;
        
        if(CurrentTimer->HitCount)
        {
            char CodeSectionText[256];
            snprintf(CodeSectionText, 256, "Section %d: Total Cycles:%llu, Total Hits: %lu, AvgCycle %f \n", Index, CurrentTimer->Cycles, CurrentTimer->HitCount, (real32)CurrentTimer->Cycles/(real32)CurrentTimer->HitCount);
            OutputDebugString(CodeSectionText);
            
            CurrentTimer->Cycles = 0;
            CurrentTimer->HitCount = 0;
        }
    }
#endif
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

internal void *
Win32InitializeMemory(memory_index PermanentMemorySize, 
                      memory_index TransientMemorySize)
{
    memory_index TotalSize = PermanentMemorySize + TransientMemorySize;
    void *Result = VirtualAlloc(0, TotalSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    
    // NOTE(stylia): VirtualAlloc automatically initializes memory to zero.
    //               On other platforms this must be guaranteed.
    
    return(Result);
}

internal void
Win32StartRecording(win32_state *WinState)
{
    win32_replay_stream *ReplayStream = &WinState->ReplayStream;
    
    DeleteFile(ReplayStream->RecordFilename);
    DeleteFile(ReplayStream->MemoryFilename);
    
    ReplayStream->MemoryFile = CreateFileA(ReplayStream->MemoryFilename, GENERIC_READ|GENERIC_WRITE, 
                                           0, 0, OPEN_ALWAYS, 0, 0);
    
    Assert(ReplayStream->MemoryFile);
    
    DWORD BytesWritten = 0;
    DWORD MemorySize = (DWORD)(WinState->PermanentMemorySize + WinState->TransientMemorySize);
    WriteFile(ReplayStream->MemoryFile, WinState->Memory, MemorySize, &BytesWritten, 0);
    Assert(MemorySize == BytesWritten);
    
    CloseHandle(ReplayStream->MemoryFile);
    
    ReplayStream->RecordFile = CreateFileA(ReplayStream->RecordFilename, GENERIC_READ|GENERIC_WRITE, 
                                           0, 0, OPEN_ALWAYS, 0, 0);
    
    Assert(ReplayStream->RecordFile);
    
    ReplayStream->IsRecording = true;
    ReplayStream->RecordingIndex = 0;
}

internal void
Win32RecordInput(win32_replay_stream *ReplayStream, engine_input *Input)
{
    DWORD BytesWritten = 0;
    
#if 0
    DWORD Error = SetFilePointer(ReplayStream->RecordFile, 0, 0, FILE_END);
    Assert(Error != INVALID_SET_FILE_POINTER);
#endif
    
    WriteFile(ReplayStream->RecordFile, Input, sizeof(engine_input), &BytesWritten, 0);
    Assert(sizeof(engine_input) == BytesWritten);
    
    ReplayStream->RecordingIndex++;
}

internal void
Win32StopRecording(win32_replay_stream *ReplayStream)
{
    CloseHandle(ReplayStream->RecordFile);
    ReplayStream->IsRecording = false;
}

internal void
Win32StartPlayback(win32_replay_stream *ReplayStream)
{
    ReplayStream->MemoryFile = CreateFileA(ReplayStream->MemoryFilename, GENERIC_READ, FILE_SHARE_READ, 
                                           0, OPEN_EXISTING, 0, 0);
    
    ReplayStream->RecordFile = 
        CreateFileA(ReplayStream->RecordFilename, GENERIC_READ, FILE_SHARE_READ, 
                    0, OPEN_EXISTING, 0, 0);
    
    Assert(ReplayStream->MemoryFile);
    Assert(ReplayStream->RecordFile);
    
    ReplayStream->PlayingIndex = 0;
    ReplayStream->IsPlayingBack = true;
}

internal engine_input
Win32GetPlayBackInput(win32_state *WinState)
{
    engine_input Result = {};
    win32_replay_stream *ReplayStream = &WinState->ReplayStream;
    
    if(ReplayStream->IsPlayingBack)
    {
        
        DWORD BytesRead = 0;
        DWORD Error;
        
        if(ReplayStream->PlayingIndex == 0)
        {
            DWORD MemorySize = (DWORD)(WinState->PermanentMemorySize + WinState->TransientMemorySize);
            
            Error = SetFilePointer(ReplayStream->MemoryFile, 0, 0, FILE_BEGIN);
            Assert(Error != INVALID_SET_FILE_POINTER);
            
            ReadFile(ReplayStream->MemoryFile, WinState->Memory, MemorySize, &BytesRead, 0);
            Assert(BytesRead == MemorySize);
        }
        
        
        Error = SetFilePointer(ReplayStream->RecordFile, 
                               sizeof(engine_input)*ReplayStream->PlayingIndex, 
                               0, FILE_BEGIN);
        Assert(Error != INVALID_SET_FILE_POINTER);
        
        BytesRead = 0;
        Error = ReadFile(ReplayStream->RecordFile, &Result, 
                         sizeof(engine_input), &BytesRead, 0);
        
        Assert(BytesRead == sizeof(engine_input));
        
        ReplayStream->PlayingIndex++;
        
        if(ReplayStream->PlayingIndex == ReplayStream->RecordingIndex)
        {
            ReplayStream->PlayingIndex = 0;
        }
    }
    
    return(Result);
}

// TODO(stylia): Maybe put a playback button seperately 
//               so after recording ends you play back whenever you want
//               If i opt in on this, then deleting files here is no good.
internal void
Win32StopPlayback(win32_replay_stream *ReplayStream)
{
    CloseHandle(ReplayStream->MemoryFile);
    CloseHandle(ReplayStream->RecordFile);
    DeleteFile(ReplayStream->MemoryFilename);
    DeleteFile(ReplayStream->RecordFilename);
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

internal real32
Win32InputProcessStickValue(int32 StickValueRaw)
{
    real32 Result = 0;
    
    if(StickValueRaw < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
    {
        Result = StickValueRaw / -32768.0f;
    }
    else if(StickValueRaw > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
    {
        Result = StickValueRaw / 32767.0f;
    }
    else
    {
        Result = 0.0f;
    }
    
    return(Result);
}

internal void
Win32InputProcessButton(engine_input_button *Button,  bool32 Pressed)
{
    Button->Press = (Pressed) ? Button->Press + 1 : 0;
}

internal
int64 Win32GetTimeStamp()
{
    LARGE_INTEGER Ticks = {};
    
    QueryPerformanceCounter(&Ticks);
    Assert(Ticks.QuadPart != 0);
    
    return(Ticks.QuadPart);
}

internal inline
real32 Win32GetSecondsElapsed(int64 TimeStampA, int64 TimeStampB)
{
    real32 Result = 
    (TimeStampA > TimeStampB) ? 
    (real32)((real32)(TimeStampA - TimeStampB) / (real32)GlobalTicksPerSecond):
    (real32)((real32)(TimeStampB - TimeStampA) / (real32)GlobalTicksPerSecond);
    
    return(Result);
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
Win32ResizeImageBuffer(win32_image *Buffer, uint32 Width, uint32 Height)
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
    Buffer->BmpInfo.bmiHeader.biHeight = (int32)Height; 
    Buffer->BmpInfo.bmiHeader.biPlanes = 1;
    Buffer->BmpInfo.bmiHeader.biBitCount = 8*BYTES_PER_PIXEL;
    Buffer->BmpInfo.bmiHeader.biCompression = BI_RGB;
    Buffer->BmpInfo.bmiHeader.biSize = sizeof(Buffer->BmpInfo.bmiHeader);
    Buffer->Pitch = Width*BYTES_PER_PIXEL;
    
    memory_index BmpSize = BYTES_PER_PIXEL*Width*Height;
    
    Buffer->Pixels = VirtualAlloc(0, BmpSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    Assert(Buffer->Pixels);
}

internal void 
Win32CopyImageBufferToDC(win32_image *Buffer, 
                         HDC DC, uint32 DCWidth, uint32 DCHeight, 
                         uint32 OffsetX = 10, uint32 OffsetY = 10)
{
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

internal void
Win32InitDirectSound(HWND Window, win32_sound *WinSound, 
                     int32 SamplesPerSecond, int16 Channels, int16 BytesPerSample)
{
    HMODULE DS = LoadLibraryA("dsound.dll");
    
    if(DS)
    {
        direct_sound_create *DSCreate = (direct_sound_create *)GetProcAddress(DS, "DirectSoundCreate");
        if(DSCreate)
        {
            if(DSCreate(0, &WinSound->DirectSound, 0) == DS_OK)
            {
                LPDIRECTSOUND DirectSound = WinSound->DirectSound;
                if(DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY) == DS_OK)
                {
                    DSBUFFERDESC BufferDesc;
                    BufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
                    BufferDesc.dwBufferBytes = 0;
                    BufferDesc.dwReserved = 0;
                    BufferDesc.lpwfxFormat = 0;
                    BufferDesc.guid3DAlgorithm = GUID_NULL;
                    BufferDesc.dwSize = sizeof(BufferDesc);
                    
                    if(DirectSound->CreateSoundBuffer(&BufferDesc, 
                                                      &WinSound->PrimaryBuffer, 0) == DS_OK)
                    {
                        WinSound->IsValid = true;
                    }
                    else
                    {
                        OutputDebugString("Win32InitDirectSound: Could not create primary sound buffer\n");
                        WinSound->IsValid = false;
                    }
                }
                else
                {
                    OutputDebugString("Win32InitDirectSound: Could not set CooperativeLevel\n");
                    WinSound->IsValid = false;
                }
            }
            else
            {
                OutputDebugString("Win32InitDirectSound: DirectSoundCreate failed\n");
                WinSound->IsValid = false;
            }
        }
        else
        {
            OutputDebugString("Win32InitDirectSound: Could not load function DirectSoundCreate\n");
            WinSound->IsValid = false;
        }
    }
    else
    {
        OutputDebugString("Win32InitDirectSound: Could not load dsound.dll\n");
        WinSound->IsValid = false;
    }
}

internal void
Win32CopySoundSamples(void *SampleBuffer, uint32 SampleBufferSize,
                      win32_sound_buffer *SoundBuffer,
                      DWORD ByteToLock, DWORD LockSize)
{
    void *Region1;
    void *Region2;
    DWORD Region1Size;
    DWORD Region2Size;
    
    if(SoundBuffer->DS->Lock(ByteToLock, LockSize, 
                             &Region1, &Region1Size,
                             &Region2, &Region2Size, 
                             0) == DS_OK)
    {
        
        uint32 SamplesToCopy = Region1Size / SoundBuffer->BlockAlign;
        
        uint16 *Source = (uint16 *)SampleBuffer;
        uint16 *Dest = (uint16 *)Region1;
        
        for(uint32 SampleIndex = 0;
            SampleIndex < SamplesToCopy;
            ++SampleIndex)
        {
            *Dest++ = *Source++;
            *Dest++ = *Source++;
            SoundBuffer->RunningSampleIndex++;
        }
        
        SamplesToCopy = Region2Size / SoundBuffer->BlockAlign;
        
        Dest = (uint16 *)Region2;
        
        for(uint32 SampleIndex = 0;
            SampleIndex < SamplesToCopy;
            ++SampleIndex)
        {
            *Dest++ = *Source++;
            *Dest++ = *Source++;
            SoundBuffer->RunningSampleIndex++;
        }
        
        if(SoundBuffer->DS->Unlock(Region1, Region1Size, Region2, Region2Size) != DS_OK)
        {
            OutputDebugString("Win32CopySoundSamples: Could not unlock the sound buffer\n");
        }
    }
    else
    {
        OutputDebugString("Win32CopySoundSamples: Could not lock the sound buffer\n");
    }
}

internal void
Win32ClearSoundBuffer(win32_sound_buffer *SoundBuffer)
{
    void *Region1;
    void *Region2;
    DWORD Region1Size;
    DWORD Region2Size;
    
    if(SoundBuffer->DS->Lock(0, 0, 
                             &Region1, &Region1Size,
                             &Region2, &Region2Size, 
                             DSBLOCK_ENTIREBUFFER) == DS_OK)
    {
        uint8 *Dest = (uint8 *)Region1;
        
        for(uint32 SizeIndex = 0;
            SizeIndex < Region1Size;
            ++SizeIndex)
        {
            *Dest++ = 0;
        }
        
        Dest = (uint8 *)Region2;
        
        for(uint32 SizeIndex = 0;
            SizeIndex < Region2Size;
            ++SizeIndex)
        {
            *Dest++ = 0;
        }
        
        if(SoundBuffer->DS->Unlock(Region1, Region1Size, Region2, Region2Size) != DS_OK)
        {
            OutputDebugString("Win32ClearSoundBuffer: Could not unlock the sound buffer\n");
        }
    }
    else
    {
        OutputDebugString("Win32ClearSoundBuffer: Could not lock the sound buffer\n");
    }
}

internal void
Win32InitDirectSoundBuffer(win32_sound *WinSound, win32_sound_buffer *SoundBuffer, 
                           int32 SamplesPerSecond, int16 Channels, int16 BytesPerSample,
                           int32 BufferSize)
{
    SoundBuffer->BytesPerSample = BytesPerSample;
    SoundBuffer->Channels = Channels;
    SoundBuffer->SamplesPerSecond = SamplesPerSecond;
    SoundBuffer->Size = BufferSize; 
    SoundBuffer->BlockAlign = Channels * BytesPerSample;
    
    WAVEFORMATEX WaveFormat = {};
    WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
    WaveFormat.wBitsPerSample = BytesPerSample * 8;
    WaveFormat.nChannels = Channels;
    WaveFormat.nSamplesPerSec = SamplesPerSecond;
    WaveFormat.nBlockAlign = Channels * BytesPerSample;
    WaveFormat.nAvgBytesPerSec = WaveFormat.nBlockAlign * SamplesPerSecond;
    WaveFormat.cbSize = 0;
    
    DSBUFFERDESC BufferDesc;
    BufferDesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2;
    BufferDesc.dwBufferBytes = BufferSize;
    BufferDesc.dwReserved = 0;
    BufferDesc.lpwfxFormat = &WaveFormat;
    BufferDesc.guid3DAlgorithm = DS3DALG_DEFAULT;
    BufferDesc.dwSize = sizeof(BufferDesc);
    
    if(WinSound->DirectSound->CreateSoundBuffer(&BufferDesc, 
                                                &SoundBuffer->DS, 0) == DS_OK)
    {
        SoundBuffer->IsValid = true;
        OutputDebugString("Created secondary sound buffer\n");
    }
    else
    {
        SoundBuffer->IsValid = false;
        OutputDebugString("Could not create secondary sound buffer\n");
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
        
        case WM_DESTROY:
        case WM_QUIT:
        {
            GlobalPause = true;
            GlobalRunning = false;
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
    real32 TargetSecondsPerFrame = 1.0f / TargetFPS;
    int64 TargetTicksPerFrame = int64(TargetSecondsPerFrame * GlobalTicksPerSecond);
    
    // NOTE(stylia): Set the best granularity for timers, 
    //               so the task scheduler can be more accurate
    //               waking up the app after it sleeps
    uint32 MSTimerGranularity = 1;
    bool32 IsSleepGranular = true;
    
    Win32LoadXInput();
    
    if(timeBeginPeriod(MSTimerGranularity) == TIMERR_NOCANDO)
    {
        IsSleepGranular = false;
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
    //               I will keep increasing this if needed
    WinState.PermanentMemorySize = MegaBytes(100);
    WinState.TransientMemorySize = MegaBytes(300);
    WinState.Memory = Win32InitializeMemory(WinState.PermanentMemorySize, WinState.TransientMemorySize);
    
    if(!WinState.Memory)
    {
        OutputDebugString("Could not initialize program memory\n");
        return(-1);
    }
    
    // TODO(stylia): Multiple replay streams and memory maps
    Win32ConcatString(WinState.ReplayStream.RecordFilename, MAX_PATH, ".\\engine_1.hei");
    Win32ConcatString(WinState.ReplayStream.MemoryFilename, MAX_PATH, ".\\engine.mem");
    
    int16 Channels = 2;
    int32 SamplesPerSecond = 48000;
    int16 BytesPerSample = 2;
    int16 BlockAlign = Channels * BytesPerSample;
    
    win32_sound WinSound = {};
    Win32InitDirectSound(Window, &WinSound, SamplesPerSecond, Channels, BytesPerSample);
    
    win32_sound_buffer *SoundBuffer = &WinSound.SecondaryBuffer;
    
    Win32InitDirectSoundBuffer(&WinSound, SoundBuffer, 
                               SamplesPerSecond, Channels, BytesPerSample, BlockAlign * SamplesPerSecond);
    
    Win32ClearSoundBuffer(SoundBuffer);
    
    if(SoundBuffer->DS->Play(0, 0, DSBPLAY_LOOPING) != DS_OK)
    {
        OutputDebugString("Direct Sound buffer cannot play\n");
        SoundBuffer->IsValid = false;
    }
    
    WinSound.SizeOfSamples = SamplesPerSecond * BlockAlign;
    WinSound.Samples = VirtualAlloc(0, WinSound.SizeOfSamples, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if(!WinSound.Samples)
    {
        OutputDebugString("WinMain: Could not allocate samples \n");
        WinSound.IsValid = false;
    }
    
    win32_engine_code EngineCode = {};
    GetCurrentDirectory(256, EngineCode.EngineDLLPath);
    Win32ContructDLLPath(EngineCode.EngineDLLPath, 256, "\\build\\engine.dll");
    Win32ContructDLLPath(EngineCode.EngineDLLPath_Loaded, 256, "\\build\\engine_loaded.dll");
    Win32ReloadEngineDLL(&EngineCode);
    
    // TODO(stylia): Think about the DIB size
    Win32ResizeImageBuffer(&GlobalImageBuffer, 1024, 768);
    
    if(!GlobalImageBuffer.Pixels)
    {
        OutputDebugString("DIB Section could not be initialized \n");
        return(-1);
    }
    
    engine_memory *EngineMemory = (engine_memory *)WinState.Memory;
    EngineMemory->PermanentMemorySize = WinState.PermanentMemorySize;
    EngineMemory->TransientMemorySize = WinState.TransientMemorySize;
    EngineMemory->DEBUGPlatformReadFile = Win32DEBUGReadFile;
    EngineMemory->DEBUGPlatformWriteFile = Win32DEBUGWriteFile;
    EngineMemory->DEBUGPlatformFreeFile = Win32DEBUGFreeFile;
    
    engine_input EngineInput = {};
    // NOTE(stylia): Game Loop
    GlobalRunning = true;
    while(GlobalRunning)
    {
        int64 BeginFrameTS = Win32GetTimeStamp();
        FILETIME DLLWriteTimeNow = Win32GetFileLastWriteTime(EngineCode.EngineDLLPath); 
        
        if(CompareFileTime(&DLLWriteTimeNow, &EngineCode.EngineDLL_LastWriteTime) == FILE_TIME_LATER)
        {
            Win32ReloadEngineDLL(&EngineCode);
        }
        
        EngineInput.dt = TargetSecondsPerFrame;
        
        engine_input_controller *Keyboard = &EngineInput.Controllers[0];
        // TODO(stylia): Temporary
        Keyboard->IsConnected = true;
        
        MSG Message;
        while(PeekMessageA(&Message, 0, 0, 0, PM_REMOVE))
        {
            bool32 IsAltDown = ((Message.lParam >> 31) && 1);
            
            switch(Message.message)
            {
                case WM_KEYDOWN:
                case WM_SYSKEYDOWN:
                case WM_KEYUP:
                case WM_SYSKEYUP:
                {
                    bool32 IsDown = ((Message.message == WM_KEYUP) || (Message.message == WM_SYSKEYUP)) ? false : true;
                    
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
                        // TODO(stylia): make it so this is first keyboard input after it
                        case L_KEY:
                        {
                            if(IsDown)
                            {
                                if(!WinState.ReplayStream.IsPlayingBack)
                                {
                                    if(!WinState.ReplayStream.IsRecording)
                                    {
                                        Win32StartRecording(&WinState);
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
                        
                        case F4:
                        {
                            if(IsAltDown)
                            {
                                GlobalPause = true;
                                GlobalRunning = false;
                            }
                        }break;
                    }
                }break;
                
                default:
                {
                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                }break;
            }
        }
        
        // TODO(stylia): Global Pause needs to be thought out.
        // Think about what should be paused. Will this be only for debug??
        
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
                        EngineInput.Controllers[ControllerIndex + 1].IsConnected = true;
                        
                        XINPUT_GAMEPAD *Gamepad = &ControllerState.Gamepad;
                        
                        bool32 Up = Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_UP;
                        bool32 Down = Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
                        bool32 Left = Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
                        bool32 Right = Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
                        bool32 A = Gamepad->wButtons & XINPUT_GAMEPAD_A;
                        bool32 X = Gamepad->wButtons & XINPUT_GAMEPAD_X;
                        bool32 RightShoulder = Gamepad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
                        bool32 Start = Gamepad->wButtons & XINPUT_GAMEPAD_START;
                        
                        engine_input_controller *Controller = &EngineInput.Controllers[ControllerIndex + 1];
                        Win32InputProcessButton(&Controller->Buttons.Up, Up);
                        Win32InputProcessButton(&Controller->Buttons.Down, Down);
                        Win32InputProcessButton(&Controller->Buttons.Left, Left);
                        Win32InputProcessButton(&Controller->Buttons.Right, Right);
                        Win32InputProcessButton(&Controller->Buttons.Jump, A);
                        Win32InputProcessButton(&Controller->Buttons.Attack, X);
                        Win32InputProcessButton(&Controller->Buttons.Power, RightShoulder);
                        Win32InputProcessButton(&Controller->Buttons.Start, Start);
                        
                        Controller->StickX = Win32InputProcessStickValue(Gamepad->sThumbLX);
                        Controller->StickY = Win32InputProcessStickValue(Gamepad->sThumbLY);
                        
                        Controller->IsAnalog = 
                        (Controller->StickX || Controller->StickY) ? true : false;
                    }
                    else
                    {
                        EngineInput.Controllers[ControllerIndex + 1].IsConnected = false;
                    }
                }
            }
            else
            {
                EngineInput = Win32GetPlayBackInput(&WinState);
            }
            
            if(WinState.ReplayStream.IsRecording)
            {
                Win32RecordInput(&WinState.ReplayStream, &EngineInput);
            }
            
            if(EngineCode.EngineOutputSound && SoundBuffer->IsValid)
            {
                real32 AudioSecondsElapsed = Win32GetSecondsElapsed(BeginFrameTS, Win32GetTimeStamp());
                
                real32 FrameFlipSecondsRemaining = TargetSecondsPerFrame - AudioSecondsElapsed;
                
                DWORD WriteCursor;
                DWORD PlayCursor;
                
                if(SoundBuffer->DS->GetCurrentPosition(&PlayCursor, &WriteCursor) == DS_OK)
                {
                    if(!SoundBuffer->IsInitialized)
                    {
                        SoundBuffer->RunningSampleIndex = WriteCursor / SoundBuffer->BlockAlign;
                        SoundBuffer->IsInitialized = true;
                    }
                    
                    DWORD PlayCursorAtFrameFlip = PlayCursor + DWORD(FrameFlipSecondsRemaining * 
                                                                     SamplesPerSecond * BlockAlign);
                    
                    bool32 AudioCanSynchronizeWithFrameFlip;
                    
                    if(WriteCursor > PlayCursor){
                        AudioCanSynchronizeWithFrameFlip = (WriteCursor < PlayCursorAtFrameFlip);
                    }
                    else
                    {
                        AudioCanSynchronizeWithFrameFlip = 
                        ((WriteCursor + SoundBuffer->Size) < PlayCursorAtFrameFlip);
                    }
#if 0
                    DWORD ByteToLock;
                    
                    if(AudioCanSynchronizeWithFrameFlip)
                    {
                        ByteToLock = PlayCursorAtFrameFlip;
                    }
                    else
                    {
                        ByteToLock = (SoundBuffer->RunningSampleIndex * SoundBuffer->BlockAlign) % SoundBuffer->Size;
                    }
                    
#else
                    DWORD ByteToLock = (SoundBuffer->RunningSampleIndex * SoundBuffer->BlockAlign) % SoundBuffer->Size;
#endif
                    
                    DWORD LockSize = 0;
                    if(ByteToLock > PlayCursor)
                    {
                        LockSize = PlayCursor + (SoundBuffer->Size - ByteToLock);
                    }
                    else if(ByteToLock < PlayCursor)
                    {
                        LockSize = (PlayCursor - ByteToLock);
                    }
                    else
                    {
                        // TODO(stylia): See why this is triggered when hotloading
                        //InvalidCodePath;
                    }
                    
                    
                    engine_sound EngineSound = {};
                    EngineSound.Hz = 440;
                    EngineSound.SampleBuffer = (int16 *)WinSound.Samples;
                    EngineSound.SampleBufferSize = WinSound.SizeOfSamples;
                    EngineSound.SamplesPerSecond = SamplesPerSecond;
                    EngineSound.SampleCount = LockSize / SoundBuffer->BlockAlign;
                    EngineSound.SampleSize = BlockAlign;
                    
                    EngineCode.EngineOutputSound(EngineMemory, &EngineSound);
                    
                    Win32CopySoundSamples(WinSound.Samples, WinSound.SizeOfSamples, 
                                          SoundBuffer, ByteToLock, LockSize);
                    
                }
            }
            
            if(EngineCode.EngineUpdateAndRender)
            {
                engine_image EngineImageBuffer = {};
                EngineImageBuffer.Width = GlobalImageBuffer.Width;
                EngineImageBuffer.Height = GlobalImageBuffer.Height;
                EngineImageBuffer.Pitch = BYTES_PER_PIXEL*EngineImageBuffer.Width;
                EngineImageBuffer.Pixels = GlobalImageBuffer.Pixels;
                
                EngineCode.EngineUpdateAndRender(EngineMemory, 
                                                 &EngineInput, 
                                                 &EngineImageBuffer);
                
                Win32HandleDebug(EngineMemory);
            }
            
        }
        
        real32 SecondsElapsed = Win32GetSecondsElapsed(BeginFrameTS, Win32GetTimeStamp());
        
        if(SecondsElapsed < TargetSecondsPerFrame)
        {
            if(IsSleepGranular)
            {
                int32 RemainingMS = int32(1000.f * (TargetSecondsPerFrame - SecondsElapsed));
                if(RemainingMS > 0)
                {
                    Sleep(RemainingMS);
                }
            }
            
            SecondsElapsed = Win32GetSecondsElapsed(BeginFrameTS, Win32GetTimeStamp());
            
            while(SecondsElapsed < TargetSecondsPerFrame)
            {
                SecondsElapsed = Win32GetSecondsElapsed(BeginFrameTS, Win32GetTimeStamp());
            }
        }
        else
        {
            // TODO(stylia): Missed Frame, log it
            OutputDebugString("Missed Frame \n");
        }
        
        win32_window_dim WindowDim = Win32GetWindowDim(Window);
        Win32CopyImageBufferToDC(&GlobalImageBuffer, WindowDC, 
                                 WindowDim.Width, WindowDim.Height);
        
        real64 FPS = 1.0f / SecondsElapsed;
        
#if DEBUG
        char FPSBuffer[256] = {};
        snprintf(FPSBuffer, 256, "FPS: %f \n", FPS);
        
        OutputDebugString(FPSBuffer);
#endif
    }
    
    if(WinState.Memory)
    {
        if(!VirtualFree(WinState.Memory, 0, MEM_RELEASE))
        {
            OutputDebugString("Exit: Memory allocated could not be freed.\n");
            return(-1);
        }
    }
    
    if(WinSound.Samples)
    {
        if(!VirtualFree(WinSound.Samples, 0, MEM_RELEASE))
        {
            OutputDebugString("Exit: Memory allocated for samples could not be freed.\n");
        }
    }
    
    if(GlobalImageBuffer.Pixels)
    {
        if(!VirtualFree(GlobalImageBuffer.Pixels, 0, MEM_RELEASE))
        {
            OutputDebugString("Exit: Memory allocated for image buffer could not be freed.\n");
        }
    }
    
    timeEndPeriod(MSTimerGranularity);
    
    return(0);
}