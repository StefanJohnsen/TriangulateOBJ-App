#pragma once
/*
  TriangulateOBJ.h

  Wavefront OBJ - C++ Class for triangulating obj file

  Copyright (c) 2023 FalconCoding

  This software is released under the terms of the
  GNU General Public License v3.0. Details and terms of this
  license can be found at: https://www.gnu.org/licenses/gpl-3.0.html
  For those who require the freedom to operate without the
  constraints of the GPL, a commercial license can be obtaining by
  contacting the author at stefan.johnsen@outlook.com
 */

#ifndef TRIANGULATE_OBJ
#define TRIANGULATE_OBJ

#include <string>
#include <cmath>
#include <vector>
#include <iostream>

namespace obj
{
	struct Count
	{
		size_t oldTriangles() const { return triangles.first; }

		size_t newTriangles() const { return triangles.second; }

		size_t sumTriangles() const { return oldTriangles() + newTriangles(); }

		bool empty() const { return Vertices == 0; }

		size_t Vertices = 0;
		size_t polygons = 0;

		std::pair<size_t, size_t> triangles;
	};

	class Triangulate
	{
	public:

		explicit Triangulate() : source(nullptr), target(nullptr) {}

		~Triangulate();

		Triangulate(const Triangulate&) = delete;

		Triangulate(const Triangulate&&) = delete;

		Triangulate& operator=(const Triangulate&) = delete;

		Triangulate& operator=(const Triangulate&&) = delete;

		bool triangulate(const std::string& source_obj, const std::string& target_obj);

		const Count& metrics() const { return count; }

		bool empty() const { return count.empty(); }

	private:

		Count count;

		bool triangulate();

		bool can_triangulate();

		bool write_header(const std::string&);

		void close();

		bool error();

		FILE* source;
		FILE* target;
	};

	//-------------------------------------------------------------------------------------------------------

	struct Point
	{
		size_t i = 0;
		float  x = 0.0f;
		float  y = 0.0f;
		float  z = 0.0f;
	};

	//-------------------------------------------------------------------------------------------------------

	char* trim(char*);

	bool iseol(const char&);

	bool isspace(const char&);

	bool parse(const char*, Point&, Count&);

	bool parse(const char*, std::vector<int>&, size_t, Count&);

	char* parse(char* line, std::vector<Point>& vertex, Count&);

	//-------------------------------------------------------------------------------------------------------

	inline Triangulate::~Triangulate() { close(); }

	inline bool Triangulate::triangulate(const std::string& source_obj, const std::string& target_obj)
	{
		close();

		this->source = fopen(source_obj.c_str(), "rb");

		if( this->source == nullptr )
		{
			std::cout << "Impossible to open obj file for read!" << std::endl;

			return error();
		}

		if( !can_triangulate() ) return error();

		this->target = fopen(target_obj.c_str(), "w");

		if( this->target == nullptr )
		{
			std::cout << "Impossible to open obj file for write!" << std::endl;

			return error();
		}

		if( !write_header(source_obj) ) return error();
		if( !triangulate() ) return error();
		if( !write_header(source_obj) ) return error();

		return true;
	}

	inline void Triangulate::close()
	{
		if( source ) fclose(source);
		if( target ) fclose(target);

		source = nullptr;
		target = nullptr;
	}

	inline bool Triangulate::error()
	{
		close();

		return false;
	}

	inline bool Triangulate::triangulate()
	{
		constexpr int BUFFER_CHAR = 10'000;

		char buff[BUFFER_CHAR] = {0};

		std::vector<Point> vertex;

		while( fgets(buff, sizeof buff, source) )
		{
			const char* line = parse(buff, vertex, count);

			if( line == nullptr )
				return error();

			if( fputs(line, target) == EOF )
				return error();

			if( fputc('\n', target) == EOF )
				return error();
		}

		return true;
	}

	inline bool Triangulate::can_triangulate()
	{
		Count temp;

		constexpr int BUFFER_CHAR = 10'000;

		char buff[BUFFER_CHAR] = {0};

		bool vertex(false), polygon(false);

		while( fgets(buff, sizeof buff, source) )
		{
			const char* line = trim(buff);

			if( !polygon && *line == 'f' && *(line + 1) == ' ' )
			{
				std::vector<int> indices;

				if( !parse(line + 2, indices, 0, temp) )
					indices.clear();

				if( indices.size() > 3 )
					polygon = true;
			}

			if( !vertex && *line == 'v' && *(line + 1) == ' ' )
				vertex = true;

			if( vertex && polygon )
				break;
		}

		if( !(vertex && polygon) )
			return error();

		return fseek(source, 0, SEEK_SET) == 0;
	}

