// Toon shading


#include "headers.h"
#include "linalg.h"
#include "wavefront.h"
#include "renderer.h"
#include "gpuProgram.h"
#include "font.h"


GLFWwindow *window;
wfModel    *obj;      // the object
Renderer   *renderer; // class to do multipass rendering

float theta = 0;
bool sleeping = false;

GLuint windowWidth = 600;
GLuint windowHeight = 400;
float factor = 0;

float fovy;
vec3 eyePosition;

bool isTorso = false; // for torso.obj model, which uses a different projection matrix



void GLFWErrorCallback( int error, const char* description )

{
  char *errorString;

  switch (error) {
  case GLFW_API_UNAVAILABLE:
    errorString = "GLFW_API_UNAVAILABLE";
    break;
  case GLFW_FORMAT_UNAVAILABLE:
    errorString = "GLFW_FORMAT_UNAVAILABLE";
    break;
  case GLFW_INVALID_ENUM:
    errorString = "GLFW_INVALID_ENUM";
    break;
  case GLFW_INVALID_VALUE:
    errorString = "GLFW_INVALID_VALUE";
    break;
  case GLFW_NO_CURRENT_CONTEXT:
    errorString = "GLFW_NO_CURRENT_CONTEXT";
    break;
  case GLFW_NOT_INITIALIZED:
    errorString = "GLFW_NOT_INITIALIZED";
    break;
  case GLFW_OUT_OF_MEMORY:
    errorString = "GLFW_OUT_OF_MEMORY";
    break;
  case GLFW_PLATFORM_ERROR:
    errorString = "GLFW_PLATFORM_ERROR";
    break;
  case GLFW_VERSION_UNAVAILABLE:
    errorString = "GLFW_VERSION_UNAVAILABLE";
    break;
  default:
    errorString = "UNKNOWN";
  }

  cerr << "Error " << errorString << " (" << error << "): " << description << endl;
  exit(1);
}



void display()

{
  glClearColor( 1, 1, 1, 1 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // OCS-to-WCS

  mat4 M;

  if (isTorso)
    M = rotate( theta, vec3(0,1,0) )
      * rotate( -M_PI/2.0, vec3(1,0,0) )
      * translate( -1 * obj->centre );
  else
    M = rotate( theta, vec3(0.5,2,0) )
      * translate( -1 * obj->centre );

  // model-view transform (i.e. OCS-to-VCS)

  mat4 MV = translate( -1 * eyePosition )
          * M;

  // model-view-projection transform (i.e. OCS-to-CCS)

  float n = (eyePosition - obj->centre).length() - obj->radius;
  float f = (eyePosition - obj->centre).length() + obj->radius;

  mat4 MVP = perspective( fovy, windowWidth / (float) windowHeight, n, f )
           * MV;

  // Light direction in VCS is above, to the right, and behind the
  // eye.  That's in direction (1,1,1) since the view direction is
  // down the -z axis.

  vec3 lightDir(1,1,0.2);
  lightDir = lightDir.normalize();

  // Draw the objects

  renderer->render( obj, M, MV, MVP, lightDir );

  // Output status message

  char buffer[1000];
  renderer->makeStatusMessage( buffer );

  render_text( buffer, 10, 10, window );
}


// Reshape the window


void windowSizeCallback( GLFWwindow* window, int width, int height )

{
  windowWidth = width;
  windowHeight = height;
  glViewport( 0, 0, width, height );
  renderer->reshape( width, height );
}



// Handle a key press


void keyCallback( GLFWwindow* window, int key, int scancode, int action, int mods )
  
{
  if (action == GLFW_PRESS || action == GLFW_REPEAT)
    switch (key) {
    case 27: exit(0);
    case 'P':
      sleeping = !sleeping;
      break;
    case 'D':
      renderer->incDebug();
      break;
    case 'F':
      if (mods & GLFW_MOD_SHIFT)
	factor += 0.01; // uppercase F
      else
	factor -= 0.01; // lowercase f 
      cout << "factor = " << factor << endl;
      break;
    case GLFW_KEY_UP:
      eyePosition = (1/1.1) * eyePosition;
      break;
    case GLFW_KEY_DOWN:
      eyePosition = 1.1 * eyePosition;
      break;
    case GLFW_KEY_LEFT:
      fovy *= 1.1;
      break;
    case GLFW_KEY_RIGHT:
      fovy /= 1.1;
      break;
    }
}



// Main program


int main( int argc, char **argv )

{
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " scene.obj" << endl;
    exit(1);
  }

  // Set up GLFW

  glfwSetErrorCallback( GLFWErrorCallback );
  
  if (!glfwInit()) {
    cerr << "glfwInit() failed" << endl;
    return 1;
  }

  // If you get an error on running, try the other glfwWindowHint below
  
  glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_ES_API ); // use this for laptops
  //glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_API ); // use this for CasLab


  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );

  // Set up the window

  window = glfwCreateWindow( windowWidth, windowHeight, "toon shading", NULL, NULL );
  
  if (!window) {
    glfwTerminate();
    cerr << "glfwCreateWindow() failed." << endl;
    return 1;
  }

  glfwMakeContextCurrent( window );
  glfwSwapInterval( 1 );
  gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress );

  glfwSetKeyCallback( window, keyCallback );
  glfwSetWindowSizeCallback( window, windowSizeCallback );

  // Init fonts

#if 0
  char *execDir = strdup( argv[0] );
  char *lastSlash = strrchr( execDir, '/' );
  if (lastSlash == NULL)
    execDir[0] = '\0';
  else
    *(lastSlash+1) = '\0';

  char fn[1000];
  sprintf( fn, "%sFreeSans.ttf", execDir );
  initFont( fn, 20 ); // 20 = font height in pixels
#else
  initFont( "FreeSans.ttf", 20 ); // 20 = font height in pixels
#endif

  // Set up world objects

  obj = new wfModel( argv[1], MIPMAP_LINEAR );

  isTorso = (strlen(argv[1]) >= 9 && strcmp( &argv[1][strlen(argv[1])-9] , "torso.obj" ) == 0);

  // Point camera to the model

  const float initEyeDistance = 5.0;

  eyePosition = (initEyeDistance * obj->radius) * vec3(0,0,1);
  fovy = 2 * atan2( 1, initEyeDistance );

  // Set up renderer

  renderer = new Renderer( windowWidth, windowHeight );

  // Main loop

  struct timeb prevTime, thisTime; // record the last rendering time
  ftime( &prevTime );

  glEnable( GL_DEPTH_TEST );

  while (!glfwWindowShouldClose( window )) {

    // Find elapsed time since last render

    ftime( &thisTime );
    float elapsedSeconds = (thisTime.time + thisTime.millitm / 1000.0) - (prevTime.time + prevTime.millitm / 1000.0);
    prevTime = thisTime;

    // Update the world state

    if (!sleeping)
      theta += elapsedSeconds * 0.3;

    // Clear, display, and check for events

    glClearColor( 1, 1, 1, 1 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear depth buffer

    display();

    glfwSwapBuffers( window );
    glfwPollEvents();
  }

  // Clean up

  glfwDestroyWindow( window );
  glfwTerminate();

  return 0;
}



