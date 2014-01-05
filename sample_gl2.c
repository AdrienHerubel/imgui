/*
// sample_gl2.cpp - public domain
// authored from 2012-2013 by Adrien Herubel 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include <GL/glew.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include "GL/glfw.h"

#include "imgui.h"
#include "imguiRenderGL2.h"

int main( int argc, char **argv )
{
    GLenum err;
    int width = 1024, height = 768;
    
    // imgui states
    imguiBool checked1 = GL_FALSE;
    imguiBool checked2 = GL_FALSE;
    imguiBool checked3 = GL_TRUE;
    imguiBool checked4 = GL_FALSE;
    float value1 = 50.f;
    float value2 = 30.f;
    int scrollarea1 = 0;
    int scrollarea2 = 0;

    /* // glfw scrolling */
    int glfwscroll = 0;
    
    /* // Initialise GLFW */
    if( !glfwInit() )
        {
            fprintf( stderr, "Failed to initialize GLFW\n" );
            exit( EXIT_FAILURE );
        }

    /* // Open a window and create its OpenGL context */
    if( !glfwOpenWindow( width, height, 0,0,0,0, 24,0, GLFW_WINDOW ) )
        {
            fprintf( stderr, "Failed to open GLFW window\n" );
            glfwTerminate();
            exit( EXIT_FAILURE );
        }

    glfwSetWindowTitle( "imgui sample imguiRenderGL2" );
    err = glewInit();
    if (GLEW_OK != err)
        {
            /* Problem: glewInit failed, something is seriously wrong. */
            fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
            exit( EXIT_FAILURE );
        }

    /* // Ensure we can capture the escape key being pressed below */
    glfwEnable( GLFW_STICKY_KEYS );

    /* // Enable vertical sync (on cards that support it) */
    glfwSwapInterval( 1 );

    /* // Init UI */
    if (!imguiRenderGLInit("DroidSans.ttf"))
        {
            fprintf(stderr, "Could not init GUI renderer.\n");
            exit(EXIT_FAILURE);
        }

    glClearColor(0.8f, 0.8f, 0.8f, 1.f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    do
        {
            int i;
            int currentglfwscroll;
            int mscroll = 0;
            int mousex; int mousey;
            int leftButton, rightButton, middleButton;
            int toggle = 0;
            unsigned char mousebutton;
            float projection[16];

            glfwGetWindowSize(&width, &height);
            glViewport(0, 0, width, height);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Mouse states
            mousebutton = 0;
            currentglfwscroll = glfwGetMouseWheel();
            mscroll = 0;
            if (currentglfwscroll < glfwscroll)
                mscroll = 2;
            if (currentglfwscroll > glfwscroll)
                mscroll = -2;
            glfwscroll = currentglfwscroll;
            
            glfwGetMousePos(&mousex, &mousey);
            mousey = height - mousey;
            leftButton   = glfwGetMouseButton( GLFW_MOUSE_BUTTON_LEFT );
            rightButton  = glfwGetMouseButton( GLFW_MOUSE_BUTTON_RIGHT );
            middleButton = glfwGetMouseButton( GLFW_MOUSE_BUTTON_MIDDLE );
            toggle = 0;
            if( leftButton == GLFW_PRESS )
                mousebutton |= IMGUI_MBUT_LEFT;
    
            // Draw UI
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            
            projection[0] = 2.0f/width; projection[1] = projection[2] = projection[3] = 0.0f;
            projection[4] = 0.0f;       projection[5] = 2.0f/height; projection[6] = projection[7] =  0.0f;
            projection[8] = projection[9] = 0.0f; projection[10] = -2.0f; projection[11] = 0.0f;
            projection[12] = projection[13] = projection[14] = -1.0f; projection[15] = 1.0f;  
            glLoadMatrixf(projection);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glUseProgram(0);


            imguiBeginFrame(mousex, mousey, mousebutton, mscroll);

            imguiBeginScrollArea("Scroll area", 10, 10, width / 5, height - 20, &scrollarea1);
            imguiSeparatorLine();
            imguiSeparator();

            imguiButton("Button", GL_TRUE);
            imguiButton("Disabled button", GL_FALSE);
            imguiItem("Item", GL_TRUE);
            imguiItem("Disabled item", GL_FALSE);
            toggle = imguiCheck("Checkbox", checked1, GL_TRUE);
            if (toggle)
                checked1 = !checked1;
            toggle = imguiCheck("Disabled checkbox", checked2, GL_FALSE);
            if (toggle)
                checked2 = !checked2;
            toggle = imguiCollapse("Collapse", "subtext", checked3, GL_TRUE);
            if (checked3)
                {
                    imguiIndent();
                    imguiLabel("Collapsible element");
                    imguiUnindent();
                }
            if (toggle)
                checked3 = !checked3;
            toggle = imguiCollapse("Disabled collapse", "subtext", checked4, GL_FALSE);
            if (toggle)
                checked4 = !checked4;
            imguiLabel("Label");
            imguiValue("Value");
            imguiSlider("Slider", &value1, 0.f, 100.f, 1.f, GL_TRUE);
            imguiSlider("Disabled slider", &value2, 0.f, 100.f, 1.f, GL_FALSE);
            imguiIndent();
            imguiLabel("Indented");
            imguiUnindent();
            imguiLabel("Unindented");

            imguiEndScrollArea();

            imguiBeginScrollArea("Scroll area", 20 + width / 5, 500, width / 5, height - 510, &scrollarea2);
            imguiSeparatorLine();
            imguiSeparator();
            for ( i = 0; i < 100; ++i)
                imguiLabel("A wall of text");

            imguiEndScrollArea();
            imguiEndFrame();

            imguiDrawText(30 + width / 5 * 2, height - 20, IMGUI_ALIGN_LEFT, "Free text",  imguiRGBA(32,192, 32,192));
            imguiDrawText(30 + width / 5 * 2 + 100, height - 40, IMGUI_ALIGN_RIGHT, "Free text",  imguiRGBA(32, 32, 192, 192));
            imguiDrawText(30 + width / 5 * 2 + 50, height - 60, IMGUI_ALIGN_CENTER, "Free text",  imguiRGBA(192, 32, 32,192));

            imguiDrawLine(30 + width / 5 * 2, height - 80, 30 + width / 5 * 2 + 100, height - 60, 1.f, imguiRGBA(32,192, 32,192));
            imguiDrawLine(30 + width / 5 * 2, height - 100, 30 + width / 5 * 2 + 100, height - 80, 2.f, imguiRGBA(32, 32, 192, 192));
            imguiDrawLine(30 + width / 5 * 2, height - 120, 30 + width / 5 * 2 + 100, height - 100, 3.f, imguiRGBA(192, 32, 32,192));

            imguiDrawRoundedRect(30 + width / 5 * 2, height - 240, 100, 100, 5.f, imguiRGBA(32,192, 32,192));
            imguiDrawRoundedRect(30 + width / 5 * 2, height - 350, 100, 100, 10.f, imguiRGBA(32, 32, 192, 192));
            imguiDrawRoundedRect(30 + width / 5 * 2, height - 470, 100, 100, 20.f, imguiRGBA(192, 32, 32,192));
        
            imguiDrawRect(30 + width / 5 * 2, height - 590, 100, 100, imguiRGBA(32, 192, 32, 192));
            imguiDrawRect(30 + width / 5 * 2, height - 710, 100, 100, imguiRGBA(32, 32, 192, 192));
            imguiDrawRect(30 + width / 5 * 2, height - 830, 100, 100, imguiRGBA(192, 32, 32,192));

            imguiRenderGLDraw(width, height); 

            /* // Check for errors */
            err = glGetError();
            if(err != GL_NO_ERROR)
                {
                    fprintf(stderr, "OpenGL Error : %s\n", gluErrorString(err));
                }

            /* // Swap buffers */
            glfwSwapBuffers();

        } /* // Check if the ESC key was pressed or the window was closed */
    while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
           glfwGetWindowParam( GLFW_OPENED ) );

    /* // Clean UI */
    imguiRenderGLDestroy();

    /* // Close OpenGL window and terminate GLFW */
    glfwTerminate();

    exit( EXIT_SUCCESS );
}

