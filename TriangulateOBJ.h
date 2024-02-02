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
#include <map>
#include <vector>
#include <iostream>
#include <stdexcept>

namespace obj
{
	static constexpr float epsilon = 1e-6f;

	struct Count
	{
		bool empty() const { return vertices == 0; }

		size_t vertices = 0;

		std::pair<size_t, size_t> polygons;
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
		Point() : i(0), x(0.0f), y(0.0f), z(0.0f) {}

		Point(const float& x, const float& y, const float& z) : i(0), x(x), y(y), z(z) {}

		size_t i;
		float  x;
		float  y;
		float  z;
	};

	struct Triangle
	{
		Triangle(const Point& p0, const Point& p1, const Point& p2) : p0(p0), p1(p1), p2(p2) {}

		Point p0, p1, p2;
	};

	//-------------------------------------------------------------------------------------------------------

	char* trim(char*);

	bool iseol(const char&);

	bool isspace(const char&);

	bool parse(const char*, Point&, Count&);

	char* parse(char* line, std::vector<Point>&, Count&);

	bool parse(const char*, std::vector<int>&, const std::vector<Point>&, Count&);

	//-------------------------------------------------------------------------------------------------------

	inline Triangulate::~Triangulate() { close(); }

	inline bool Triangulate::triangulate(const std::string& source_obj, const std::string& target_obj)
	{
		close();

		source = fopen(source_obj.c_str(), "rb");

		if( source == nullptr )
		{
			std::cout << "Impossible to open obj file for read!" << std::endl;

			return error();
		}

		if( !can_triangulate() ) return error();

		target = fopen(target_obj.c_str(), "w");

		if( target == nullptr )
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
		constexpr int BUFFER_CHAR = 100'000;

		char buff[BUFFER_CHAR] = {0};

		std::vector<Point> vertex;

		while( fgets(buff, sizeof buff, source) )
		{
			const char* line = parse(buff, vertex, count);

			if( line == nullptr )
				continue;

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

		constexpr int BUFFER_CHAR = 100'000;

		char buff[BUFFER_CHAR] = {0};

		bool vertex(false), polygon(false);

		const std::vector<Point> _;

		while( fgets(buff, sizeof buff, source) )
		{
			const char* line = trim(buff);

			if( !polygon && *line == 'f' && *(line + 1) == ' ' )
			{
				std::vector<int> indices;

				if( !parse(line + 2, indices, _, temp) )
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

	//-------------------------------------------------------------------------------------------------------

	inline bool strtoi(const char* text, int& i, const char*& end)
	{
		static int v;

		static const char* p;

		static bool negative;

		p = text;

		negative = false;

		while( *p == ' ' || *p == '\t' ) p++;

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

		while( *p == ' ' || *p == '\t' ) p++;

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

		e++;

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

		point.i = count.vertices++;

		return true;
	}

	inline int listIndex(int index, int listSize)
	{
		return index > 0 ? index - 1 : index + listSize;
	}

	inline bool parse(const char* line, std::vector<int>& indices, const std::vector<Point>& vertex, Count& count)
	{
		static int index, size;

		size = static_cast<int>(vertex.size());

		while( !iseol(*line) )
		{
			if( !strtoi(line, index, line) )
				return false;

			index = listIndex(index, size);

			indices.emplace_back(index);

			while( !isspace(*line) && !iseol(*line) )
				line++;
		}

		return true;
	}

	char* triangulate(char* line, const std::vector<int>&, std::vector<Point>&, Count&);

	inline char* parse(char* line, std::vector<Point>& vertex, Count& count)
	{
		line = trim(line);

		if( *line == 'f' && *(line + 1) == ' ' )
		{
			std::vector<int> indices;

			if( !parse(line + 2, indices, vertex, count) )
				return nullptr;

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

	std::vector<Triangle> triangulate(std::vector<Point>&);

	inline char* triangulate(char* line, const std::vector<int>& indices, std::vector<Point>& vertex, Count& count)
	{
		if( line == nullptr || *line != 'f' )
			return nullptr;

		const auto initialCountOfIndices = indices.size();

		if( initialCountOfIndices < 3 )
			return nullptr;

		if( initialCountOfIndices == 3 )
			count.triangles.first++;

		if( initialCountOfIndices > 3 )
			count.polygons.first++;

		char* lineStart = line;

		line++;

		std::string text;

		std::map<size_t, std::string> index_word;

		const auto size = static_cast<int>(vertex.size());

		while( strtoword(line, text, line) )
		{
			int index;

			const char* word = text.c_str();

			if( !strtoi(word, index, word) )
				return nullptr;

			index = listIndex(index, size);

			if( index_word.count(index) == 0 )
				index_word[index] = text;
		}

		if( initialCountOfIndices > 3 )
			count.polygons.second++;

		std::vector<Point> polygon;

		for( const auto& index : indices )
		{
			if( index < size )
				polygon.emplace_back(vertex[index]);
		}

		if( indices.size() < 3 )
			return nullptr;
				
		const std::vector<Triangle> triangles = triangulate(polygon);

		if( triangles.empty() )
			return nullptr;

		line = lineStart;

		for( const auto& triangle : triangles )
		{
			*line++ = 'f';

			*line++ = ' ';
			for( const auto& c : index_word[triangle.p0.i] ) *line++ = c;

			*line++ = ' ';
			for( const auto& c : index_word[triangle.p1.i] ) *line++ = c;

			*line++ = ' ';
			for( const auto& c : index_word[triangle.p2.i] ) *line++ = c;

			*line++ = '\n';

			count.triangles.second++;
		}

		*(--line) = '\0';

		return lineStart;
	}

	//-------------------------------------------------------------------------------------------------------

	enum class TurnDirection
	{
		Right  = 1,
		Left   = -1,
		NoTurn = 0
	};

	inline Point operator-(const Point& u, const Point& v)
	{
		return {u.x - v.x , u.y - v.y , u.z - v.z};
	}

	inline Point operator/(const Point& u, const float div)
	{
		if( div == 0.0f ) return {0.0f , 0.0f , 0.0f};

		return {u.x / div , u.y / div , u.z / div};
	}

	inline bool operator==(const Point& u, const Point& v)
	{
		if( fabs(u.x - v.x) > epsilon ) return false;
		if( fabs(u.y - v.y) > epsilon ) return false;
		if( fabs(u.z - v.z) > epsilon ) return false;

		return true;
	}

	inline Point cross(const Point& u, const Point& v)
	{
		return {u.y * v.z - u.z * v.y , u.z * v.x - u.x * v.z , u.x * v.y - u.y * v.x};
	}

	inline double dot(const Point& u, const Point& v)
	{
		return u.x * v.x + u.y * v.y + u.z * v.z;
	}

	inline float length(const Point& u)
	{
		return std::sqrt(u.x * u.x + u.y * u.y + u.z * u.z);
	}

	inline Point normalize(const Point& point)
	{
		return point / obj::length(point);
	}

	inline TurnDirection turn(const Point& p, const Point& u, const Point& n, const Point& q)
	{
		const auto v = cross(q - p, u);

		const auto dot = obj::dot(v, n);

		if( dot > +0.001 ) return TurnDirection::Right;
		if( dot < -0.001 ) return TurnDirection::Left;

		return TurnDirection::NoTurn;
	}

	inline float triangleAreaSquared(const Point& a, const Point& b, const Point& c)
	{
		const auto cross = obj::cross(b - a, c - a);

		return (cross.x * cross.x + cross.y * cross.y + cross.z * cross.z) / 4.0f;
	}

	//-------------------------------------------------------------------------------------------------------

	inline Point normal(const std::vector<Point>& polygon) //Newell's method
	{
		Point normal;

		const auto n = polygon.size();

		if( n < 3 ) return normal;

		for( size_t index = 0; index < n; index++ )
		{
			const Point& item = polygon[index % n];
			const Point& next = polygon[(index + 1) % n];

			normal.x += (next.y - item.y) * (next.z + item.z);
			normal.y += (next.z - item.z) * (next.x + item.x);
			normal.z += (next.x - item.x) * (next.y + item.y);
		}

		return normalize(normal);
	}

	inline bool convex(const std::vector<Point>& polygon, const Point& normal)
	{
		const auto n = polygon.size();

		if( n < 3 ) return false;

		if( n == 3 ) return true;

		auto polygonTurn = TurnDirection::NoTurn;

		for( size_t index = 0; index < n; index++ )
		{
			const auto& prev = polygon[(index - 1 + n) % n];
			const auto& item = polygon[index % n];
			const auto& next = polygon[(index + 1) % n];

			const auto u = normalize(item - prev);

			const auto itemTurn = turn(prev, u, normal, next);

			if( itemTurn == TurnDirection::NoTurn )
				continue;

			if( polygonTurn == TurnDirection::NoTurn )
				polygonTurn = itemTurn;

			if( polygonTurn != itemTurn )
				return false;
		}

		return true;
	}

	inline bool clockwiseOriented(const std::vector<Point>& polygon, const Point& normal)
	{
		const auto n = polygon.size();

		if( n < 3 ) return false;

		double orientationSum(0.0);

		for( size_t index = 0; index < n; index++ )
		{
			const auto& prev = polygon[(index - 1 + n) % n];
			const auto& item = polygon[index % n];
			const auto& next = polygon[(index + 1) % n];

			const auto& edge        = item - prev;
			const auto& toNextPoint = next - item;

			const auto cross = obj::cross(edge, toNextPoint);

			orientationSum += dot(cross, normal);
		}

		return orientationSum < 0.0;
	}

	inline void makeClockwiseOrientation(std::vector<Point>& polygon, const Point& normal)
	{
		if( polygon.size() < 3 ) return;

		if( !clockwiseOriented(polygon, normal) )
			polygon = {polygon.rbegin() , polygon.rend()};
	}

	inline bool pointInsideOrEdgeTriangle(const Point& a, const Point& b, const Point& c, const Point& p, bool& edge)
	{
		static double zero = std::numeric_limits<double>::epsilon();

		// Initialize edge to false
		edge = false;

		// Vectors from point p to vertices of the triangle
		const auto v0 = c - a;
		const auto v1 = b - a;
		const auto v2 = p - a;

		const auto dot00 = dot(v0, v0);
		const auto dot01 = dot(v0, v1);
		const auto dot02 = dot(v0, v2);
		const auto dot11 = dot(v1, v1);
		const auto dot12 = dot(v1, v2);

		// Check for degenerate triangle
		const auto denom = dot00 * dot11 - dot01 * dot01;

		if( std::abs(denom) < zero )
		{
			// The triangle is degenerate (i.e., has no area)
			return false;
		}

		// Compute barycentric coordinates
		const auto invDenom = 1.0 / denom;

		const auto u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		const auto v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		// Check for edge condition
		if( std::abs(u) < zero || std::abs(v) < zero || std::abs(u + v - 1) < zero )
			edge = true;

		// Check if point is inside the triangle (including edges)
		return (u >= 0.0) && (v >= 0.0) && (u + v < 1.0);
	}

	inline void removeConsecutiveEqualItems(std::vector<Point>& list)
	{
		const auto n = list.size();

		const std::vector copy(std::move(list));

		list.clear();

		for( size_t index = 0; index < n; index++ )
		{
			const Point& item = copy[index % n];
			const Point& next = copy[(index + 1) % n];

			if( item.i == next.i ) continue;

			list.emplace_back(item);
		}
	}

	//-------------------------------------------------------------------------------------------------------

	inline bool isEar(const int index, const std::vector<Point>& polygon, const Point& normal)
	{
		const auto n = polygon.size();

		if( n < 3 ) return false;

		if( n == 3 ) return true;

		bool edge(false);

		const auto prevIndex = (index - 1 + n) % n;
		const auto itemIndex = index % n;
		const auto nextIndex = (index + 1) % n;

		const Point& prev = polygon[prevIndex];
		const Point& item = polygon[itemIndex];
		const Point& next = polygon[nextIndex];

		const auto u = normalize(item - prev);

		if( turn(prev, u, normal, next) != TurnDirection::Right )
			return false;

		for( size_t i = 0; i < polygon.size(); i++ )
		{
			if( i == prevIndex ) continue;
			if( i == itemIndex ) continue;
			if( i == nextIndex ) continue;

			if( pointInsideOrEdgeTriangle(prev, item, next, polygon[i], edge) )
				return false;
		}

		return true;
	}

	inline int getBiggestEar(const std::vector<Point>& polygon, const Point& normal)
	{
		const auto n = static_cast<int>(polygon.size());

		if( n == 3 ) return 0;

		if( n == 0 ) return -1;

		int maxIndex(-1);

		double maxArea(DBL_MIN);

		for( int index = 0; index < n; index++ )
		{
			if( isEar(index, polygon, normal) )
			{
				const Point& prev = polygon[(index - 1 + n) % n];
				const Point& item = polygon[index % n];
				const Point& next = polygon[(index + 1) % n];

				const auto area = triangleAreaSquared(prev, item, next);

				if( area > maxArea )
				{
					maxIndex = index;

					maxArea = area;
				}
			}
		}

		return maxIndex;
	}

	inline int getOverlappingEar(const std::vector<Point>& polygon, const Point& normal)
	{
		const auto n = static_cast<int>(polygon.size());

		if( n == 3 ) return 0;

		if( n == 0 ) return -1;

		for( int index = 0; index < n; index++ )
		{
			const Point& prev = polygon[(index - 1 + n) % n];
			const Point& item = polygon[index % n];
			const Point& next = polygon[(index + 1) % n];

			const auto u = normalize(item - prev);

			if( turn(prev, u, normal, next) != TurnDirection::NoTurn )
				continue;

			const auto v = normalize(next - item);

			if( dot(u, v) < 0.0 ) //Opposite direction -> ear
				return index;
		}

		return -1;
	}

	//-------------------------------------------------------------------------------------------------------

	inline std::vector<Triangle> fanTriangulation(std::vector<Point>& polygon)
	{
		std::vector<Triangle> triangles;

		for( size_t index = 1; index < polygon.size() - 1; ++index )
			triangles.emplace_back(polygon[0], polygon[index], polygon[index + 1]);

		return triangles;
	}

	inline std::vector<Triangle> cutTriangulation(std::vector<Point>& polygon, const Point& normal)
	{
		std::vector<Triangle> triangles;

		makeClockwiseOrientation(polygon, normal);

		while( !polygon.empty() )
		{
			int index = getBiggestEar(polygon, normal);

			if( index == -1 )
				index = getOverlappingEar(polygon, normal);

			if( index == -1 )
				return {};

			const auto n = polygon.size();

			const Point& prev = polygon[(index - 1 + n) % n];
			const Point& item = polygon[index % n];
			const Point& next = polygon[(index + 1) % n];

			triangles.emplace_back(prev, item, next);

			polygon.erase(polygon.begin() + index);

			if( polygon.size() < 3 ) break;
		}

		return polygon.size() == 2 ? triangles : std::vector<Triangle>();
	}

	inline std::vector<Triangle> triangulate(std::vector<Point>& polygon)
	{
		removeConsecutiveEqualItems(polygon);

		if( polygon.size() < 3 ) return {};

		if( polygon.size() == 3 )
		{
			std::vector<Triangle> triangle;

			triangle.emplace_back(polygon[0], polygon[1], polygon[2]);

			return triangle;
		}

		const auto normal = obj::normal(polygon);

		return convex(polygon, normal) ? fanTriangulation(polygon) : cutTriangulation(polygon, normal);
	}

	//-------------------------------------------------------------------------------------------------------

	inline bool Triangulate::write_header(const std::string& source_obj)
	{
		if( fseek(target, 0, SEEK_SET) != 0 ) return error();

		fprintf(target, "# Triangulated OBJ File\n");
		fprintf(target, "# File Triangulated by FalconCoding (https://github.com/StefanJohnsen)\n");
		fprintf(target, "\n");
		fprintf(target, "# Original file name : %s\n", filename(source_obj).c_str());
		fprintf(target, "#          Vertices  : %zu\n", count.vertices);
		fprintf(target, "#          Polygons  : %zu\n", count.polygons.first);
		fprintf(target, "#          Triangles : %zu\n", count.triangles.first);
		fprintf(target, "\n");

		fprintf(target, "# This triangulated file\n");
		fprintf(target, "#          Polygons  : %zu    %zu\n", count.polygons.first, count.polygons.second - count.polygons.first);
		fprintf(target, "#          Triangles : %zu    %zu\n", count.triangles.first, count.triangles.second - count.triangles.first);
		fprintf(target, "\n");

		fprintf(target, "# Total triangles after triangulations : %zu\n", count.triangles.first + count.triangles.second);

		if( !count.empty() ) return true;

		fprintf(target, "%s\n", buffer(5).c_str());
		fprintf(target, "# Please note that any comments regarding the number of triangles and faces below,\n");
		fprintf(target, "# originating from the original file, will be incorrect for this triangulated file.\n");
		fprintf(target, "# Please update or remove old metrics information.\n");
		fprintf(target, "#%s\n", std::string(100, '_').c_str());
		fprintf(target, "#\n");
		fprintf(target, "\n");

		return true;
	}
}

#endif // TRIANGULATE_OBJ
