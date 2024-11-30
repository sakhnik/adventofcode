#include "../test.hpp"
#include <functional>
#include <fstream>
#include <cassert>
#include <map>
#include <set>
#include <list>

namespace {

struct Rotation
{
    int sign{};  // bitmask, 3 bits
    std::array<int, 3> idx = {0, 1, 2};

    Rotation() = default;

    bool operator<(const Rotation &o) const
    {
        if (sign < o.sign) return true;
        if (sign > o.sign) return false;
        return idx < o.idx;
    }

    static bool ForEach(std::function<bool(const Rotation &)> pred)
    {
        Rotation r;
        do
        {
            for (r.sign = 0; r.sign < 8; ++r.sign)
            {
                if (pred(r))
                    return true;
            }
        }
        while (std::next_permutation(std::begin(r.idx), std::end(r.idx)));
        return false;
    }
};


class Vector
{
public:
    int x[3] = {0, 0, 0};

    bool operator==(const Vector &o) const
    {
        return x[0] == o.x[0] && x[1] == o.x[1] && x[2] == o.x[2];
    }

    bool operator<(const Vector &o) const
    {
        if (x[0] < o.x[0]) return true;
        if (x[0] > o.x[0]) return false;
        if (x[1] < o.x[1]) return true;
        if (x[1] > o.x[1]) return false;
        return x[2] < o.x[2];
    }

    size_t Len() const
    {
        return std::abs(x[0]) + std::abs(x[1]) + std::abs(x[2]);
    }

    Vector Signature() const
    {
        Vector n{std::abs(x[0]), std::abs(x[1]), std::abs(x[2])};
        std::sort(n.x, n.x + 3);
        return n;
    }

    Vector operator-() const
    {
        return {-x[0], -x[1], -x[2]};
    }

    Vector operator-(const Vector &o) const
    {
        return {x[0] - o.x[0], x[1] - o.x[1], x[2] - o.x[2]};
    }

    Vector operator+(const Vector &o) const
    {
        return {x[0] + o.x[0], x[1] + o.x[1], x[2] + o.x[2]};
    }

    Vector Rotate(const Rotation &r) const
    {
        auto tr = [this, &r](int i) {
            return (-2 * ((r.sign >> i) & 1) + 1) * x[r.idx[i]];
        };

        return {tr(0), tr(1), tr(2)};
    }
};

//inline std::ostream& operator<<(std::ostream &os, const Vector &v)
//{
//    return os << v.x[0] << "," << v.x[1] << "," << v.x[2];
//}

struct Transformation
{
    Vector trans{};
    Rotation rot{};

    bool operator<(const Transformation &o) const
    {
        if (trans < o.trans) return true;
        if (trans == o.trans)
            return rot < o.rot;
        return false;
    }

    Vector operator()(const Vector &v) const
    {
        return v.Rotate(rot) - trans;
    }
};

class Scanner
{
public:
    int num{};
    bool resolved = false;

    using BeaconsT = std::vector<Vector>;
    BeaconsT beacons;

    Scanner(int num, std::istream &is)
        : num{num}
    {
        std::string line;
        while (getline(is, line))
        {
            if (line.empty())
                break;
            Vector v{};
            if (3 == sscanf(line.c_str(), "%d,%d,%d", &v.x[0], &v.x[1], &v.x[2]))
                beacons.push_back(v);
        }

        CalcSignatures();
    }

    void MarkResolved() { resolved = true; }

    using BeaconPairT = std::pair<size_t, size_t>;
    using SignaturesT = std::vector<std::pair<Vector, BeaconPairT>>;

    SignaturesT signatures;

    void CalcSignatures()
    {
        for (size_t i = 0; i < beacons.size() - 1; ++i)
        {
            for (size_t j = i + 1; j < beacons.size(); ++j)
            {
                auto s = (beacons[i] - beacons[j]).Signature();
                signatures.emplace_back(s, BeaconPairT{i, j});
            }
        }
        std::sort(signatures.begin(), signatures.end(), [](const auto &a, const auto &b) { return a.first < b.first; });
    }

    std::vector<Vector> GetCommonSignatures(const Scanner &o) const
    {
        const SignaturesT &a = signatures;
        const SignaturesT &b = o.signatures;
        std::vector<Vector> common;
        for (size_t i{0}, j{0}; i < a.size() && j < b.size(); )
        {
            if (a[i].first < b[j].first)
                ++i;
            else if (a[i].first == b[j].first)
            {
                if (common.empty() || common.back() != a[i].first)
                    common.push_back(a[i].first);
                ++i;
                ++j;
            }
            else
                ++j;
        }
        return common;
    }

    Transformation transformation;

    bool TryResolve(size_t overlap_count, Scanner &other_scanner) const
    {
        assert(resolved);

        auto common_signatures = GetCommonSignatures(other_scanner);
        if (common_signatures.size() < overlap_count * (overlap_count - 1) / 2)
            return false;

        auto iterateMatches = [&](std::function<bool(Vector, Vector, Vector, Vector)> func) {
            auto iti = signatures.begin();
            auto itk = other_scanner.signatures.begin();

            for (auto d : common_signatures)
            {
                // Assume the signatures will be pairwise unique (actually checked)
                while (iti->first != d) ++iti;
                while (itk->first != d) ++itk;

                auto [srci, dsti] = iti->second;
                auto [srck, dstk] = itk->second;

                const auto &beacons2 = other_scanner.beacons;
                if (func(beacons[srci], beacons[dsti], beacons2[srck], beacons2[dstk]))
                    return true;
            }
            return false;
        };

        Transformation transformation{};

        bool found = Rotation::ForEach([&](const Rotation &r) {

            // Translation vector to set of overlapping points
            std::map<Vector, std::set<Vector>> overlaps;
            Vector last_translation{};

            bool found = iterateMatches([&](Vector p1, Vector p2, Vector q1, Vector q2) {
                q1 = q1.Rotate(r);
                q2 = q2.Rotate(r);
                last_translation = q1 - p1;
                if (last_translation == Vector{} || last_translation != q2 - p2)
                    return false;
                auto &o = overlaps[last_translation];
                o.insert(p1);
                o.insert(p2);
                if (o.size() >= overlap_count - 1)
                    return true;
                return false;
            });
            if (found)
            {
                transformation.rot = r;
                transformation.trans = last_translation;
                return true;
            }

            return false;
        });

        if (!found)
            return false;

        other_scanner.transformation = transformation;
        other_scanner.resolved = true;

        // Translate beacon coordinates to the common origin
        for (auto &b : other_scanner.beacons)
            b = transformation(b);

        return true;
    }
};

class Scanners
{
public:
    using ScannersT = std::vector<Scanner>;
    ScannersT scanners;

