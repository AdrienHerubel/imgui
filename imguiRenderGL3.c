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

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>

#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

#include "imgui.h"

// Some math headers don't have PI defined.
static const float PI = 3.14159265f;
/*
  void imguifree(void* ptr);
  void* imguimalloc(size_t size);

  #define STBTT_malloc(x,y)    imguimalloc(x,y)
  #define STBTT_free(x,y)      imguifree(x,y)*/
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
/*
  void imguifree(void* ptr)
  {
  free(ptr);
  }

  void* imguimalloc(size_t size)
  {
  return malloc(size);
  }
*/
#define TEMP_COORD_COUNT 100
#define CIRCLE_VERTS 32

static float g_tempCoords[TEMP_COORD_COUNT*2];
static float g_tempNormals[TEMP_COORD_COUNT*2];
static float g_tempVertices[TEMP_COORD_COUNT * 12 + (TEMP_COORD_COUNT - 2) * 6];
static float g_tempTextureCoords[TEMP_COORD_COUNT * 12 + (TEMP_COORD_COUNT - 2) * 6];
static float g_tempColors[TEMP_COORD_COUNT * 24 + (TEMP_COORD_COUNT - 2) * 12];

static float g_circleVerts[CIRCLE_VERTS*2];

static stbtt_bakedchar g_cdata[96]; // ASCII 32..126 is 95 glyphs
static GLuint g_ftex = 0;
static GLuint g_whitetex = 0;
static GLuint g_vao = 0;
static GLuint g_vbos[3] = {0, 0, 0};
static GLuint g_program = 0;
static GLuint g_programViewportLocation = 0;
static GLuint g_programTextureLocation = 0;

unsigned int RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
return (r) | (g << 8) | (b << 16) | (a << 24);
}

