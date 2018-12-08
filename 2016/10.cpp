#include <doctest/doctest.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <cassert>
#include <boost/lexical_cast.hpp>

using boost::lexical_cast;

namespace {

struct Dest
{
	enum {
		BOT,
		OUTPUT
	} where;
	int number;

	std::string Dump() const
	{
		return (where == BOT ? "b" : "o") + std::to_string(number);
	}
};

struct Instr
{
	Dest low;
	Dest high;

	std::string Dump() const
	{
		return low.Dump() + " " + high.Dump();
	}
};

struct BotInstr
{
	std::vector<int> chips;
	Instr instr;

	std::string Dump() const
	{
		std::string ret;
		const char *comma = "";
		for (auto v : chips)
		{
			ret += comma + std::to_string(v);
			comma = ",";
		}
		ret += "\t" + instr.Dump();
		return ret;
	}
};

typedef std::vector<BotInstr> FactoryT;

std::string Dump(const FactoryT &f)
{
	std::string ret;
	for (const auto &bot : f)
		ret += bot.Dump() + '\n';
	return ret;
}

FactoryT Parse(std::istream &&is)
{
	FactoryT factory;

	auto get_bot = [&factory](size_t bot) -> FactoryT::value_type&
	{
		if (factory.size() == bot)
			factory.emplace_back();
		else if (factory.size() < bot)
			factory.resize(bot + 1);
		return factory[bot];
	};

	const static std::regex initr(R"(value (\d+) goes to bot (\d+))");
	const static std::regex instrr(R"(bot (\d+) gives low to (bot|output) (\d+) and high to (bot|output) (\d+))");

	std::string line;
	while (getline(is, line))
	{
		std::smatch match;

		if (regex_match(line, match, initr))
		{
			assert(match.size() == 3);
			auto &bot = get_bot(lexical_cast<int>(match[2].str()));
			bot.chips.emplace_back(lexical_cast<int>(match[1].str()));
		}
		else if (regex_match(line, match, instrr))
		{
			assert(match.size() == 6);
			auto &bot = get_bot(lexical_cast<int>(match[1].str()));
			bot.instr.low = {
				match[2].str() == "bot" ? Dest::BOT : Dest::OUTPUT,
				lexical_cast<int>(match[3].str())
			};
			bot.instr.high = {
				match[4].str() == "bot" ? Dest::BOT : Dest::OUTPUT,
				lexical_cast<int>(match[5].str())
			};
		}
	}

	return factory;
}

template <typename BotFilter, typename OutFilter>
bool Solve(FactoryT &factory, BotFilter bot_action, OutFilter out_filter)
{
	bool has_change = true;

	while (has_change)
	{
		has_change = false;

		for (size_t i = 0; i != factory.size(); ++i)
		{
			auto &bot = factory[i];

			assert(bot.chips.size() < 3);
			if (bot.chips.size() == 2)
			{
				has_change = true;
				sort(begin(bot.chips), end(bot.chips));
				if (bot_action(i, bot))
					return true;

				const auto &il = bot.instr.low;
				if (il.where == Dest::BOT)
					factory[il.number].chips.push_back(bot.chips[0]);
				else
					if (out_filter(il.number, bot.chips[0]))
						return true;

				const auto &ih = bot.instr.high;
				if (ih.where == Dest::BOT)
					factory[ih.number].chips.push_back(bot.chips[1]);
				else
					if (out_filter(ih.number, bot.chips[1]))
						return true;

				bot.chips.clear();
			}
		}
	}

	return false;
}

int Solve1(FactoryT &factory, int target_l, int target_h)
{
	int result{-1};

	auto filt = [&](int bot_number, const BotInstr &bot)
	{
		if (bot.chips[0] == target_l && bot.chips[1] == target_h)
		{
			result = bot_number;
			return true;
		}
		return false;
	};

	auto noout = [](int, int) -> bool { return false; };
	if (!Solve(factory, filt, noout))
		return -1;

	return result;
}

int Solve2(FactoryT &factory)
{
	auto nobot = [](int, const BotInstr &) { return false; };

	std::array<int, 3> outs{ {-1, -1, -1} };
	int count = 3;

	auto outfilt = [&](int out, int val)
	{
		if (out < 3 && outs[out] == -1)
		{
			outs[out] = val;
			return --count == 0;
		}
		return false;
	};

	if (!Solve(factory, nobot, outfilt))
		return -1;

	return outs[0] * outs[1] * outs[2];
}

} //namespace;

TEST_CASE(TEST_NAME)
{
	const char *const test =
R"(value 5 goes to bot 2
bot 2 gives low to bot 1 and high to bot 0
value 3 goes to bot 1
bot 1 gives low to output 1 and high to bot 0
bot 0 gives low to output 2 and high to output 0
value 2 goes to bot 2)";

	auto f = Parse(std::istringstream{test});

	FactoryT fref = {
		{ {}, {{Dest::OUTPUT, 2}, {Dest::OUTPUT, 0}} },
		{ {3}, {{Dest::OUTPUT, 1}, {Dest::BOT, 0}} },
		{ {5,2}, {{Dest::BOT, 1}, {Dest::BOT, 0}} },
	};
	REQUIRE(Dump(f) == Dump(fref));
	REQUIRE(Solve1(f, 2, 5) == 2);

	auto factory = Parse(std::ifstream{INPUT});
	MESSAGE(Solve1(factory, 17, 61));

	MESSAGE(Solve2(factory));
}