	inline std::string filename(const std::string& file)
	{
		const auto find = file.find_last_of("/\\");

		if( find == std::string::npos )
			return file;

		return file.substr(find + 1);
	}

	inline std::string buffer(size_t count)
	{
		static const size_t length = std::to_string(SIZE_MAX).length();

		std::string buffer = std::string(count * length, ' ');

		return buffer;
	}

	inline bool Triangulate::write_header(const std::string& source_obj)
	{
		if( fseek(target, 0, SEEK_SET) != 0 ) return error();

		fprintf(target, "# Triangulated OBJ File\n");
		fprintf(target, "# File Triangulated by FalconCoding (https://github.com/StefanJohnsen)\n");
		fprintf(target, "\n");
		fprintf(target, "# Original File      : %s\n", filename(source_obj).c_str());
		fprintf(target, "#          Vertices  : %zu\n", count.Vertices);
		fprintf(target, "#          Polygons  : %zu\n", count.polygons);
		fprintf(target, "#          Triangles : %zu\n", count.oldTriangles());
		fprintf(target, "\n");
		fprintf(target, "# Triangles created to replace polygons   : %zu\n", count.newTriangles());
		fprintf(target, "# Total triangles of existing and created : %zu\n", count.sumTriangles());

		if( !count.empty() ) return true;

		fprintf(target, "%s\n", buffer(5).c_str()); // buffer space for 5 %zu values
		fprintf(target, "# Please note that any comments regarding the number of triangles and faces below,\n");
		fprintf(target, "# originating from the original file, will be incorrect for this triangulated file.\n");
		fprintf(target, "# Please update or remove old metrics information.\n");
		fprintf(target, "#%s\n", std::string(100, '_').c_str());
		fprintf(target, "#\n");
		fprintf(target, "\n");

		return true;
	}

	//-------------------------------------------------------------------------------------------------------

	inline bool strtoi(const char* text, int& i, const char*& end)
	{
		static int v;

		static const char* p;

		static bool negative;

		p = text;

		negative = false;

		while( *p == ' ' ) p++;

		if( *p == '-' )
		{
			negative = true;

			p++;
		}
		else if( *p == '+' )
			p++;

		v = 0;

		while( *p >= '0' && *p <= '9' )
		{
			v = (v * 10) + (*p - '0');

			p++;
		}

		end = p;

		i = negative ? -v : v;

		return text == end ? false : true;
	}

	inline bool strtof(const char* text, float& d, const char*& end)
	{
		static float v;

		static const char* p;

		static int exponent;

		static float factor;

		static bool negExp;

		static bool negative;

		p = text;

		negative = false;

		while( *p == ' ' ) ++p;

		if( *p == '-' )
		{
			negative = true;

			p++;
		}
		else if( *p == '+' )
			p++;

		v = 0.0;

		while( *p >= '0' && *p <= '9' )
		{
			v = v * 10.0f + static_cast<float>(*p - '0');

			p++;
		}

		if( *p == '.' )
		{
			p++;

			factor = 0.1f;

			while( *p >= '0' && *p <= '9' )
			{
				v += factor * static_cast<float>(*p - '0');

				factor *= 0.1f;

				p++;
			}
		}

		if( *p == 'e' || *p == 'E' )
		{
			++p;

			exponent = 0;

			negExp = false;

			if( *p == '-' )
			{
				negExp = true;

				p++;
			}
			else if( *p == '+' )
			{
				p++;
			}

			while( *p >= '0' && *p <= '9' )
			{
				exponent = exponent * 10 + (*p - '0');

				p++;
			}

			v *= static_cast<float>(pow(10.0, negExp ? -exponent : exponent));
		}

		end = p;

		d = negative ? -v : v;

		return text == end ? false : true;
	}

	inline bool strtoword(char* text, std::string& word, char*& end)
	{
		static char* p;
		static char* e;

		p = text;

		while( isspace(*p) ) p++;

		e = p;

		while( !isspace(*e) && !iseol(*e) ) e++;

		end = e;

		word = std::string(p, e);

		return text == end || word.empty() ? false : true;
	}
	//-------------------------------------------------------------------------------------------------------

