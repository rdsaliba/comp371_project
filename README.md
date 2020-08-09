# comp371_project 
Semester: S20

Section: CC

## Team members
| Name          | ID        | 
| ------------- |:-------------:|
| Taqi Haque | 40043834 |
| Roy Saliba | 27381751 | 
| Hau Gilles Che | 40063554 | 
| William Kang | 40099021 | 
| Swetang Patel | 27205813 | 

## How To Run
1. Clone this repo. "git clone <url>"

2. Set the project directory as your working directory. "cd comp371_project"

3. Run the application (preferably with Visual Studio or Xcode)

## Environment used

Visual Studio 2019 

Windows SDK Version: 10.0

Platform Toolset: Visual Studia 2019 (v142)

## Features and Functionalities
This OpenGL application contains a simple virtual scene consisting of 5 models. The 5 models are placed on a 100x100 square grid. Each model consists of the 3rd letter of the name and 4th digit of the student ID of the team member who created it. Each model were transformed using scaling, rotation, and translation. The user can select any of the models using the keys 1-5, and apply transformations using the controls listed below.

## Controls

## Rubiks Cube:
Rubiks Cube Notation info:
The faces of a rubiks cube are commonly referred to by the first letter of each face in this manner:
  
  L = Left 
  
  R = Right
  
  U = Up
  
  D = Down
  
  F = Front
  
  B = Back
 
-Each face can be rotated in a clockwise or counterclockwise direction. 

-A clockwise rotation (based on the face where the rotation is applied on) is denoted by the letter   associated with said face.

-A counterclockwise rotation is denoted by the letter and a ' (e.g.: L' = counterclockwise rotation on the Left face).

-Like mentionned above, the direction of a rotation is based on the face in question, thus opposite faces rotate clockwise in opposite directions (e.g.: L = R')
 
### Rubiks Cube Controls:
lower case input will trigger a clockwise rotation of the desired face while uppercase (shift + [key]) will trigger a counterclockwise rotation

i/I: L/L'

o/O: R/R'

f/F: F/F'

y/Y: B/B'

n/N: U/U'

m/M: D/D' 

### General Controls:

ESC: close window

1-5: select model

H: home button

X: texture toggle

N: shadow toggle

### Model Controls:

U: scale up by 0.1f

J: scale down by 0.1f

shift_A: move left (x direction) by 0.1f

shift_D: move right (y direction) by 0.1f

shift_W: move up (y direction) by 0.1f

shift_S: move down (y direction) by 0.1f

A: rotate left 5 degrees about Y axis

D: rotate right 5 degrees about Y axis

Z: translate in the negative Z direction by 0.1f, and shear by -0.05f

shift_Z: translate in the positive Z direction by 0.1f, and shear by 0.05f

spacebar: randomly reposition model on the grid


P: points rendering mode

L: lines rendering mode

T: triangles rendering mode


### Camera Controls:

Mouse_Cursor: FPS camera

G: move forward

V: move back

C: move left

B: move right

shift: move faster


Right_mouse_button: pan camera in x direction

Middle_mouse_button: tilt camera in y direction

Left_mouse_button: Zoom in/out


### World Orientation Controls:

Left_arrow: rotation (CCW) about positive x axis by 5.0f

Right_arrow: rotation (CCW) about negative x axis by 5.0f

Up_arrow: rotation (CCW) about positive y axis by 5.0f

Down_arrow: rotation (CCW) about negative y axis by 5.0f

## Information on Classes
programmingAssignment: main

Axis: draws x, y, z axis

ViewController: Handles initialization of camera and view matrix and process updates of the viewMatrix according to user inputs.

ModelController: Handles initialization and  actions against the models rendered in the 3D world. Hold Reference to the currently focused model and applies transformations to the models based on user input.

Model: Representation a 3D model to be rendered.

[XXX]Model: Representation of 3D model [LETTER][DIGIT].

Sphere: 3D model of a triangle mesh sphere that is draw above each individual model

ModelUtilities: Set of utilities/helper functions to manipulate/transform the models in the 3D way in a standardized manner.

# Learn OpenGL
http://docs.gl/

http://www.opengl-tutorial.org/

https://learnopengl.com/Getting-started/OpenGL
