#pragma once
/*
  cmd.h - Helper class for file operations

  NB: TriangulateOBJ.h has no dependencies to this file, only main.cpp is using it.
  
  This header provides a set of utility functions for handling file operations,
  reporting file sizes and execution times, and working with file paths.

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

#include <string>
#include <iostream>
#include <algorithm>
#include <filesystem>

static std::string file_lbl = "triangulated";
static std::string file_ext = "obj";

using Path = std::filesystem::path;

static Path source;
static Path target;

bool arg1(char* argv[]);

bool arg2(char* argv[]);

bool arg3(char* argv[]);

bool arg();

void launch();

inline bool arg(const int argc, char* argv[])
{
	launch();

	switch( argc )
	{
	case 1:return arg1(argv);
	case 2:return arg2(argv);
	case 3:return arg3(argv);
	default:return arg();
	}
}

inline bool arg1(char* argv[])
{
	std::cout << "Error: No source " << file_ext << " file specified" << std::endl;

	return false;
}

std::string ext(const Path&);

inline bool arg2(char* argv[])
{
	source = argv[1];

	if( !exists(source) )
	{
		std::cout << "Error: Could not open the source file " << source.string() << std::endl;

		return false;
	}

	if( ext(source) != file_ext )
	{
		std::cout << "Error: Source file is not an " << file_ext << " file " << source.string() << std::endl;

		return false;
	}

	target = source;
	target.replace_filename(source.stem().string() + "." + file_lbl + "." + file_ext);

	return true;
}

inline bool arg3(char* argv[])
{
	if( !arg2(argv) ) return false;

	const Path path(argv[2]);

	if( is_directory(path) )
	{
		if( path == source.parent_path() )
			return true;

		target = path / source.filename();

		return true;
	}

	target = path;

	if( ext(target).empty() )
	{
		std::cout << "Error: Target directory is unknown " << target.string() << std::endl;

		return false;
	}

	if( ext(target) != file_ext )
	{
		std::cout << "Error: Target file is not an " << file_ext << " file " << target.string() << std::endl;

		return false;
	}

	if( !is_directory(target.parent_path()) )
	{
		std::cout << "Error: Target file has unknown directory " << target.string() << std::endl;

		return false;
	}

	return true;
}

inline bool arg()
{
	std::cout << "Error argument: Too many arguments" << std::endl;

	return false;
}

inline std::string ext(const Path& path)
{
	std::string ext = path.extension().string();

	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

	if( ext.empty() ) return {};

	return ext.substr(1);
}
