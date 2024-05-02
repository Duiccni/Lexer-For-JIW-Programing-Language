#undef NDEBUG
#include <iostream>
#include <cassert>
#include <fstream>
#include <cstdint>
#include <cstdlib>
#include <vector>

inline static char* findFirst(char* str, char c)
{
	char* _c = str;
	for (; *_c != '\0'; _c++)
		if (*_c == c) return _c;
	return _c;
}

inline static char* findFirst(char* str, const char* chars)
{
	char* _c = str;
	for (; *_c != '\0'; _c++)
		for (const char* c = chars; *c != '\0'; c++)
			if (*_c == *c) return _c;
	return _c;
}

inline static char* findFirstNot(char* str, const char* chars)
{
	char* _c = str;
	for (; *_c != '\0'; _c++)
		for (const char* c = chars; *c != '\0'; c++)
			if (*_c != *c) return _c;
	return _c;
}

inline static char* findFirst(char* str, int f(int))
{
	char* _c = str;
	for (; *_c != '\0'; _c++)
		if (f(*_c)) return _c;
	return _c;
}

inline static char* findFirstNot(char* str, int f(int))
{
	char* _c = str;
	for (; *_c != '\0'; _c++)
		if (!f(*_c)) return _c;
	return _c;
}

#pragma pack(2)
struct String
{
	char* start;
	uint16_t length;

	template <typename T>
	inline T applyAsNormalString(T f(const char*)) const
	{
		char ei = start[length];
		start[length] = '\0';
		T v = f(start);
		start[length] = ei;
		return v;
	}

	inline void print() const
	{
		for (uint16_t i = 0; i < length; i++)
			putchar(start[i]);
	}

	inline long long toInt() const
	{
		return applyAsNormalString(atoll);
	}

	inline double toDouble() const
	{
		return applyAsNormalString(atof);
	}
};

inline static void todo(const char* str)
{
	std::cout << "TODO: " << str;
	abort();
}

namespace tokenKinds
{
	constexpr uint8_t
		number = 0,
		alpha = 1,
		solo = 2,
		string = 3;

	const char* names[] = {
		"number",
		"alpha",
		"solo",
		"\033[32mstring"
	};
}

struct Token
{
	String str;
	uint8_t kind;

	inline void print() const
	{
		std::cout << tokenKinds::names[kind] << "\033[0m\t'\033[36m";

		str.print();

		std::cout << "\033[0m'\n";
	}
};

int main()
{
	std::vector<Token> tokens;

	std::ifstream file("test.jiw");
	assert(file.is_open());

	file.seekg(0, std::ios::end);
	size_t fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	char* buffer = (char*)malloc(fileSize);
	assert(buffer != nullptr);

	file.read(buffer, fileSize);
	file.close();

	uint8_t kind;
	char* end = nullptr;
	for (char* c = buffer; *c != '\0';)
	{
		if (isspace(*c))
		{
			c++;
			continue;
		}
		else if (isdigit(*c))
		{
			end = findFirstNot(c + 1, isdigit);
			kind = tokenKinds::number;
		}
		else if (isalpha(*c))
		{
			end = findFirstNot(c + 1, isalnum);
			kind = tokenKinds::alpha;
		}
		else
		{
			if (*c == '/')
			{
				if (c[1] == '/')
				{
					c = findFirst(c + 2, '\n') + 1;
					continue;
				}
				else if (c[1] == '*')
				{
					c++;
					while (true)
					{
						c = findFirst(c + 1, '*');
						if (c[1] == '/')
							break;
					}
					c += 2;
					continue;
				}
			}

			tokens.push_back({ { c, 1 }, tokenKinds::solo });

			if (*c == '"')
			{
				end = findFirst(++c, '"');
				tokens.push_back({ { c, static_cast<uint16_t>(end - c) }, tokenKinds::string });
				tokens.push_back({ { c = end, 1 }, tokenKinds::solo });
			}

			c++;
			continue;
		}

		tokens.push_back({ { c, static_cast<uint16_t>(end - c) }, kind });
		c = end;
	}

	for (Token& token : tokens)
	{
		token.print();
	}
}