static void drawPolygon(const float* coords, unsigned numCoords, float r, unsigned int col)
{
int i, j;
float colf[4]      = { (float) (col&0xff) / 255.f, (float) ((col>>8)&0xff) / 255.f, (float) ((col>>16)&0xff) / 255.f, (float) ((col>>24)&0xff) / 255.f};
float colTransf[4] = { (float) (col&0xff) / 255.f, (float) ((col>>8)&0xff) / 255.f, (float) ((col>>16)&0xff) / 255.f, 0};
    
int vSize;
int uvSize;
int cSize;
float * v;
float * uv;
float * c;
float * ptrV;
float * ptrC;

if (numCoords > TEMP_COORD_COUNT) numCoords = TEMP_COORD_COUNT;
for ( i = 0, j = numCoords-1; i < numCoords; j=i++)
    {
const float* v0 = &coords[j*2];
const float* v1 = &coords[i*2];
float dx = v1[0] - v0[0];
float dy = v1[1] - v0[1];
float d = sqrtf(dx*dx+dy*dy);
if (d > 0)
    {
d = 1.0f/d;
dx *= d;
dy *= d;
}
g_tempNormals[j*2+0] = dy;
g_tempNormals[j*2+1] = -dx;
}

    
for ( i = 0, j = numCoords-1; i < numCoords; j=i++)
    {
float dlx0 = g_tempNormals[j*2+0];
float dly0 = g_tempNormals[j*2+1];
float dlx1 = g_tempNormals[i*2+0];
float dly1 = g_tempNormals[i*2+1];
float dmx = (dlx0 + dlx1) * 0.5f;
float dmy = (dly0 + dly1) * 0.5f;
float   dmr2 = dmx*dmx + dmy*dmy;
if (dmr2 > 0.000001f)
    {
float   scale = 1.0f / dmr2;
if (scale > 10.0f) scale = 10.0f;
dmx *= scale;
dmy *= scale;
}
g_tempCoords[i*2+0] = coords[i*2+0]+dmx*r;
g_tempCoords[i*2+1] = coords[i*2+1]+dmy*r;
}

vSize  = numCoords * 12 + (numCoords - 2) * 6;
uvSize = numCoords * 2 * 6 + (numCoords - 2) * 2 * 3;
cSize  = numCoords * 4 * 6 + (numCoords - 2) * 4 * 3;
v  = g_tempVertices;
uv = g_tempTextureCoords;
memset(uv, 0, uvSize * sizeof(float));
c = g_tempColors;
memset(c, 1, cSize * sizeof(float));

ptrV = v;
ptrC = c;

for ( i = 0, j = numCoords-1; i < numCoords; j=i++)
    {
*ptrV = coords[i*2];
*(ptrV+1) = coords[i*2 + 1];
ptrV += 2;
*ptrV = coords[j*2];
*(ptrV+1) = coords[j*2 + 1];
ptrV += 2;
*ptrV = g_tempCoords[j*2];
*(ptrV+1) = g_tempCoords[j*2 + 1];
ptrV += 2;
*ptrV = g_tempCoords[j*2];
*(ptrV+1) = g_tempCoords[j*2 + 1];
ptrV += 2;
*ptrV = g_tempCoords[i*2];
*(ptrV+1) = g_tempCoords[i*2 + 1];
ptrV += 2;
*ptrV = coords[i*2];
*(ptrV+1) = coords[i*2 + 1];
ptrV += 2;

*ptrC = colf[0];
*(ptrC+1) = colf[1];
*(ptrC+2) = colf[2];
*(ptrC+3) = colf[3];
ptrC += 4;
*ptrC = colf[0];
*(ptrC+1) = colf[1];
*(ptrC+2) = colf[2];
*(ptrC+3) = colf[3];
ptrC += 4;
*ptrC = colTransf[0];
*(ptrC+1) = colTransf[1];
*(ptrC+2) = colTransf[2];
*(ptrC+3) = colTransf[3];
ptrC += 4;
*ptrC = colTransf[0];
*(ptrC+1) = colTransf[1];
*(ptrC+2) = colTransf[2];
*(ptrC+3) = colTransf[3];
 ptrC += 4;
 *ptrC = colTransf[0];
 *(ptrC+1) = colTransf[1];
 *(ptrC+2) = colTransf[2];
 *(ptrC+3) = colTransf[3];
 ptrC += 4;
 *ptrC = colf[0];
 *(ptrC+1) = colf[1];
 *(ptrC+2) = colf[2];
 *(ptrC+3) = colf[3];
 ptrC += 4;
    }

 for ( i = 2; i < numCoords; ++i)
     {
         *ptrV = coords[0];
         *(ptrV+1) = coords[1];
         ptrV += 2;
         *ptrV = coords[(i-1)*2];
         *(ptrV+1) = coords[(i-1)*2+1];
         ptrV += 2;
         *ptrV = coords[i*2];
         *(ptrV+1) = coords[i*2 + 1];
         ptrV += 2;

         *ptrC = colf[0];
         *(ptrC+1) = colf[1];
         *(ptrC+2) = colf[2];
         *(ptrC+3) = colf[3];
         ptrC += 4;
         *ptrC = colf[0];
         *(ptrC+1) = colf[1];
         *(ptrC+2) = colf[2];
         *(ptrC+3) = colf[3];
         ptrC += 4;
         *ptrC = colf[0];
         *(ptrC+1) = colf[1];
         *(ptrC+2) = colf[2];
         *(ptrC+3) = colf[3];
         ptrC += 4;
     }
 glBindTexture(GL_TEXTURE_2D, g_whitetex);

 glBindVertexArray(g_vao);
 glBindBuffer(GL_ARRAY_BUFFER, g_vbos[0]);
 glBufferData(GL_ARRAY_BUFFER, vSize*sizeof(float), v, GL_STATIC_DRAW);
 glBindBuffer(GL_ARRAY_BUFFER, g_vbos[1]);
 glBufferData(GL_ARRAY_BUFFER, uvSize*sizeof(float), uv, GL_STATIC_DRAW);
 glBindBuffer(GL_ARRAY_BUFFER, g_vbos[2]);
 glBufferData(GL_ARRAY_BUFFER, cSize*sizeof(float), c, GL_STATIC_DRAW);
 glDrawArrays(GL_TRIANGLES, 0, (numCoords * 2 + numCoords - 2)*3);

}

