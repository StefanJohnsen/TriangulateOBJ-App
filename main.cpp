/*
  Copyright (c) 2023 FalconCoding

  This software is released under the terms of the
  GNU General Public License v3.0. Details and terms of this
  license can be found at: https://www.gnu.org/licenses/gpl-3.0.html
*/

/*
  -[Possible command arguments]---------------------------------------------------------

   (*) c:\temp\lego.obj
	   c:\temp\lego.obj lego_convert.obj                                (same directory)
	   c:\temp\lego.obj c:\temp\triangulated\lego_converted.obj
	   c:\temp\lego.obj c:\converted\objfiles

   (*) Best choice => triangulated file => c:\temp\lego.triangulated.obj

  --------------------------------------------------------------------------------------
*/

#include <iostream>
#include "cmd.h"
#include "out.h"
#include "TriangulateOBJ.h"

using namespace std;

int main(int argc, char* argv[])
{
	obj::Triangulate obj;

	if( !arg(argc, argv) ) return 1;

	const auto triangulated = obj.triangulate(source.string(), target.string());

	if( triangulated )
		std::cout << source.string() << " has been triangulated" << std::endl;
	else if( obj.empty() )
		std::cout << source.string() << " can not be triangulated (no polygons)" << std::endl;
	else
		std::cout << source.string() << " can not be triangulated (unknown format)" << std::endl;

	report(obj);

	return triangulated ? 0 : 1;
}
