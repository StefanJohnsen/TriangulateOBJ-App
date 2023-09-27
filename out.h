#pragma once
/*
  out.h - Helper class for file reporting

  NB: TriangulateOBJ.h has no dependencies to this file, only main.cpp is using it.

  This header provides a set of utility functions for handling file operations,
  reporting file sizes and execution times, and working with file paths.

  Copyright (c) 2023 FalconCoding

  This software is released under the terms of the
  GNU General Public License v3.0. Details and terms of this
  license can be found at: https://www.gnu.org/licenses/gpl-3.0.html
*/

#include <string>
#include <chrono>
#include <sstream>
#include <iostream>

#include "cmd.h"
#include "div.h"

#include "TriangulateOBJ.h"

#define indent std::string(5, ' ')

using Clock = std::chrono::high_resolution_clock;

static std::chrono::time_point<Clock> init;

std::string stopwatch();

std::string file_size_info();

inline void report(const obj::Triangulate& obj)
{
	constexpr int n(60);

	if( obj.metrics().empty() ) return;

	const auto v = obj.metrics().vertices;
	const auto t = obj.metrics().triangles;
	const auto p = obj.metrics().polygons;

	coutLocaleGuard localeGuard(std::locale(std::locale(), new thousandsFacet));

	std::cout << indent << std::endl << std::endl;
	std::cout << indent << std::string(n, '-') << std::endl;
	std::cout << indent << target.filename().string() << " " << file_size_info() << std::endl;
	std::cout << indent << std::string(n, '-') << std::endl;
	std::cout << indent << "Vertices              : " << std::setw(10) << v << std::endl;
	std::cout << indent << std::string(n, '-') << std::endl;
	std::cout << indent << "Triangles             : " << std::setw(10) << t.first << std::endl;
	std::cout << indent << "Polygons              : " << std::setw(10) << p.first << std::endl;
	std::cout << indent << std::string(n, '-') << std::endl;
	std::cout << indent << "Triangles    (after)  : " << std::setw(10) << t.first + t.second << "     (+" << t.second << ")" << std::endl;
	std::cout << indent << "Polygons     (after)  : " << std::setw(10) << p.first - p.second << std::endl;
	std::cout << indent << std::string(n, '-') << std::endl;
	std::cout << indent << "Execution time        : " << stopwatch() << std::endl;
	std::cout << indent << std::string(n, '-') << std::endl << std::endl;
}

inline std::string stopwatch(const std::chrono::time_point<Clock>& time, const std::chrono::time_point<Clock>& stop)
{
	using Seconds = std::chrono::seconds;
	using Minutes = std::chrono::minutes;
	using Hours = std::chrono::hours;
	using MilliSeconds = std::chrono::milliseconds;
	using MicroSeconds = std::chrono::microseconds;

	const auto duration = std::chrono::duration_cast<MicroSeconds>(stop - time);

	const auto hours        = std::chrono::duration_cast<Hours>(duration);
	const auto minutes      = std::chrono::duration_cast<Minutes>(duration % Hours(1));
	const auto seconds      = std::chrono::duration_cast<Seconds>(duration % Minutes(1));
	const auto milliseconds = std::chrono::duration_cast<MilliSeconds>(duration % Seconds(1));
	const auto microseconds = std::chrono::duration_cast<MicroSeconds>(duration % MilliSeconds(1));

	std::string result;

	if( minutes.count() > 0 )
	{
		result += (hours.count() < 10 ? "0" : "") + std::to_string(hours.count()) + ":";
		result += (minutes.count() < 10 ? "0" : "") + std::to_string(minutes.count()) + ":";
		result += (seconds.count() < 10 ? "0" : "") + std::to_string(seconds.count());
	}
	else if( seconds.count() > 0 )
		result += std::to_string(seconds.count()) + " seconds ";
	else if( milliseconds.count() > 0 )
		result += std::to_string(milliseconds.count()) + " milliseconds ";
	else
		result += std::to_string(microseconds.count()) + " microseconds";

	return result;
}

inline void launch()
{
	init = Clock::now();
}

inline std::string stopwatch()
{
	return stopwatch(init, Clock::now());
}

inline std::string byte_text(const size_t&);

inline std::string file_size()
{
	struct stat st;

	if( stat(target.string().c_str(), &st) != 0 )
		return {};

	return byte_text(static_cast<size_t>(st.st_size));
}

inline std::string file_extend()
{
	struct stat stSource;

	if( stat(source.string().c_str(), &stSource) != 0 )
		return {};

	struct stat stTarget;

	if( stat(target.string().c_str(), &stTarget) != 0 )
		return {};

	const auto byteSource = static_cast<size_t>(stSource.st_size);
	const auto byteTarget = static_cast<size_t>(stTarget.st_size);

	if( byteSource == byteTarget )
		return {};

	if( byteSource < byteTarget )
		return "+" + byte_text(byteTarget - byteSource);

	return "-" + byte_text(byteSource - byteTarget);
}

inline std::string file_size_info()
{
	return file_size() + "    (" + file_extend() + ")";
}

inline std::string byte_text(const size_t& byte)
{
	const char* sizeUnits[] = {" bytes" , "KB" , "MB" , "GB" , "TB"};

	auto unitIndex = 0;
	auto size      = static_cast<double>(byte);

	while( size >= 1024.0 && unitIndex < 4 )
	{
		size /= 1024.0;
		unitIndex++;
	}

	std::ostringstream result;
	result << std::fixed << std::setprecision(0) << size << sizeUnits[unitIndex];
	return result.str();
}