static void drawRect(float x, float y, float w, float h, float fth, unsigned int col)
{
    float verts[4*2] =
        {
            x+0.5f, y+0.5f,
            x+w-0.5f, y+0.5f,
            x+w-0.5f, y+h-0.5f,
            x+0.5f, y+h-0.5f,
        };
    drawPolygon(verts, 4, fth, col);
}

/*
  static void drawEllipse(float x, float y, float w, float h, float fth, unsigned int col)
  {
  float verts[CIRCLE_VERTS*2];
  const float* cverts = g_circleVerts;
  float* v = verts;

  for (int i = 0; i < CIRCLE_VERTS; ++i)
  {
  *v++ = x + cverts[i*2]*w;
  *v++ = y + cverts[i*2+1]*h;
  }

  drawPolygon(verts, CIRCLE_VERTS, fth, col);
  }
*/
static void drawRoundedRect(float x, float y, float w, float h, float r, float fth, unsigned int col)
{
    
#define n CIRCLE_VERTS / 4
    //const unsigned n = CIRCLE_VERTS/4;
    float verts[(n+1)*4*2];
    const float* cverts = g_circleVerts;
    float* v = verts;
    int i;
    for ( i = 0; i <= n; ++i)
        {
            *v++ = x+w-r + cverts[i*2]*r;
            *v++ = y+h-r + cverts[i*2+1]*r;
        }

    for ( i = n; i <= n*2; ++i)
        {
            *v++ = x+r + cverts[i*2]*r;
            *v++ = y+h-r + cverts[i*2+1]*r;
        }

    for ( i = n*2; i <= n*3; ++i)
        {
            *v++ = x+r + cverts[i*2]*r;
            *v++ = y+r + cverts[i*2+1]*r;
        }

    for ( i = n*3; i < n*4; ++i)
        {
            *v++ = x+w-r + cverts[i*2]*r;
            *v++ = y+r + cverts[i*2+1]*r;
        }
    *v++ = x+w-r + cverts[0]*r;
    *v++ = y+r + cverts[1]*r;

    drawPolygon(verts, (n+1)*4, fth, col);
}


static void drawLine(float x0, float y0, float x1, float y1, float r, float fth, unsigned int col)
{
    float dx = x1-x0;
    float dy = y1-y0;
    float d = sqrtf(dx*dx+dy*dy);
    float nx;
    float ny;
    float verts[4*2];
    if (d > 0.0001f)
        {
            d = 1.0f/d;
            dx *= d;
            dy *= d;
        }
    nx = dy;
    ny = -dx;
    r -= fth;
    r *= 0.5f;
    if (r < 0.01f) r = 0.01f;
    dx *= r;
    dy *= r;
    nx *= r;
    ny *= r;

    verts[0] = x0-dx-nx;
    verts[1] = y0-dy-ny;

    verts[2] = x0-dx+nx;
    verts[3] = y0-dy+ny;

    verts[4] = x1+dx+nx;
    verts[5] = y1+dy+ny;

    verts[6] = x1+dx-nx;
    verts[7] = y1+dy-ny;

    drawPolygon(verts, 4, fth, col);
}


