#include "../test.hpp"
#include <fstream>
#include <boost/rational.hpp>
#include <boost/multiprecision/gmp.hpp>
#include <boost/process.hpp>

namespace {

using namespace boost::ut;

struct Snowstorm
{
    using I = boost::multiprecision::mpz_int;
    using R = boost::rational<I>;
    using Vec = std::array<I, 3>;
    using VecR = std::array<R, 3>;

    struct Line
    {
        Vec pos;
        Vec slope;
    };
    std::vector<Line> lines;

    Snowstorm(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
        {
            std::istringstream iss{line};
            auto next = [&](auto &&next) -> int64_t {
                std::string token;
                iss >> token;
                if (token == "@")
                    return next(next);
                return std::stoll(token);
            };
            Line h;
            h.pos[0] = next(next);
            h.pos[1] = next(next);
            h.pos[2] = next(next);
            h.slope[0] = next(next);
            h.slope[1] = next(next);
            h.slope[2] = next(next);
            lines.push_back(h);
        }
    }

    static std::vector<VecR> GetIntersection(Line a, Line b, bool ignoreZ)
    {
        // Ta param for a, Tb for b
        // The set of equations:
        //
        // Xa + Ua*Ta = Xb + Ub*Tb
        // Ya + Va*Ta = Yb + Vb*Tb
        // Za + Wa*Ta = Zb + Wb*Tb
        //
        // Solve for Ta, Tb
        auto t = Solve(a.slope[0], -b.slope[0], a.slope[1], -b.slope[1], b.pos[0] - a.pos[0], b.pos[1] - a.pos[1]);
        if (t.empty())
            return {};

        // Ignore the past
        if (t[0] < 0 || t[1] < 0)
            return {};

        if (!ignoreZ && a.pos[2] + a.slope[2] * t[0] != b.pos[2] + b.slope[2] * t[1])
            return {};

        VecR intersection;
        for (int i = 0; i < 3; ++i)
            intersection[i] = a.pos[i] + a.slope[i] * t[0];
        return {intersection};
    }

    static I CalcDeterminant(I a, I b, I c, I d)
    {
        return a * d - b * c;
    }

    static std::vector<R> Solve(I a, I b, I c, I d, I e, I f)
    {
        auto det = CalcDeterminant(a, b, c, d);
        if (det == 0)
            return {};
        std::vector<R> res;
        // Workaround boost::rational bug when denominator is negative
        if (det < 0)
        {
            res.push_back(R{-CalcDeterminant(e, b, f, d), -det});
            res.push_back(R{-CalcDeterminant(a, e, c, f), -det});
        }
        else
        {
            res.push_back(R{CalcDeterminant(e, b, f, d), det});
            res.push_back(R{CalcDeterminant(a, e, c, f), det});
        }
        return res;
    }

    int Task1(I min, I max)
    {
        int count{};

        for (int i = 1; i < lines.size(); ++i)
        {
            for (int j = 0; j < i; ++j)
            {
                auto c = GetIntersection(lines[i], lines[j], true);
                if (c.empty())
                    continue;
                auto x = c[0][0];
                auto y = c[0][1];
                if (x >= min && x <= max && y >= min && y <= max)
                    ++count;
            }
        }
        return count;
    }

    std::string Task2()
    {
        // Use maxima to solve (life is too short to code in C++)
        // Unknowns: x, y, z, vx, vy, vz
        // One hailstone adds an unknown time of collision, but add three equations for x, y, z
        // Three hailstones result in 9 equations with 9 unknowns.
        // eqns : [
        //     19 - 2 * t1 = x + vx * t1,
        //     13 + 1 * t1 = y + vy * t1,
        //     30 - 2 * t1 = z + vz * t1,
        // 
        //     18 - 1 * t2 = x + vx * t2,
        //     19 - 1 * t2 = y + vy * t2,
        //     22 - 2 * t2 = z + vz * t2,
        // 
        //     20 - 2 * t3 = x + vx * t3,
        //     25 - 2 * t3 = y + vy * t3,
        //     34 - 4 * t3 = z + vz * t3
        // ];
        // 
        // solve(eqns, [x, y, z, vx, vy, vz, t1, t2, t3]);
        std::ostringstream oss;
        oss << "eqns: [\n";
        const char *delim = "";
        for (int i = 0; i < 3; ++i)
        {
            const auto &h = lines[i];
            for (int j = 0; j < 3; ++j)
            {
                oss << delim;
                delim = ",\n";
                oss << "    " << h.pos[j] << " + " << "(" << h.slope[j] << ") * t" << i << " = x" << j << " + v" << j << " * t" << i;
            }
        }
        oss << "\n];\n";
        oss << "roots: solve(eqns, [x0, x1, x2, v0, v1, v2, t0, t1, t2]);\n";
        oss << "res: ev(x0 + x1 + x2, roots);\n";
        //std::cerr << oss.str() << std::endl;

        const char *const fname = "/tmp/hailstorm.maxima";
        {
            std::ofstream ofs(fname);
            ofs << oss.str();
        }

        namespace bp = boost::process;

        std::string command = "maxima --very-quiet --batch=";
        command += fname;

        bp::ipstream pipe_stream;
        bp::child process(command, bp::std_out > pipe_stream);

        // Get the last line of the output
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(pipe_stream, line))
            lines.push_back(line);
        process.wait();

        // The penultimate line
        std::string res = lines[lines.size() - 2];
        auto idx = res.find_first_not_of(' ');
        res = res.erase(0, idx);
        return res;
    }
};

suite s = [] {
    "2023-24"_test = [] {
        const char *const TEST1 = R"(19, 13, 30 @ -2,  1, -2
18, 19, 22 @ -1, -1, -2
20, 25, 34 @ -2, -2, -4
12, 31, 28 @ -1, -2, -1
20, 19, 15 @  1, -5, -3
)";
        Snowstorm test1{std::istringstream{TEST1}};
        expect(2_i == test1.Task1(7, 27));
        expect("47" == test1.Task2());

        Snowstorm task{std::ifstream{INPUT}};
        Printer::Print(__FILE__, "1", task.Task1(200000000000000ll, 400000000000000ll));
        Printer::Print(__FILE__, "2", task.Task2());
    };
};

} //namespace;
