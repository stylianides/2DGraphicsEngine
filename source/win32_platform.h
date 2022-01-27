
#ifndef WIN32_PLATFORM_H
#define WIN32_PLATFORM_H


struct win32_window_dim
{
    int32 Width;
    int32 Height;
};

struct win32_image_buffer
{
    BITMAPINFO BmpInfo;
    
    uint32 Width;
    uint32 Height;
    
    // TODO(stylia): think about this, do we need pitch?
    int32 Pitch; 
    
    void *Pixels;
};

#endif //WIN32_PLATFORM_H
