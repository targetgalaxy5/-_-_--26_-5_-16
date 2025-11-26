
#include <iostream>
#include <future>
#include <chrono>
#include <syncstream>
using namespace std;

void compute(const string& name, bool slow) {
    int t = slow ? 7 : 1;
    this_thread::sleep_for(chrono::seconds(t));
    osyncstream(cout) << name << "\n";
}

void work() {
    auto start = chrono::steady_clock::now();

    // A (slow)
    auto fA = async(launch::async, [](){ compute("A", true); });

    // B (quick)
    auto fB = async(launch::async, [](){ compute("B", false); });

    // C (slow)
    auto fC = async(launch::async, [](){ compute("C", true); });

    // D2 depends only on nothing (quick)
    auto fD2 = async(launch::async, [](){ compute("D2", false); });

    // D1 depends on A, B, C
    auto fD1 = async(launch::async, [&](){
        fA.get();
        fB.get();
        fC.get();
        compute("D1", false);
    });

    // F1 depends on D1 and D2
    auto fF1 = async(launch::async, [&](){
        fD1.get();
        fD2.get();
        compute("F1", false);
    });

    // F2 depends on D2
    auto fF2 = async(launch::async, [&](){
        fD2.get();
        compute("F2", false);
    });

    fF1.get();
    fF2.get();

    auto end = chrono::steady_clock::now();
    double time = chrono::duration<double>(end - start).count();

    osyncstream(cout) << "Work is done! Time: " << time << " sec\n";
}

int main() {
    work();
}
