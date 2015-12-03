//--------------------------------------------------------------------------------------
// BTH - Stefan Petersson 2014.
//--------------------------------------------------------------------------------------
#include <windows.h>

#include <string>
#include <fstream>
#include <streambuf>

#include <gl/glew.h>
#include <gl/GL.h>

#include "bth_image.h"
#include "SOIL\src\SOIL.h"

//GLM - Mathematics library
#include "glm\glm\glm.hpp"
#include "glm\glm\gtc\matrix_transform.hpp"
#include "glm\glm\gtc\type_ptr.hpp"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

using namespace std;

HWND InitWindow(HINSTANCE hInstance);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HGLRC CreateOpenGLContext(HWND wndHandle);

GLuint gVertexBuffer = 0;
GLuint gVertexAttribute = 0;
GLuint gShaderProgram = 0;

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

// functions implemented in Helper.cpp
// This function ALLOCATES MEMORY!
int loadshader(char* filename, char** ShaderSource, unsigned long& len);
// This function RELEASES THE MEMORY ALLOCATED
void unloadshader(GLubyte** ShaderSource);

//Transforms
glm::mat4 world;
glm::mat4 view;
glm::mat4 projection;

//Texture
GLuint texture;

void CreateShaders()
{
	GLint result;
	GLint length;

	//create vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	// open glsl file and put it in a string
	ifstream shaderFile("VertexShader.glsl");
	std::string shaderText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	// make a double pointer (only valid here)
	const char* shaderTextPtr = shaderText.c_str();
	// ask GL to load this
	glShaderSource(vs, 1, &shaderTextPtr, nullptr);
	// ask GL to compile it
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];
		glGetShaderInfoLog(vs, length, NULL, message);
		OutputDebugStringA(message);
		delete[] message;
	}

	//create fragment shader | same process.	
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	shaderFile.open("Fragment.glsl");
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	shaderTextPtr = shaderText.c_str();
	glShaderSource(fs, 1, &shaderTextPtr, nullptr);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];
		glGetShaderInfoLog(fs, length, NULL, message);
		OutputDebugStringA(message);
		delete[] message;
	}

	GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
	shaderFile.open("Geometry.glsl");
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	shaderTextPtr = shaderText.c_str();
	glShaderSource(gs, 1, &shaderTextPtr, nullptr);
	glCompileShader(gs);
	glGetShaderiv(gs, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderiv(gs, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];
		glGetShaderInfoLog(gs, length, NULL, message);
		OutputDebugStringA(message);
		delete[] message;
	}

	//link shader program (connect vs and ps)
	gShaderProgram = glCreateProgram();
	glAttachShader(gShaderProgram, fs);
	glAttachShader(gShaderProgram, vs);
	glAttachShader(gShaderProgram, gs);
	glLinkProgram(gShaderProgram);

	glGetProgramiv(gShaderProgram, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramiv(gShaderProgram, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];
		glGetProgramInfoLog(gShaderProgram, length, NULL, message);
		OutputDebugStringA(message);
		delete[] message;
	}
}

