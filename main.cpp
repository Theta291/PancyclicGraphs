#include <iostream>
#include <set>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <boost/pending/disjoint_sets.hpp>
#include "pancyclic.h"

using namespace std;


int main(int argc, char** argv)
{
    int verts = 18;

    unordered_set<Chord> test_chords;
    test_chords.insert(Chord(0, 16, verts));
    test_chords.insert(Chord(1, 15, verts));
    test_chords.insert(Chord(2, 4, verts));
    test_chords.insert(Chord(3, 5, verts));
    test_chords.insert(Chord(6, 8, verts));
    test_chords.insert(Chord(7, 9, verts));
    test_chords.insert(Chord(10, 17, verts));
    test_chords.insert(Chord(11, 13, verts));
    test_chords.insert(Chord(12, 14, verts));
    Hamiltonian test_graph = Hamiltonian::from_iter(verts, test_chords.begin(), test_chords.end());

    for (Chord chord : test_graph.chords()) cout << chord << endl;

    int compn = 0;
    for (auto comp : test_graph.get_crossing_components())
    {
        cout << "Comp num: " << ++compn << endl;
        for (Chord chord : comp.chords()) cout << '\t' << chord << endl;
    }

    cout << endl << "Test graph: " << endl;
    cout << test_graph.describe() << endl;

    auto iso_num = test_graph.get_graph_iso_num();
    Hamiltonian iso_graph(verts, iso_num);

    cout << endl << "Iso graph:" << endl;
    cout << iso_graph.describe() << endl;

    /*
    Span s1(0, 9, 18);
    Span s2(4, 1, 18);
    Span s3(13, 10, 18);

    cout << s1.coincident(s2) << endl;
    cout << s2.coincident(s1) << endl;
    cout << s1.coincident(s3) << endl;
    cout << s3.coincident(s1) << endl;
    cout << s2.coincident(s3) << endl;
    cout << s3.coincident(s2) << endl;
    */
    return 0;
}
