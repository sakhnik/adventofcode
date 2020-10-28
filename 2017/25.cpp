#include <unordered_set>
#include <numeric>
#include "../test.hpp"

namespace {

constexpr unsigned StateIdx(char c) { return c - 'A'; }

struct Action
{
    bool out;       // What to write
    int direction;  // Where to move ±1
    unsigned next;  // What's the next state index
};

struct State
{
    Action action[2];  // Action on 0 or 1
};

struct Machine
{
    void Run(unsigned iterations, unsigned state, State desc[])
    {
        _state = state;
        _position = 0;
        while (iterations--)
        {
            auto &st = desc[_state];
            auto it = _tape.find(_position);
            bool is_one = it != _tape.end();
            auto &action = st.action[is_one];
            if (action.out)
            {
                if (!is_one)
                    _tape.insert(_position);
            }
            else
            {
                if (is_one)
                    _tape.erase(it);
            }
            _state = action.next;
            _position += action.direction;
        }
    }

    unsigned GetChecksum() const
    {
        return _tape.size();
    }

private:
    unsigned _state;
    int _position;
    std::unordered_set<int> _tape;   // absent -- 0, present -- 1 at the position
};

using namespace boost::ut;

suite s = [] {
    "2017-25"_test = [] {
        {
            State desc[] = {
                {{{true, 1, StateIdx('B')}, {false, -1, StateIdx('B')}}},
                {{{true, -1, StateIdx('A')}, {true, 1, StateIdx('A')}}},
            };

            Machine m;
            m.Run(6, 0, desc);
            expect(m.GetChecksum() == 3_u);
        }

        State desc[] = {

            //In state A:
            {{
                //If the current value is 0:
                {true,           //- Write the value 1.
                    1,              //- Move one slot to the right.
                    StateIdx('B')}, //- Continue with state B.
                                    //If the current value is 1:
                {false,          //- Write the value 0.
                    -1,             //- Move one slot to the left.
                    StateIdx('C')}  //- Continue with state C.
            }},

            //In state B:
            {{
                //If the current value is 0:
                {true,           //- Write the value 1.
                    -1,             //- Move one slot to the left.
                    StateIdx('A')}, //- Continue with state A.
                                    //If the current value is 1:
                {true,           //- Write the value 1.
                    -1,             //- Move one slot to the left.
                    StateIdx('D')}  //- Continue with state D.
            }},

            //In state C:
            {{
                //If the current value is 0:
                {true,           //- Write the value 1.
                    1,              //- Move one slot to the right.
                    StateIdx('D')}, //- Continue with state D.
                                    //If the current value is 1:
                {false,          //- Write the value 0.
                    1,              //- Move one slot to the right.
                    StateIdx('C')}  //- Continue with state C.
            }},

            //In state D:
            {{
                //If the current value is 0:
                {false,          //- Write the value 0.
                    -1,             //- Move one slot to the left.
                    StateIdx('B')}, //- Continue with state B.
                                    //If the current value is 1:
                {false,          //- Write the value 0.
                    1,              //- Move one slot to the right.
                    StateIdx('E')}  //- Continue with state E.
            }},

            //In state E:
            {{
                //If the current value is 0:
                {true,           //- Write the value 1.
                    1,              //- Move one slot to the right.
                    StateIdx('C')}, //- Continue with state C.
                                    //If the current value is 1:
                {true,           //- Write the value 1.
                    -1,             //- Move one slot to the left.
                    StateIdx('F')}  //- Continue with state F.
            }},

            //In state F:
            {{
                //If the current value is 0:
                {true,           //- Write the value 1.
                    -1,             //- Move one slot to the left.
                    StateIdx('E')}, //- Continue with state E.
                                    //If the current value is 1:
                {true,           //- Write the value 1.
                    1,              //- Move one slot to the right.
                    StateIdx('A')}  //- Continue with state A.
            }},
        };

        Machine m;
        m.Run(12656374, 'A' - 'A', desc);
        Printer::Print(__FILE__, nullptr, m.GetChecksum());
    };
};

} //namespace;