imguiBool  imguiRenderGLInit(const char* fontpath)
{
    int i;
    FILE *fp;
    unsigned char* ttfBuffer;
    unsigned char* bmap;
    unsigned char white_alpha = 255;
    int size;
    GLuint vso, fso;
    const char * vs =
        "#version 150\n"
        "uniform vec2 Viewport;\n"
        "in vec2 VertexPosition;\n"
        "in vec2 VertexTexCoord;\n"
        "in vec4 VertexColor;\n"
        "out vec2 texCoord;\n"
        "out vec4 vertexColor;\n"
        "void main(void)\n"
        "{\n"
        "    vertexColor = VertexColor;\n"
        "    texCoord = VertexTexCoord;\n"
        "    gl_Position = vec4(VertexPosition * 2.0 / Viewport - 1.0, 0.f, 1.0);\n"
        "}\n";
    
    const char * fs =
        "#version 150\n"
        "in vec2 texCoord;\n"
        "in vec4 vertexColor;\n"
        "uniform sampler2D Texture;\n"
        "out vec4  Color;\n"
        "void main(void)\n"
        "{\n"
        "    float alpha = texture(Texture, texCoord).r;\n"
        "    Color = vec4(vertexColor.rgb, vertexColor.a * alpha);\n"
        "}\n";
    
    for ( i = 0; i < CIRCLE_VERTS; ++i)
        {
            float a = (float)i/(float)CIRCLE_VERTS * PI*2;
            g_circleVerts[i*2+0] = cosf(a);
            g_circleVerts[i*2+1] = sinf(a);
        }

    // Load font.
    fp = fopen(fontpath, "rb");
    if (!fp) return IMGUI_FALSE;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    ttfBuffer = (unsigned char*)malloc(size);
    if (!ttfBuffer)
        {
            fclose(fp);
            return IMGUI_FALSE;
        }

    fread(ttfBuffer, 1, size, fp);
    fclose(fp);
    fp = 0;

    bmap = (unsigned char*)malloc(512*512);
    if (!bmap)
        {
            free(ttfBuffer);
            return IMGUI_FALSE;
        }

    stbtt_BakeFontBitmap(ttfBuffer,0, 15.0f, bmap,512,512, 32,96, g_cdata);

    // can free ttf_buffer at this point
    glGenTextures(1, &g_ftex);
    glBindTexture(GL_TEXTURE_2D, g_ftex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512,512, 0, GL_RED, GL_UNSIGNED_BYTE, bmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // can free ttf_buffer at this point
    glGenTextures(1, &g_whitetex);
    glBindTexture(GL_TEXTURE_2D, g_whitetex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1, 1, 0, GL_RED, GL_UNSIGNED_BYTE, &white_alpha);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenVertexArrays(1, &g_vao);
    glGenBuffers(3, g_vbos);

    glBindVertexArray(g_vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, g_vbos[0]);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, g_vbos[1]);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, g_vbos[2]);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*4, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, 0, 0, GL_STATIC_DRAW);
    g_program = glCreateProgram();

    vso = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vso, 1, (const char **)  &vs, NULL);
    glCompileShader(vso);
    glAttachShader(g_program, vso);

    fso = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fso, 1, (const char **) &fs, NULL);
    glCompileShader(fso);
    glAttachShader(g_program, fso);

    glBindAttribLocation(g_program,  0,  "VertexPosition");
    glBindAttribLocation(g_program,  1,  "VertexTexCoord");
    glBindAttribLocation(g_program,  2,  "VertexColor");
    glBindFragDataLocation(g_program, 0, "Color");
    glLinkProgram(g_program);
    glDeleteShader(vso);
    glDeleteShader(fso);

    glUseProgram(g_program);
    g_programViewportLocation = glGetUniformLocation(g_program, "Viewport");
    g_programTextureLocation = glGetUniformLocation(g_program, "Texture");

    glUseProgram(0);


    free(ttfBuffer);
    free(bmap);

    return GL_TRUE;
}

void imguiRenderGLDestroy()
{
    if (g_ftex)
        {
            glDeleteTextures(1, &g_ftex);
            g_ftex = 0;
        }

    if (g_vao)
        {
            glDeleteVertexArrays(1, &g_vao);
            glDeleteBuffers(3, g_vbos);
            g_vao = 0;
        }

    if (g_program)
        {
            glDeleteProgram(g_program);
            g_program = 0;
        }

}

static void getBakedQuad(stbtt_bakedchar *chardata, int pw, int ph, int char_index,
                         float *xpos, float *ypos, stbtt_aligned_quad *q)
{
    stbtt_bakedchar *b = chardata + char_index;
    int round_x = STBTT_ifloor(*xpos + b->xoff);
    int round_y = STBTT_ifloor(*ypos - b->yoff);

    q->x0 = (float)round_x;
    q->y0 = (float)round_y;
    q->x1 = (float)round_x + b->x1 - b->x0;
    q->y1 = (float)round_y - b->y1 + b->y0;

    q->s0 = b->x0 / (float)pw;
    q->t0 = b->y0 / (float)pw;
    q->s1 = b->x1 / (float)ph;
    q->t1 = b->y1 / (float)ph;

    *xpos += b->xadvance;
}

