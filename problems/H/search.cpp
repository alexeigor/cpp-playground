#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <optional>
#include <limits>
#include <cassert>
#include <cstdlib>
#include <ctime>

static constexpr uint32_t HALF_MAX = std::numeric_limits<uint32_t>::max() / 2;
static constexpr uint32_t MAX_VAL  = std::numeric_limits<uint32_t>::max();

class FuzzySearch
{
public:
    explicit FuzzySearch(const std::vector<uint32_t>& input)
    {
        data_ = input;
        std::sort(data_.begin(), data_.end());
    }

    std::optional<uint32_t> Find(uint32_t x) const
    {
        const uint32_t lower = (x >> 1) + (x & 1); // ceiling of x / 2 for any x, avoiding potential overflow from doing x + 1
        const uint32_t upper = (x > HALF_MAX) ? MAX_VAL : (x << 1); // probably this can be done without branches

        auto it = std::lower_bound(data_.begin(), data_.end(), lower);
        if (it != data_.end() && *it <= upper) {
            return *it;
        }
        return std::nullopt;
    }

private:
    std::vector<uint32_t> data_;
};

int main()
{
    std::vector<uint32_t> arr = {0, 1, 2147483647, 2147483648, 4294967295};
    FuzzySearch searcher(arr);
    std::vector<uint32_t> q = {0, 1, 2147483647, 2147483648, 4294967295};

    for (uint32_t query : q)
    {
        auto result = searcher.Find(query);
        if (result.has_value()) {
            std::cout << "Query = " << query 
                      << ", Found = " << result.value() << "\n";
        } else {
            std::cout << "Query = " << query 
                      << ", No element found.\n";
        }
    }

    return 0;
}

// ----------------------------------------------------
// Simple "check" helper for pass/fail messages
// ----------------------------------------------------
static void check(bool condition, const std::string& testName)
{
    if (!condition)
        std::cerr << "[FAILED] " << testName << "\n";
    else
        std::cout << "[PASSED] " << testName << "\n";
}

// ----------------------------------------------------
// Test 1: Basic usage
// ----------------------------------------------------
void testBasic()
{
    std::vector<uint32_t> inputs = {10, 1, 100, 2, 0, 16};
    FuzzySearch searcher(inputs);

    // We'll mimic the example main() queries:
    struct {
        uint32_t query;
        bool expectFound;
    } testData[] = {
        {0,  true},
        {1,  true},
        {2,  true},
        {3,  true},
        {8,  true},
        {10, true},
        {50, true},
        {100,true},
        {160,true},
        {999999999, false}
    };

    bool allPassed = true;
    for (auto& td : testData) {
        auto result = searcher.Find(td.query);
        bool gotValue = result.has_value();
        if (gotValue != td.expectFound) {
            allPassed = false;
            break;
        }
    }

    check(allPassed, "testBasic");
}

// ----------------------------------------------------
// Test 2: Single-element array
// ----------------------------------------------------
void testSingleElementArray()
{
    // Only one element: 50
    std::vector<uint32_t> inputs = {50};
    FuzzySearch searcher(inputs);

    // Query exactly 50 => should find 50
    auto r1 = searcher.Find(50);
    bool pass1 = (r1.has_value() && r1.value() == 50u);

    // Query smaller than half => (x+1)/2 won't include 50 => no match
    auto r2 = searcher.Find(24); 
    bool pass2 = (!r2.has_value());

    // Query 100 => range is 50..200 => should find 50
    auto r3 = searcher.Find(100);
    bool pass3 = (r3.has_value() && r3.value() == 50u);

    // Query 101 => range is 51..202 => 50 is out of range => no match
    auto r4 = searcher.Find(101);
    bool pass4 = (!r4.has_value());

    bool allPassed = pass1 && pass2 && pass3 && pass4;
    std::cout << pass1 << pass2 << pass3 << pass4 << std::endl;
    check(allPassed, "testSingleElementArray");
}

// ----------------------------------------------------
// Test 3: All zeros
// ----------------------------------------------------
void testAllZeros()
{
    // All elements are zero
    std::vector<uint32_t> inputs = {0, 0, 0, 0};
    FuzzySearch searcher(inputs);

    // For x=0 => (0+1)/2=0..0 => should find zero
    auto r0 = searcher.Find(0);
    bool pass0 = (r0.has_value() && r0.value() == 0u);

    // For x=1 => (1+1)/2=1..2 => lower_bound(1) => all zeros < 1 => no match
    bool pass1 = (!searcher.Find(1).has_value());

    // For x=2 => range is 1..4 => same reasoning => no match
    bool pass2 = (!searcher.Find(2).has_value());

    // For x=10 => range is 5..20 => no match
    bool pass3 = (!searcher.Find(10).has_value());

    bool allPassed = pass0 && pass1 && pass2 && pass3;
    check(allPassed, "testAllZeros");
}

// ----------------------------------------------------
// Test 4: Large values & boundary checks
// ----------------------------------------------------
void testLargeValues()
{
    uint32_t nearMax = std::numeric_limits<uint32_t>::max(); // 4294967295
    uint32_t halfMax = nearMax / 2;                          // ~2147483647

    // We'll store nearMax, halfMax, 0, 1
    std::vector<uint32_t> inputs = {nearMax, halfMax, 0, 1};
    FuzzySearch searcher(inputs);

    // 1) For x=nearMax => range is approx halfMax..nearMax.
    //    lower_bound(halfMax) => points to halfMax or nearMax.
    //    Because it's sorted, the first element >= halfMax is halfMax, 
    //    which is <= nearMax, so we expect halfMax.
    auto r1 = searcher.Find(nearMax);
    std::cout << r1.value() << std::endl;
    bool pass1 = (r1.has_value() && r1.value() == nearMax);

    // 2) For x=(halfMax - 10), let's see if we find halfMax or not.
    //    range is ~((halfMax-10)+1)/2..(halfMax-10)*2 => 
    //    That should include halfMax, so we expect that result.
    uint32_t smallQuery = halfMax - 10;
    auto r2 = searcher.Find(smallQuery);
    bool pass2 = (r2.has_value() && r2.value() == halfMax);

    // 3) For x=0 => range is 0..0 => we expect 0
    auto r3 = searcher.Find(0);
    bool pass3 = (r3.has_value() && r3.value() == 0u);

    // 4) For x=1 => range is 1..2 => we expect to find 1
    auto r4 = searcher.Find(1);
    bool pass4 = (r4.has_value() && r4.value() == 1u);

    bool allPassed = pass1 && pass2 && pass3 && pass4;
    std::cout << pass1 << pass2 << pass3 << pass4 << std::endl;
    check(allPassed, "testLargeValues");
}

// ----------------------------------------------------
// Main: run all tests
// ----------------------------------------------------
int main1()
{
    testBasic();
    testSingleElementArray();
    testAllZeros();
    testLargeValues();
    return 0;
}