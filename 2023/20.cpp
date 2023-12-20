#include "../test.hpp"
#include <fstream>

namespace {

using namespace boost::ut;


struct Pulses
{
    Pulses(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
        {
            std::istringstream iss{line};
            std::string def, token;
            iss >> def >> token;

            decltype(modules)::iterator it;
            if (def.starts_with('%'))
                it = modules.insert({def.substr(1), std::make_unique<FlipFlop>()}).first;
            else if (def.starts_with('&'))
                it = modules.insert({def.substr(1), std::make_unique<Conjunction>()}).first;
            else
                it = modules.insert({def, std::make_unique<Module>()}).first;

            while (iss >> token)
            {
                if (token.ends_with(','))
                    token = token.erase(token.size() - 1);
                it->second->outputs.push_back(token);
            }
        }

        for (auto &mod : modules)
            for (const auto &out : mod.second->outputs)
            {
                auto it = modules.find(out);
                if (it == modules.end())
                    it = modules.insert({out, std::make_unique<Module>()}).first;
                ++(it->second)->input_count;
            }
    }

    struct Signal
    {
        std::string from;
        std::string to;
        int value; // -1 for nothing, 0 = low, 1 = high
    };

    std::queue<Signal> signals;
    using PulseCountersT = std::array<size_t, 2>;
    PulseCountersT pulse_counters{};
    std::unordered_map<std::string, PulseCountersT> module_pulse_counters;

    void Enqueue(Signal signal)
    {
        //std::cout << signal.from << " -" << (signal.value ? "high" : "low") << "-> " << signal.to << std::endl;
        signals.push(signal);
    }

    void Process()
    {
        while (!signals.empty())
        {
            Signal signal = std::move(signals.front());
            signals.pop();

            Module *module = modules.at(signal.to).get();
            auto pulse = module->Process(signal);
            if (pulse >= 0)
            {
                Signal output{signal.to, "", pulse};
                pulse_counters[pulse] += module->outputs.size();
                ++module_pulse_counters[signal.to][pulse];
                for (auto out : module->outputs)
                {
                    output.to = out;
                    Enqueue(output);
                }
            }
        }
    }

    struct Module
    {
        int input_count{};
        std::vector<std::string> outputs;

        virtual ~Module() = default;
        virtual int Process(const Signal &signal) { return signal.value; }
    };

    std::unordered_map<std::string, std::unique_ptr<Module>> modules;

    struct FlipFlop : Module
    {
        bool state{false};

        int Process(const Signal &signal) override
        {
            if (signal.value > 0) return -1;
            return state = !state;
        }
    };

    struct Conjunction : Module
    {
        std::unordered_map<std::string, int> memory;

        int Process(const Signal &signal) override
        {
            memory[signal.from] = signal.value;
            if (memory.size() < input_count)
                return 1;
            bool all_high = find_if(memory.begin(), memory.end(), [](const auto &m) { return !m.second; }) == memory.end();
            return !all_high;
        }
    };

    int64_t Task1()
    {
        for (int i = 0; i < 1000; ++i)
        {
            ++pulse_counters[0];
            Enqueue({"button", "broadcaster", false});
            Process();
        }
        return pulse_counters[0] * pulse_counters[1];
    }

    int64_t Task2()
    {
        // (&hn, &mp, &xf, &fz) -> &xn -> rx
        std::vector<std::string> targets{"hn", "mp", "xf", "fz"};
        std::unordered_map<std::string, size_t> periods;
        for (int i = 1000; periods.size() < targets.size(); ++i)
        {
            Enqueue({"button", "broadcaster", false});
            Process();
            for (const auto &m : targets)
            {
                auto &highs = module_pulse_counters.at(m)[1];
                if (highs && !periods.count(m))
                    periods[m] = i + 1;
            }
        }
        size_t res{1};
        for (auto p : periods)
            res = std::lcm(res, p.second);
        return res;
    }
};

suite s = [] {
    "2023-20"_test = [] {
        const char *const TEST1 = R"(broadcaster -> a, b, c
%a -> b
%b -> c
%c -> inv
&inv -> a
)";
        Pulses test1{std::istringstream{TEST1}};
        expect(eq(32000000ull, test1.Task1()));

        const char *const TEST2 = R"(broadcaster -> a
%a -> inv, con
&inv -> b
%b -> con
&con -> output
)";
        Pulses test2{std::istringstream{TEST2}};
        expect(eq(11687500ull, test2.Task1()));

        Pulses task{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", task.Task1());
        Printer::Print(__FILE__, "2", task.Task2());
    };
};

} //namespace;
