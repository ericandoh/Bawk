              _
          _n_|_|_,_
         |===.-.===|
         |  ((_))  |
         '==='-'==='

          CS 194-26
          PROJECT 8

---------------------------------
INTRO
---------------------------------
Name: Jingyu Oh
SID: 23358680

---------------------------------
ABOUT CODE
---------------------------------
Written in: C++, a couple of C dependencies, and a few core libraries
Codebase to run the program is too big (since it comes attached with a game engine and whatnot)
so the code can be found at: https://github.com/ericandoh/Bawk/tree/efros

All files:
 - Bawk/efros_leung_two.cpp: the source C++ file containing all the code written SPECIFICALLY for this project
 - Bawk/debug_action.cpp: entrypoint where we call efros_leung_two.cpp
 - Bawk/main.cpp: entrypoint of program
 - Bawk.xcodeproj: Xcode project file for the project
 - Bawk/*.cpp: all source files needed to run the program
 - Bawk-copyme: needed to run the program. see section "How To Run"
 - Bawk-exec: precompiled standalone binary, might not work for non-MacOSX environments

---------------------------------
LIBRARIES
---------------------------------
List of libraries I used for this project:
GLFW
OpenGL
GLM (source included)
JsonReader (source included)

The installs for the nonincluded files can be found at:
http://www.glfw.org/docs/latest/build.html
https://www.opengl.org/

Not all the libraries might be needed to run the program - see "How To Run" for details

---------------------------------
HOW TO RUN
---------------------------------
There are multiple ways to run this program, depending on how much you want to do (compile, just run, etc?)
All of them are a bit complicated due to lack of foresight on my part. 

First, clone my project at: https://github.com/ericandoh/Bawk/tree/efros

Next, go into the folder, and copy/move the folder "Bawk-copyme" into your root directory. Rename the folder "Bawk". 
If this is not possible, see the section for how to compile, and edit the file "Bawk/main_dir.h"

I've attached a standalone executable binary compiled on my system with the source code. This may or may not work 
since it was compiled specifically for my OS. Once you have copied over the "Bawk-copyme" folder and renamed it to 
"Bawk", if all goes well it should start running off the box. 

If instead you wish to compile the code, there are a few options:
1. Via Xcode (Mac only): Open the Bawk.xcodeproj file, and resolve dependencies in the Build file. Most notably, there will be 
   missing links for the GLFW libraries. You will need to add those and let Xcode know where to search for those libraries.
   There's tutorial links at:
     https://chrischoy208.wordpress.com/2014/01/05/how-to-setup-xcode-5-project-with-glfl-and-opengl/
     http://stackoverflow.com/questions/777111/xcode-project-setup-for-glfw-library
2. Via command line directly (complicated): You will need to link to a copy of OpenGL in your g++, and also link to GLFW, in your 
   compile command line...

Once its running:

  I've configured the program to read in the data from the area of the world from (0, 3, 0) to (48, 12, 48) (inclusive-exclusive)
  This can be changed at: "Bawk/debug_action.cpp"

  The output will be then written to (-48,3,-48) to (-16,13,-16) of the world. The program writes one iteration per frame of the game
  and so you can see the blocks being written over time. 

  The actual block data is saved to root/Bawk/data/testworld, in chunk-sized block data files. 















