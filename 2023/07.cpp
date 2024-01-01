#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;

struct Values
{
    char NORMAL['Z'] = {0};
    char JOKER['Z'] = {0};

    Values()
    {
        for (int ch = '2'; ch <= '9'; ++ch)
        {
            NORMAL[ch] = ch;
            JOKER[ch] = ch;
        }
        NORMAL[(int)'T'] = 'A';
        NORMAL[(int)'J'] = 'B';
        NORMAL[(int)'Q'] = 'C';
        NORMAL[(int)'K'] = 'D';
        NORMAL[(int)'A'] = 'E';
        JOKER[(int)'T'] = 'A';
        JOKER[(int)'J'] = '0';
        JOKER[(int)'Q'] = 'C';
        JOKER[(int)'K'] = 'D';
        JOKER[(int)'A'] = 'E';
    }

} label_values;


class Cards
{
public:
    struct Hand
    {
        std::string card;
        int bid{};
    };

    std::vector<Hand> hands;

    Cards(std::istream &&is)
    {
        Hand hand;
        while (is >> hand.card >> hand.bid)
            hands.push_back(hand);
    }

    int Task1()
    {
        std::sort(hands.begin(), hands.end(), [](const Hand &a, const Hand &b) {
            return CompareCards(a.card, b.card);
        });
        int sum{};
        for (int i = 0; i < hands.size(); ++i)
            sum += (i + 1) * hands[i].bid;
        return sum;
    }

    int Task2()
    {
        std::sort(hands.begin(), hands.end(), [](const Hand &a, const Hand &b) {
            return CompareCardsJoker(a.card, b.card);
        });
        int sum{};
        for (int i = 0; i < hands.size(); ++i)
            sum += (i + 1) * hands[i].bid;
        return sum;
    }

    enum CardType
    {
        NOTHING = 0,
        HIGH_CARD,
        ONE_PAIR,
        TWO_PAIR,
        THREE_OF_A_KIND,
        FULL_HOUSE,
        FOUR_OF_A_KIND,
        FIVE_OF_A_KIND,
    };

    static CardType GetType(const std::string &card)
    {
        std::string vals;
        std::string counts;
        for (char c : card)
        {
            auto idx = vals.find(c);
            if (idx != vals.npos)
            {
                ++counts[idx];
                continue;
            }
            vals.push_back(c);
            counts.push_back(1);
        }
        if (vals.size() == 5)
            return HIGH_CARD;
        if (vals.size() == 4)
            return ONE_PAIR;
        if (vals.size() == 3)
            return counts.find(3) != counts.npos ? THREE_OF_A_KIND : TWO_PAIR;
        if (vals.size() == 2)
            return counts.find(4) != counts.npos ? FOUR_OF_A_KIND : FULL_HOUSE;
        return FIVE_OF_A_KIND;
    }

    static bool CompareCards(std::string a, std::string b)
    {
        auto typeA = GetType(a);
        auto typeB = GetType(b);
        if (typeA < typeB)
            return true;
        if (typeA > typeB)
            return false;
        for (int i = 0; i < 5; ++i)
        {
            auto va = label_values.NORMAL[(int)a[i]];
            auto vb = label_values.NORMAL[(int)b[i]];
            if (va < vb) return true;
            if (va > vb) return false;
        }
        return false;
    }

    static bool CompareCardsJoker(std::string a, std::string b)
    {
        auto best_type = [](std::string card, auto &best_type) {
            auto joker = card.find('J');
            if (joker == card.npos)
                return GetType(card);
            CardType type = NOTHING;
            static constexpr const std::string_view labels = "23456789TQKA";
            for (auto sub : labels)
            {
                card[joker] = sub;
                type = std::max(type, best_type(card, best_type));
                // Break if the the best type has been achieved
                if (type == FIVE_OF_A_KIND)
                    break;
            }
            return type;
        };
        auto typeA = best_type(a, best_type);
        auto typeB = best_type(b, best_type);
        if (typeA < typeB)
            return true;
        if (typeA > typeB)
            return false;
        for (int i = 0; i < 5; ++i)
        {
            auto va = label_values.JOKER[(int)a[i]];
            auto vb = label_values.JOKER[(int)b[i]];
            if (va < vb) return true;
            if (va > vb) return false;
        }
        return false;
    }
};

suite s = [] {
    "2023-07"_test = [] {
        expect(Cards::FIVE_OF_A_KIND == Cards::GetType("AAAAA"));
        expect(Cards::FOUR_OF_A_KIND == Cards::GetType("AA8AA"));
        expect(Cards::FULL_HOUSE == Cards::GetType("23332"));
        expect(Cards::THREE_OF_A_KIND == Cards::GetType("TTT98"));
        expect(Cards::TWO_PAIR == Cards::GetType("23432"));
        expect(Cards::ONE_PAIR == Cards::GetType("A23A4"));
        expect(Cards::HIGH_CARD == Cards::GetType("23456"));

        const char *const TEST1 = R"(32T3K 765
T55J5 684
KK677 28
KTJJT 220
QQQJA 483
)";
        Cards test_cards{std::istringstream{TEST1}};
        expect(6440_i == test_cards.Task1());
        expect(5905_i == test_cards.Task2());

        Cards cards{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", cards.Task1());
        Printer::Print(__FILE__, "2", cards.Task2());
    };
};

} //namespace;