	inline bool iseol(const char& c)
	{
		return c == '\r' || c == '\n' || c == '\0';
	}

	inline bool isspace(const char& c)
	{
		return c == ' ' || c == '\t' || c == '\v';
	}

	inline char* trim(char* p)
	{
		static char* e;

		if( p == nullptr ) return nullptr;

		while( std::isspace(*p) ) p++;	

		e = p;

		while( *e != '\0' ) e++;

		if( e != p ) e--;

		while( std::isspace(*e) && e != p ) e--;

		if( e != p ) e++;

		*e = '\0';

		return p;
	}

	//-------------------------------------------------------------------------------------------------------

	inline bool parse(const char* line, Point& point, Count& count)
	{
		if( !strtof(line, point.x, line) )
			return false;

		if( !strtof(line, point.y, line) )
			return false;

		if( !strtof(line, point.z, line) )
			return false;

		count.Vertices++;

		return true;
	}

	inline bool parse(const char* line, std::vector<int>& indices, const size_t vertexSize, Count& count)
	{
		static int i, v;

		v = static_cast<int>(vertexSize);

		while( !iseol(*line) )
		{
			if( !strtoi(line, i, line) )
				return false;

			i = i > 0 ? i - 1 : i + v;

			indices.emplace_back(i);

			while( !isspace(*line) && !iseol(*line) )
				line++;
		}

		if( indices.size() < 4 )
			count.triangles.first++;

		return true;
	}

	char* triangulate(char* line, const std::vector<int>&, const std::vector<Point>&, Count&);

	inline char* parse(char* line, std::vector<Point>& vertex, Count& count)
	{
		line = trim(line);

		if( *line == 'f' && *(line + 1) == ' ' )
		{
			std::vector<int> indices;

			if( !parse(line + 2, indices, vertex.size(), count) )
				return nullptr;

			if( indices.size() < 4 )
				return line;

			return triangulate(line, indices, vertex, count);
		}

		if( *line == 'v' && *(line + 1) == ' ' )
		{
			Point point;

			if( !parse(line + 2, point, count) )
				return nullptr;

			vertex.emplace_back(point);
		}

		return line;
	}

	std::vector<std::vector<Point>> triangulate(const std::vector<Point>&);

	inline char* triangulate(char* line, const std::vector<int>& indices, const std::vector<Point>& vertex, Count& count)
	{
		char* head = line;

		if( line == nullptr )
			return nullptr;

		if( *line != 'f' )
			return nullptr;

		line++;

		std::string text;

		std::vector<std::string> vertexText;

		while( strtoword(line, text, line) )
			vertexText.emplace_back(text);

		std::vector<Point> polygon;

		const auto size = static_cast<int>(vertex.size());

		for( const auto& index : indices )
		{
			if( index < size )
				polygon.emplace_back(vertex[index]);

			polygon.back().i = polygon.size() - 1;
		}

		if( indices.size() != polygon.size() )
			return nullptr;

		count.polygons++;

		const std::vector<std::vector<Point>> triangles = triangulate(polygon);

		line = head;

		for( const auto& triangle : triangles )
		{
			if( triangle.size() != 3 )
				return nullptr;

			const auto& p0 = triangle[0];
			const auto& p1 = triangle[1];
			const auto& p2 = triangle[2];

			*line++ = 'f';

			*line++ = ' ';
			for( const auto& c : vertexText[p0.i] ) *line++ = c;

			*line++ = ' ';
			for( const auto& c : vertexText[p1.i] ) *line++ = c;

			*line++ = ' ';
			for( const auto& c : vertexText[p2.i] ) *line++ = c;

			*line++ = '\n';

			count.triangles.second++;
		}

		line--;

		*line = '\0';

		return head;
	}

	inline std::vector<std::vector<Point>> triangulate(const std::vector<Point>& polygon)
	{
		if( polygon.size() < 3 ) return {};

		std::vector<std::vector<Point>> triangles;

		for( size_t i = 1; i < polygon.size() - 1; ++i )
			triangles.push_back({polygon[0] , polygon[i] , polygon[i + 1]});

		return triangles;
	}
}

#endif // TRIANGULATE_OBJ