static const float g_tabStops[4] = {150, 210, 270, 330};

static float getTextLength(stbtt_bakedchar *chardata, const char* text)
{
    float xpos = 0;
    float len = 0;
    while (*text)
        {
            int c = (unsigned char)*text;
            if (c == '\t')
                {
                    int i;
                    for ( i = 0; i < 4; ++i)
                        {
                            if (xpos < g_tabStops[i])
                                {
                                    xpos = g_tabStops[i];
                                    break;
                                }
                        }
                }
            else if (c >= 32 && c < 128)
                {
                    stbtt_bakedchar *b = chardata + c-32;
                    int round_x = STBTT_ifloor((xpos + b->xoff) + 0.5);
                    len = round_x + b->x1 - b->x0 + 0.5f;
                    xpos += b->xadvance;
                }
            ++text;
        }
    return len;
}

static void drawText(float x, float y, const char *text, int align, unsigned int col)
{
    float r, g, b, a;
    float ox;

    if (!g_ftex) return;
    if (!text) return;

    if (align == IMGUI_ALIGN_CENTER)
        x -= getTextLength(g_cdata, text)/2;
    else if (align == IMGUI_ALIGN_RIGHT)
        x -= getTextLength(g_cdata, text);

    r = (float) (col&0xff) / 255.f;
    g = (float) ((col>>8)&0xff) / 255.f;
    b = (float) ((col>>16)&0xff) / 255.f;
    a = (float) ((col>>24)&0xff) / 255.f;

    // assume orthographic projection with units = screen pixels, origin at top left
    glBindTexture(GL_TEXTURE_2D, g_ftex);

    ox = x;

    while (*text)
        {
            int c = (unsigned char)*text;
            if (c == '\t')
                {
                    int i;
                    for ( i = 0; i < 4; ++i)
                        {
                            if (x < g_tabStops[i]+ox)
                                {
                                    x = g_tabStops[i]+ox;
                                    break;
                                }
                        }
                }
            else if (c >= 32 && c < 128)
                {
                    stbtt_aligned_quad q;
                  
                    float v[12];
                    float uv[12];
                    float fc[24];

                    getBakedQuad(g_cdata, 512,512, c-32, &x,&y,&q);

                    v[0] = q.x0; v[1] = q.y0;
                    v[2] = q.x1; v[3] = q.y1;
                    v[4] = q.x1; v[5] = q.y0;
                    v[6] = q.x0; v[7] = q.y0;
                    v[8] = q.x0; v[9] = q.y1;
                    v[10] = q.x1; v[11] = q.y1;
                    
                    
                    uv[0] = q.s0;  uv[1] = q.t0;
                    uv[2] = q.s1;  uv[3] = q.t1;
                    uv[4] = q.s1;  uv[5] = q.t0;
                    uv[6] = q.s0;  uv[7] = q.t0;
                    uv[8] = q.s0;  uv[9] = q.t1;
                    uv[10] = q.s1; uv[11] = q.t1;
                
                    fc[0] = r; fc[1] = g;  fc[2] = b; fc[3] = a;
                    fc[4] = r; fc[5] = g;  fc[6] = b; fc[7] = a;
                    fc[8] = r; fc[9] = g;  fc[10] = b; fc[11] = a;
                    fc[12] = r; fc[13] = g; fc[14] = b; fc[15] = a;
                    fc[16] = r; fc[17] = g; fc[18] = b; fc[19] = a;
                    fc[20] = r; fc[21] = g; fc[22] = b; fc[23] = a;
                    
                    glBindVertexArray(g_vao);
                    glBindBuffer(GL_ARRAY_BUFFER, g_vbos[0]);
                    glBufferData(GL_ARRAY_BUFFER, 12*sizeof(float), v, GL_STATIC_DRAW);
                    glBindBuffer(GL_ARRAY_BUFFER, g_vbos[1]);
                    glBufferData(GL_ARRAY_BUFFER, 12*sizeof(float), uv, GL_STATIC_DRAW);
                    glBindBuffer(GL_ARRAY_BUFFER, g_vbos[2]);
                    glBufferData(GL_ARRAY_BUFFER, 24*sizeof(float), fc, GL_STATIC_DRAW);
                    glDrawArrays(GL_TRIANGLES, 0, 6);

                }
            ++text;
        }

    //glEnd();
    //glDisable(GL_TEXTURE_2D);
}