    Scanners(std::istream &&is)
    {
        std::string line;
        while (getline(is, line))
        {
            int num{};
            if (1 == sscanf(line.c_str(), "--- scanner %d", &num))
            {
                scanners.emplace_back(num, is);
                continue;
            }
        }
    }

    size_t CountUniqueBeacons(size_t overlap_count)
    {
        auto &scanner0 = scanners.front();
        std::set<Vector> unique_beacons(scanner0.beacons.begin(), scanner0.beacons.end());
        scanner0.MarkResolved();

        std::list<Scanner*> resolved;
        resolved.push_back(&scanner0);

        std::list<Scanner*> todo;
        for (size_t i = 1; i < scanners.size(); ++i)
            todo.push_back(&scanners[i]);

        for (auto i = resolved.begin(); i != resolved.end(); ++i)
        {
            Scanner *scanner1 = *i;
            for (auto j = todo.begin(); j != todo.end(); )
            {
                Scanner *scanner2 = *j;
                if (scanner1->TryResolve(overlap_count, *scanner2))
                {
                    for (const auto &b : scanner2->beacons)
                        unique_beacons.insert(b);
                    j = todo.erase(j);
                    resolved.push_back(scanner2);
                }
                else
                    ++j;
           }
        }

        return unique_beacons.size();
    }

    size_t GetMaxDistance() const
    {
        size_t max_d{};
        for (size_t i = 0; i < scanners.size() - 1; ++i)
        {
            for (size_t j = i + 1; j < scanners.size(); ++j)
            {
                size_t d = (scanners[i].transformation.trans - scanners[j].transformation.trans).Len();
                if (d > max_d)
                    max_d = d;
            }
        }
        return max_d;
    }
};

const char *TEST_INPUT = R"(
--- scanner 0 ---
404,-588,-901
528,-643,409
-838,591,734
390,-675,-793
-537,-823,-458
-485,-357,347
-345,-311,381
-661,-816,-575
-876,649,763
-618,-824,-621
553,345,-567
474,580,667
-447,-329,318
-584,868,-557
544,-627,-890
564,392,-477
455,729,728
-892,524,684
-689,845,-530
423,-701,434
7,-33,-71
630,319,-379
443,580,662
-789,900,-551
459,-707,401

--- scanner 1 ---
686,422,578
605,423,415
515,917,-361
-336,658,858
95,138,22
-476,619,847
-340,-569,-846
567,-361,727
-460,603,-452
669,-402,600
729,430,532
-500,-761,534
-322,571,750
-466,-666,-811
-429,-592,574
-355,545,-477
703,-491,-529
-328,-685,520
413,935,-424
-391,539,-444
586,-435,557
-364,-763,-893
807,-499,-711
755,-354,-619
553,889,-390

--- scanner 2 ---
649,640,665
682,-795,504
-784,533,-524
-644,584,-595
-588,-843,648
-30,6,44
-674,560,763
500,723,-460
609,671,-379
-555,-800,653
-675,-892,-343
697,-426,-610
578,704,681
493,664,-388
-671,-858,530
-667,343,800
571,-461,-707
-138,-166,112
-889,563,-600
646,-828,498
640,759,510
-630,509,768
-681,-892,-333
673,-379,-804
-742,-814,-386
577,-820,562

--- scanner 3 ---
-589,542,597
605,-692,669
-500,565,-823
-660,373,557
-458,-679,-417
-488,449,543
-626,468,-788
338,-750,-386
528,-832,-391
562,-778,733
-938,-730,414
543,643,-506
-524,371,-870
407,773,750
-104,29,83
378,-903,-323
-778,-728,485
426,699,580
-438,-605,-362
-469,-447,-387
509,732,623
647,635,-688
-868,-804,481
614,-800,639
595,780,-596

--- scanner 4 ---
727,592,562
-293,-554,779
441,611,-461
-714,465,-776
-743,427,-804
-660,-479,-426
832,-632,460
927,-485,-438
408,393,-506
466,436,-512
110,16,151
-258,-428,682
-393,719,612
-211,-452,876
808,-476,-593
-575,615,604
-485,667,467
-680,325,-822
-627,-443,-432
872,-547,-609
833,512,582
807,604,487
839,-516,451
891,-625,532
-652,-548,-490
30,-46,-14
)";

using namespace boost::ut;
using namespace std::string_literals;

suite s = [] {
    "19"_test = [] {
        Scanners test{std::istringstream{TEST_INPUT}};
        expect(79_u == test.CountUniqueBeacons(12));
        expect(3621_u == test.GetMaxDistance());

        Scanners scanners{std::fstream{INPUT}};
        Printer::Print(__FILE__, "1", scanners.CountUniqueBeacons(12));
        Printer::Print(__FILE__, "2", scanners.GetMaxDistance());
    };
};

} //namespace;