void CreateTriangleData()
{
	// this is how we will structure the input data for the vertex shader
	// every six floats, is one vertex.
	struct TriangleVertex
	{
		float x, y, z;
		float r, g, b;
		float tX, tY;
	};
	// create the actual data in plane Z = 0
	TriangleVertex triangleVertices[4] = 
	{
		// pos and color for each vertex
		{ -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
		{ -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
		{ 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f },
		{ 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f}
	};

	// Vertex Array Object (VAO) 
	glGenVertexArrays(1, &gVertexAttribute);
	// bind == enable
	glBindVertexArray(gVertexAttribute);
	// this activates the first and second attributes of this VAO
	glEnableVertexAttribArray(0); 
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// create a vertex buffer object (VBO) id
	glGenBuffers(1, &gVertexBuffer);
	// Bind the buffer ID as an ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBuffer);
	// This "could" imply copying to the GPU, depending on what the driver wants to do...
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

	// query where which slot corresponds to the input vertex_position in the Vertex Shader 
	GLuint vertexPos = glGetAttribLocation(gShaderProgram, "vertex_position");
	// specify that: the vertex attribute "vertexPos", of 3 elements of type FLOAT, not normalized, with STRIDE != 0,
	//               starts at offset 0 of the gVertexBuffer (it is implicitly bound!)
	glVertexAttribPointer(vertexPos, 3, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(0));

	// query where which slot corresponds to the input vertex_color in the Vertex Shader 
	GLuint vertexColor = glGetAttribLocation(gShaderProgram, "vertex_color");
	// specify that: the vertex attribute "vertex_color", of 3 elements of type FLOAT, not normalized, with STRIDE != 0,
	//               starts at offset (16 bytes) of the gVertexBuffer 
	glVertexAttribPointer(vertexColor, 3, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(sizeof(float) * 3));

	GLuint texPos = glGetAttribLocation(gShaderProgram, "texture_position");
	glVertexAttribPointer(texPos, 2, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(sizeof(float) * 6));
}

void SetViewport()
{
	glViewport(0, 0, 640, 480);
}

void Render()
{
	// set the color TO BE used
	glClearColor(0, 0, 0, 1);
	// use the color to clear the color buffer and clear depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(gShaderProgram, "ourTexture"), 0);

	//Set transform variables
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -2.0f);

	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget); 

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDirection, up));

	glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
	
	//Modify transforms
	world = glm::rotate(world, -0.0004f, glm::vec3(0.0f, 1.0f, 0.0f));
	view = glm::lookAt(cameraPos, cameraTarget, up);
	projection = glm::perspective(3.14f*0.45f, 640.0f/480.0f, 0.5f, 20.0f);

	//Add transform to the shader
	GLuint worldLoc = glGetUniformLocation(gShaderProgram, "world");
	GLuint viewLoc = glGetUniformLocation(gShaderProgram, "view");
	GLuint projectionLoc = glGetUniformLocation(gShaderProgram, "projection");

	glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glUseProgram(gShaderProgram);
	glBindVertexArray(gVertexAttribute);
	
	glBindTexture(GL_TEXTURE_2D, texture);
	// draw 4 vertices starting from index 0 in the vertex array currently bound (VAO), with current in-use shader
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void loadTexture()
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Set our texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//int width, height;
	//unsigned char* image = SOIL_load_image("bth_image.h", &width, &height, 0, SOIL_LOAD_RGBA);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, BTH_IMAGE_WIDTH, BTH_IMAGE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, BTH_IMAGE_DATA);
	//glGenerateMipmap(GL_TEXTURE_2D);

	//glBindTexture(GL_TEXTURE_2D, 0);
}

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	MSG msg = { 0 };
	HWND wndHandle = InitWindow(hInstance); //1. Skapa fönster
	
	if (wndHandle)
	{
		HDC hDC = GetDC(wndHandle);
		HGLRC hRC = CreateOpenGLContext(wndHandle); //2. Skapa och koppla OpenGL context

		glewInit(); //3. Initiera The OpenGL Extension Wrangler Library (GLEW)

		glEnable(GL_DEPTH_TEST);

		SetViewport(); //4. Sätt viewport

		loadTexture();

		CreateShaders(); //5. Skapa vertex- och fragment-shaders

		CreateTriangleData(); //6. Definiera triangelvertiser, 7. Skapa vertex buffer object (VBO), 8.Skapa vertex array object (VAO)

		ShowWindow(wndHandle, nCmdShow);

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				Render(); //9. Rendera

				SwapBuffers(hDC); //10. Växla front- och back-buffer
			}
		}

		wglMakeCurrent(NULL, NULL);
		ReleaseDC(wndHandle, hDC);
		wglDeleteContext(hRC);
		DestroyWindow(wndHandle);
	}

	return (int) msg.wParam;
}

HWND InitWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.hInstance      = hInstance;
	wcex.lpszClassName = L"BTH_GL_DEMO";
	if( !RegisterClassEx(&wcex) )
		return false;

	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	
	HWND handle = CreateWindow(
		L"BTH_GL_DEMO",
		L"BTH OpenGL Demo",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	return handle;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message) 
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;		
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

HGLRC CreateOpenGLContext(HWND wndHandle)
{
	//get handle to a device context (DC) for the client area
	//of a specified window or for the entire screen
	HDC hDC = GetDC(wndHandle);

	//details: http://msdn.microsoft.com/en-us/library/windows/desktop/dd318286(v=vs.85).aspx
	static  PIXELFORMATDESCRIPTOR pixelFormatDesc =
	{
		sizeof(PIXELFORMATDESCRIPTOR),    // size of this pfd  
		1,                                // version number  
		PFD_DRAW_TO_WINDOW |              // support window  
		PFD_SUPPORT_OPENGL |              // support OpenGL  
		PFD_DOUBLEBUFFER,                 // double buffered,
		PFD_TYPE_RGBA,                    // RGBA type  
		32,                               // 32-bit color depth  
		0, 0, 0, 0, 0, 0,                 // color bits ignored  
		0,                                // no alpha buffer  
		0,                                // shift bit ignored  
		0,                                // no accumulation buffer  
		0, 0, 0, 0,                       // accum bits ignored  
		0,                                // 0-bits for depth buffer <-- modified by Stefan      
		0,                                // no stencil buffer  
		0,                                // no auxiliary buffer  
		PFD_MAIN_PLANE,                   // main layer  
		0,                                // reserved  
		0, 0, 0                           // layer masks ignored  
	};

	//attempt to match an appropriate pixel format supported by a
	//device context to a given pixel format specification.
	int pixelFormat = ChoosePixelFormat(hDC, &pixelFormatDesc);

	//set the pixel format of the specified device context
	//to the format specified by the iPixelFormat index.
	SetPixelFormat(hDC, pixelFormat, &pixelFormatDesc);

	//create a new OpenGL rendering context, which is suitable for drawing
	//on the device referenced by hdc. The rendering context has the same
	//pixel format as the device context.
	HGLRC hRC = wglCreateContext(hDC);
	
	//makes a specified OpenGL rendering context the calling thread's current
	//rendering context. All subsequent OpenGL calls made by the thread are
	//drawn on the device identified by hdc. 
	wglMakeCurrent(hDC, hRC);

	return hRC;
}