void imguiRenderGLDraw(int width, int height)
{
    const float s = 1.0f/8.0f;
    const struct imguiGfxCmd* q = imguiGetRenderQueue();
    int nq = imguiGetRenderQueueSize();
    int i;
    glViewport(0, 0, width, height);
    glUseProgram(g_program);
    glActiveTexture(GL_TEXTURE0);
    glUniform2f(g_programViewportLocation, (float) width, (float) height);
    glUniform1i(g_programTextureLocation, 0);


    glDisable(GL_SCISSOR_TEST);

    for ( i = 0; i < nq; ++i)
        {
            const struct imguiGfxCmd *cmd = &q[i];
            if (cmd->type == IMGUI_GFXCMD_RECT)
                {
                    if (cmd->rect.r == 0)
                        {
                            drawRect((float)cmd->rect.x*s+0.5f, (float)cmd->rect.y*s+0.5f,
                                     (float)cmd->rect.w*s-1, (float)cmd->rect.h*s-1,
                                     1.0f, cmd->col);
                        }
                    else
                        {
                            drawRoundedRect((float)cmd->rect.x*s+0.5f, (float)cmd->rect.y*s+0.5f,
                                            (float)cmd->rect.w*s-1, (float)cmd->rect.h*s-1,
                                            (float)cmd->rect.r*s, 1.0f, cmd->col);
                        }
                }
            else if (cmd->type == IMGUI_GFXCMD_LINE)
                {
                    drawLine(cmd->line.x0*s, cmd->line.y0*s, cmd->line.x1*s, cmd->line.y1*s, cmd->line.r*s, 1.0f, cmd->col);

                }
            else if (cmd->type == IMGUI_GFXCMD_TRIANGLE)
                {
                    if (cmd->flags == 1)
                        {
                            const float verts[3*2] =
                                {
                                    (float)cmd->rect.x*s+0.5f, (float)cmd->rect.y*s+0.5f,
                                    (float)cmd->rect.x*s+0.5f+(float)cmd->rect.w*s-1, (float)cmd->rect.y*s+0.5f+(float)cmd->rect.h*s/2-0.5f,
                                    (float)cmd->rect.x*s+0.5f, (float)cmd->rect.y*s+0.5f+(float)cmd->rect.h*s-1,
                                };
                            drawPolygon(verts, 3, 1.0f, cmd->col);
                        }
                    if (cmd->flags == 2)
                        {
                            const float verts[3*2] =
                                {
                                    (float)cmd->rect.x*s+0.5f, (float)cmd->rect.y*s+0.5f+(float)cmd->rect.h*s-1,
                                    (float)cmd->rect.x*s+0.5f+(float)cmd->rect.w*s/2-0.5f, (float)cmd->rect.y*s+0.5f,
                                    (float)cmd->rect.x*s+0.5f+(float)cmd->rect.w*s-1, (float)cmd->rect.y*s+0.5f+(float)cmd->rect.h*s-1,
                                };
                            drawPolygon(verts, 3, 1.0f, cmd->col);
                        }
                }
            else if (cmd->type == IMGUI_GFXCMD_TEXT)
                {
                    drawText(cmd->text.x, cmd->text.y, cmd->text.text, cmd->text.align, cmd->col);
                }
            else if (cmd->type == IMGUI_GFXCMD_SCISSOR)
                {

                    if (cmd->flags)
                        {
                            glEnable(GL_SCISSOR_TEST);
                            glScissor(cmd->rect.x, cmd->rect.y, cmd->rect.w, cmd->rect.h);
                        }
                    else
                        {
                            glDisable(GL_SCISSOR_TEST);
                        }
                }
        }
    glDisable(GL_SCISSOR_TEST);
}
