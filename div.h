#pragma once

/*
  div.h - Helper class for cmd.h + out.h

  NB: TriangulateOBJ.h has no dependencies to this file.
*/

#include <string>
#include <locale>

#include <algorithm>
#include <filesystem>

class coutLocaleGuard
{
public:

	coutLocaleGuard(const std::locale& newLocale) : originalLocale(std::cout.getloc())
	{
		std::cout.imbue(newLocale);
	}

	virtual ~coutLocaleGuard()
	{
		std::cout.imbue(originalLocale);
	}

private:

	std::locale originalLocale;
};

struct thousandsFacet final : std::numpunct<char>
{
	explicit thousandsFacet(const size_t refs = 0) : std::numpunct<char>(refs) {}

	char do_thousands_sep() const override { return '.'; }

	std::string do_grouping() const override { return "\003"; }
};

inline std::string ext(const std::filesystem::path& path)
{
	std::string ext = path.extension().string();

	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

	if( ext.empty() ) return {};

	return ext.substr(1);
}
