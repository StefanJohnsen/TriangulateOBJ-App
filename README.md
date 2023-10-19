# Triangulate obj files ([new release](https://github.com/StefanJohnsen/TriangulateOBJ-App/releases)) 
A lightweight console application designed to convert all polygons in a Wavefront OBJ file to triangles. The result is written to a new file with minimal modifications from the original. 
This console application is built upon the functionalities provided by the header-only file found in the [TriangulateOBJ](https://github.com/StefanJohnsen/TriangulateOBJ) repository. It demonstrates the practical applications and integrations of the TriangulateOBJ.h.
<br><br><br>
```bash
         ---------------------------------------------------
         lego.triangulated.obj 13MB    (+3MB)
         ---------------------------------------------------
         Vertices              :    101.306
         ---------------------------------------------------
         Triangles             :     10.780
         Polygons              :     97.374
         ---------------------------------------------------
         Triangles    (after)  :    213.252     (+202.472)
         Polygons     (after)  :          0
         ---------------------------------------------------
         Execution time        : 371 milliseconds
         ---------------------------------------------------
 ```

<br><br>
### New Release 1.1.0
In this latest release, we've made significant advancements in our polygon triangulation capabilities. The previous version was primarily reliant on the fan triangulation algorithm, which was efficient for convex polygons but fell short when applied to concave polygons.

To address this, the new update introduces the integration of the "Ear Cut" algorithm, significantly expanding the tool's versatility. This innovative approach ensures accurate triangulation of both convex and concave polygons, marking a substantial improvement in the algorithm's robustness and application range.

<br><br>
### Intended Users
1. **Game Developers**: Optimized models are key to performance in game development. Triangular polygons are computationally less intensive and are often preferred. With this tool, developers can easily ensure their 3D assets are optimized for real-time rendering.
2. **CAD Professionals**: Triangular polygons often provide smoother transitions when transitioning from CAD to visualization tools or VR platforms. This tool offers an easy step in the conversion process.
3. **3D Printing Enthusiasts**: Some 3D printing software may require models to be triangulated for accurate print outputs. Using this application, enthusiasts can ensure their models are ready for printing.
4. **Educators and Students**: Those teaching or learning about 3D modelling, graphics, or computer graphics algorithms can benefit from a lightweight tool to quickly triangulate models, helping in understanding the principles of 3D geometry.
5. **Software Developers**: For those integrating 3D features in their applications or looking to modify Wavefront OBJ files programmatically, this utility provides a no-fuss, dependency-free method to ensure models are triangulated.
6. **3D Artists and Designers**: Often, designers create complex models with various polygon types, but some rendering or game engines might only accept triangular polygons. This tool aids in preparing models to be compatible with such systems without the need for heavy-duty software.
   
<br><br>
# Binary releases
Download prebuilt from [releases](https://github.com/StefanJohnsen/TriangulateOBJ-App/releases)

<br><br>
# Building in Windows (Visual Studio)

Clone the [git](https://git-scm.com/downloads) repo

   ```bash
   git clone https://github.com/StefanJohnsen/TriangulateOBJ-App.git
   cd TriangulateOBJ-App
   ```

### [CMake](https://cmake.org/download/)

   ```bash
   mkdir build
   cd build
   cmake ..
   ```

### [Premake5](https://premake.github.io/download/)

   ```bash
   premake5 vs2022   # replace 'vs2022' with your version of Visual Studio
   ```
   Open `build/TriangulateOBJ.sln` file in Visual Studio and build the solution.

<br><br>
# Building in Linux and MacOS

Clone the [git](https://git-scm.com/downloads) repo

   ```bash
   git clone https://github.com/StefanJohnsen/TriangulateOBJ-App.git
   cd TriangulateOBJ
   ```

#### [CMake](https://cmake.org/download/)

   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

#### [Premake5](https://premake.github.io/download/)

   ```bash
   premake5 gmake2
   make
   ```

<br><br>
# Building with CMake Project in Visual Studio

Use Visual Studio's CMake support to build and debug on Windows and Linux

- Visual Studio 2017 or newer. Ensure you've installed the "Desktop development with C++" workload and enabled "C++ CMake tools for Windows" during installation. Visual Studio has integrated CMake support from version 2017 onwards.
- (Optional) Windows Subsystem for Linux (WSL) with a Linux distribution (e.g., Ubuntu) for those wishing to build and test in a Linux environment.

Open `TriangulateOBJ-App` folder in Visual Studio and build the solution. Remember to right-click main.cpp file in Solution Explorer, select `Set as Startup Item` in the menu and push F5 to build and run.

<br><br>
# Running the App from the Command Prompt (Windows)

1. Copy the compiled or pre-compiled `TriangulatedOBJ.exe` to the `.\TriangulateOBJ-App` directory.
  
2. Navigate to the `.\TriangulateOBJ-App` directory using the Command Prompt and run the following command:
<br>

   ```bash
   TriangulateObj ObjFiles\lego.obj
   ```

On successful execution, you'll see an output similar to:

   ```bash
   ObjFiles\lego.obj has been triangulated

   ---------------------------------------------------
   lego.triangulated.obj 13MB    (+3MB)
   ---------------------------------------------------
   Vertices              :    101.306
   ---------------------------------------------------
   Triangles             :     10.780
   Polygons              :     97.374
   ---------------------------------------------------
   Triangles    (after)  :    213.252     (+202.472)
   Polygons     (after)  :          0
   ---------------------------------------------------
   Execution time        : 371 milliseconds
   ---------------------------------------------------
   ```
This command will triangulate the specified OBJ file and provide a summary of the operation.

<br><br>
# License
This software is released under the GNU General Public License v3.0 terms.<br> 
Details and terms of this license can be found at: https://www.gnu.org/licenses/gpl-3.0.html<br><br>
For those who require the freedom to operate without the constraints of the GPL,<br>
a commercial license can be obtained by contacting the author at stefan.johnsen@outlook.com
