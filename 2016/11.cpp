#include <sstream>
#include <iomanip>
#include <array>
#include <unordered_map>
#include <cassert>
#include <queue>
#include <algorithm>
#include "../test.hpp"

namespace {

template <int COUNT>
struct State
{
	// even - generators
	// odd - microchips
	std::array<int, COUNT> items;
	int elevator;

	static constexpr unsigned long TARGET_HASH = (1 << 2*(COUNT + 1)) - 1;

	State()
		: elevator{0}
	{
		static_assert(2*(COUNT+1) <= sizeof(unsigned long)*8, "COUNT is too high");
		items.fill(0);
	}

	unsigned long Hash() const
	{
		unsigned long v = 0;
		for (auto i : items)
		{
			v <<= 2;
			v |= i;
		}
		v <<= 2;
		v |= elevator;

		return v;
	}

	explicit State(unsigned long v)
	{
		elevator = v & 3;
		v >>= 2;
		for (int i = COUNT - 1; i >= 0; --i)
		{
			items[i] = v & 3;
			v >>= 2;
		}
	}

	std::string Dump(const std::vector<const char *> &names) const
	{
		std::ostringstream oss;

		for (int floor = 3; floor >= 0; --floor)
		{
			oss << std::setw(3) << ("F" + std::to_string(floor+1));
			oss << std::setw(3) << (elevator == floor ? " E " : " . ");
			for (int i = 0; i < COUNT; ++i)
				oss << std::setw(3) << (items[i] == floor ? names[i] : " . ");
			oss << "\n";
		}
		return oss.str();
	}

	bool IsConsistent(int floor)
	{
		bool has_generators = false;
		for (int i = 0; i < COUNT; i += 2)
		{
			if (items[i] == floor)
			{
				has_generators = true;
				break;
			}
		}

		if (!has_generators)
			return true;

		for (int i = 0; i < COUNT; i += 2)
		{
			if (items[i + 1] == floor && items[i] != floor)
				return false;
		}

		return true;
	}

	bool CanCarryTogether(int i, int j) const
	{
		// We can't carry a chip (number is odd) with a foreign generator (not a preceding number)
		bool j_is_chip = (j & 1) == 1;
		bool i_is_chip = (i & 1) == 1;

		if (j_is_chip)
		{
			if (i_is_chip)
				return true;
			else
				return i+1 == j;
		}
		else
		{
			return !i_is_chip;
		}
	}
};

template <int COUNT>
int Solve(const std::vector<int> &configuration,
		  const std::vector<const char *> &names)
{
	// map current_state -> previous_state
	std::unordered_map<unsigned long, unsigned long> space;
	std::queue<std::pair<int, unsigned long>> to_search;

	State<COUNT> initial_state;
	for (int i = 0; i < COUNT; ++i)
		initial_state.items[i] = configuration[i];

	to_search.emplace(0, initial_state.Hash());

	auto dump_solution = [&](unsigned long state, int distance)
	{
		//vector<unsigned long> s;
		//s.emplace_back(state);

		//while (true)
		//{
		//	auto prev = space[s.back()];
		//	s.emplace_back(prev);
		//	if (prev == initial_state.Hash())
		//		break;
		//}

		//while (!s.empty())
		//{
		//	cout << State<COUNT>{s.back()}.Dump(names) << endl;
		//	s.pop_back();
		//}

		return distance;
	};

	while (!to_search.empty())
	{
		int distance = to_search.front().first;
		State<COUNT> state(to_search.front().second);
		to_search.pop();

		int floor = state.elevator;

		// Try to move up and down
		for (int direction : { -1, 1 })
		{
			int next_floor = floor + direction;
			if (next_floor < 0 || next_floor > 3)
				continue;

			// Try to move every item
			for (int i = 0; i < COUNT; ++i)
			{
				if (state.items[i] != floor)
					continue;

				State<COUNT> new_state{state};
				new_state.elevator = next_floor;
				new_state.items[i] = next_floor;
				auto hash = new_state.Hash();
				if (space.find(hash) == space.end())
				{
					if (new_state.IsConsistent(floor) && new_state.IsConsistent(next_floor))
					{
						if (hash == State<COUNT>::TARGET_HASH)
							return dump_solution(hash, distance + 1);
						space.insert({hash, state.Hash()});
						to_search.emplace(distance + 1, hash);
					}
				}
			}

			// Try to move pair of items
			for (int i = 0; i < COUNT - 1; ++i)
			{
				if (state.items[i] != floor)
					continue;
				for (int j = i + 1; j < COUNT; ++j)
				{
					if (state.items[j] != floor)
						continue;
					if (!state.CanCarryTogether(i, j))
						continue;

					State<COUNT> new_state{state};
					new_state.elevator = next_floor;
					new_state.items[i] = next_floor;
					new_state.items[j] = next_floor;
					auto hash = new_state.Hash();
					if (space.find(hash) == space.end())
					{
						if (new_state.IsConsistent(floor) && new_state.IsConsistent(next_floor))
						{
							if (hash == State<COUNT>::TARGET_HASH)
								return dump_solution(hash, distance + 1);
							space.insert({hash, state.Hash()});
							to_search.emplace(distance + 1, hash);
						}
					}
				}
			}
		}
	}

	return -1;
}

using namespace boost::ut;

suite s = [] {
	"11"_test = [] {
		// The first floor contains a hydrogen-compatible microchip and a lithium-compatible microchip.
		// The second floor contains a hydrogen generator.
		// The third floor contains a lithium generator.
		// The fourth floor contains nothing relevant.

		// 0: hydorgen
		// 1: lithium
		expect(11_i == Solve<4>({1, 0, 2, 0}, {"HG", "HM", "LG", "LM"}));

		// The first floor contains a thulium generator, a thulium-compatible microchip, a plutonium generator, and a strontium generator.
		// The second floor contains a plutonium-compatible microchip and a strontium-compatible microchip.
		// The third floor contains a promethium generator, a promethium-compatible microchip, a ruthenium generator, and a ruthenium-compatible microchip.
		// The fourth floor contains nothing relevant.

		if (Printer::EnableAll())
		{
			// 0: thulium
			// 1: plutonium
			// 2: strotium
			// 3: promethium
			// 4: ruthenium
			Printer::Print(__FILE__, "1", Solve<10>({0, 0, 0, 1, 0, 1, 2, 2, 2, 2}, {}));

			// <...>
			// 5: elerium
			// 6: dilithium
			Printer::Print(__FILE__, "2", Solve<14>({0, 0, 0, 1, 0, 1, 2, 2, 2, 2, 0, 0, 0, 0}, {}));
		}
	};
};

} //namespace;
