/*
//
// Copyright (c) 2009-2010 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

// Source altered and distributed from https://github.com/AdrienHerubel/imgui
*/
#ifndef IMGUI_H
#define IMGUI_H

#ifdef __cplusplus
extern "C" {
#endif

#define IMGUI_TRUE  1
#define IMGUI_FALSE 0

    typedef unsigned char imguiBool;

    enum imguiMouseButton
    {
        IMGUI_MBUT_LEFT = 0x01, 
        IMGUI_MBUT_RIGHT = 0x02, 
    };

    enum imguiTextAlign
    {
        IMGUI_ALIGN_LEFT,
        IMGUI_ALIGN_CENTER,
        IMGUI_ALIGN_RIGHT,
    };

    static unsigned int imguiRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        return (r) | (g << 8) | (b << 16) | (a << 24);
    }

    void imguiInit();
    void imguiBeginFrame(int mx, int my, unsigned char mbut, int scroll);
    void imguiEndFrame();

    imguiBool imguiBeginScrollArea(const char* name, int x, int y, int w, int h, int* scroll);
    void imguiEndScrollArea();

    void imguiIndent();
    void imguiUnindent();
    void imguiSeparator();
    void imguiSeparatorLine();

    imguiBool imguiButton(const char* text, imguiBool enabled);
    imguiBool imguiItem(const char* text, imguiBool enabled);
    imguiBool imguiCheck(const char* text, imguiBool checked, imguiBool enabled);
    imguiBool imguiCollapse(const char* text, const char* subtext, imguiBool checked, imguiBool enable);
    void imguiLabel(const char* text);
    void imguiValue(const char* text);
    imguiBool imguiSlider(const char* text, float* val, float vmin, float vmax, float vinc, imguiBool enabled);

    void imguiDrawText(int x, int y, int align, const char* text, unsigned int color);
    void imguiDrawLine(float x0, float y0, float x1, float y1, float r, unsigned int color);
    void imguiDrawRoundedRect(float x, float y, float w, float h, float r, unsigned int color);
    void imguiDrawRect(float x, float y, float w, float h, unsigned int color);

    /* // Pull render interface. */
    enum imguiGfxCmdType
    {
        IMGUI_GFXCMD_RECT,
        IMGUI_GFXCMD_TRIANGLE,
        IMGUI_GFXCMD_LINE,
        IMGUI_GFXCMD_TEXT,
        IMGUI_GFXCMD_SCISSOR,
    };

    struct imguiGfxRect
    {
        short x,y,w,h,r;
    };

    struct imguiGfxText
    {
        short x,y,align;
        const char* text;
    };

    struct imguiGfxLine
    {
        short x0,y0,x1,y1,r;
    };

    struct imguiGfxCmd
    {
        char type;
        char flags;
        char pad[2];
        unsigned int col;
        union
        {
            struct imguiGfxLine line;
            struct imguiGfxRect rect;
            struct imguiGfxText text;
        };
    };

    const struct imguiGfxCmd * imguiGetRenderQueue();
    int imguiGetRenderQueueSize();

#ifdef __cplusplus
}
#endif


#endif/* // IMGUI_H*/
