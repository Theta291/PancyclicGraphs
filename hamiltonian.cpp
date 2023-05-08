#include <vector>
#include <string>
#include <iterator>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <boost/pending/disjoint_sets.hpp>
#include "pancyclic.h"

Hamiltonian::Hamiltonian() : num_vertices(0) {}

Hamiltonian::Hamiltonian(int nvert) : num_vertices(nvert) {}


// Unordered multiset would be more appropriate, but it would be a nightmare to implement the hash function
std::vector<Hamiltonian> Hamiltonian::get_crossing_components() const
{
    // TODO: Look into this demo from boost:
    // https://www.boost.org/doc/libs/1_33_1/libs/graph/doc/incremental_components.html

    // It's hard to make a disjoint set of chords, so I just map the chords to indices
    std::vector<Chord const*> chord_vec(chords.size());
    std::unordered_map<Chord const*, int> chord_to_idx;
    int idx = 0;

    for (auto const& chord : chords)
    {
        chord_to_idx[&chord] = idx;
        chord_vec[idx++] = &chord;
    }

    // For some reason, I have to do this to make the disjoint set use arrays to store ranks and parents
    std::vector<int> rank(chords.size());
    std::vector<int> parent(chords.size());
    boost::disjoint_sets<int*, int*> comps(&rank[0], &parent[0]);

    // Add all chords to disjoint sets
    for (auto const& chord : chord_vec) comps.make_set(chord_to_idx[chord]);

    // For each pair of crossing chords, combine the sets
    for (auto it1 = chord_vec.begin(); it1 != std::prev(chord_vec.end()); ++it1)
    {
        for (auto it2 = std::next(it1); it2 != chord_vec.end(); ++it2)
        {
            if ((*it1)->crossing(**it2)) comps.union_set(chord_to_idx[*it2], chord_to_idx[*it1]);
        }
    }

    // Make map of Hamiltonians
    std::unordered_map<int, Hamiltonian> comps_map;
    for (auto const& chord : chord_vec) comps_map[comps.find_set(chord_to_idx[chord])].chords.insert(*chord);

    // Make and return a vector of Hamiltonians, one for each chord component
    std::vector<Hamiltonian> ret_vec;
    for (auto& num_graph : comps_map)
    {
        num_graph.second.num_vertices = num_vertices;
        ret_vec.push_back(num_graph.second);
    }
    return ret_vec;
}

std::unordered_map<Chord, Hamiltonian*> Hamiltonian::get_crossing_components_map() const
{
    // TODO: Look into this demo from boost:
    // https://www.boost.org/doc/libs/1_33_1/libs/graph/doc/incremental_components.html

    // It's hard to make a disjoint set of chords, so I just map the chords to indices
    std::vector<Chord const*> chord_vec(chords.size());
    std::unordered_map<Chord const*, int> chord_to_idx;
    int idx = 0;

    for (auto const& chord : chords)
    {
        chord_to_idx[&chord] = idx;
        chord_vec[idx++] = &chord;
    }

    // For some reason, I have to do this to make the disjoint set use arrays to store ranks and parents
    std::vector<int> rank(chords.size());
    std::vector<int> parent(chords.size());
    boost::disjoint_sets<int*, int*> comps(&rank[0], &parent[0]);

    // Add all chords to disjoint sets
    for (auto const& chord : chord_vec) comps.make_set(chord_to_idx[chord]);

    // For each pair of crossing chords, combine the sets
    for (auto it1 = chord_vec.begin(); it1 != std::prev(chord_vec.end()); ++it1)
    {
        for (auto it2 = std::next(it1); it2 != chord_vec.end(); ++it2)
        {
            if ((*it1)->crossing(**it2)) comps.union_set(chord_to_idx[*it2], chord_to_idx[*it1]);
        }
    }

    // Make two maps: one from component number to hamiltonian pointer to help keep track
    // Another from chord to hamiltonian pointer
    std::unordered_map<int, Hamiltonian*> comp_to_hamil;
    std::unordered_map<Chord, Hamiltonian*> chord_to_hamil;
    for (auto const& chord : chord_vec) {
        int comp = comps.find_set(chord_to_idx[chord]);
        if (comp_to_hamil.find(comp) == comp_to_hamil.end()) comp_to_hamil[comp] = new Hamiltonian(num_vertices);
        comp_to_hamil[comp]->chords.insert(*chord);
        chord_to_hamil[*chord] = comp_to_hamil[comp];
    }

    return chord_to_hamil;
}

std::vector<short> Hamiltonian::get_graph_num() const
{
    // Not very fast for sparse graphs
    // For sparse graphs, consider calculating the bit position instead of looping through all possible edges
    // Would it be faster to make an adjacency list?

    // Formula for max number of chords: n C 2 - n (because n edges are assumed)
    const unsigned int num_bits = 8 * sizeof(short);
    auto const max_num_chords = Chord::max_num_chords(num_vertices);
    const unsigned int rem = max_num_chords % sizeof(short);

    // Round size up to align with short
    std::vector<short> ret(max_num_chords / num_bits + (rem ? num_bits - rem : 0));
    unsigned long int idx = 0;
    Chord iter_chord(0, 1, num_vertices);
    for (int start = 0; start < num_vertices - 2; start++)
    {
        iter_chord.start = start;
        // A chord starting at idx 0 cannot end at num_vertices-1, so I have to adjust
        // Hence the - (start ? 0 : 1)
        for (int end = start+2; end < num_vertices - (start ? 0 : 1); end++)
        {
            iter_chord.end = end;
            unsigned long int div = idx / num_bits;
            ret[div] |= chords.count(iter_chord) << (idx++ % num_bits);
        }
    }
    return ret;
}

Hamiltonian::Hamiltonian(int nvert, std::vector<short> const& graph_num)
{
    num_vertices = nvert;
    const unsigned int num_bits = 8 * sizeof(short);
    auto const max_num_chords = Chord::max_num_chords(num_vertices);
    const unsigned int rem = max_num_chords % sizeof(short);

    // Round size up to align with short
    unsigned long int idx = 0;
    Chord iter_chord(0, 1, num_vertices);
    for (int start = 0; start < num_vertices - 2; start++)
    {
        iter_chord.start = start;
        // A chord starting at idx 0 cannot end at num_vertices-1, so I have to adjust
        // Hence the - (start ? 0 : 1)
        for (int end = start+2; end < num_vertices - (start ? 0 : 1); end++)
        {
            iter_chord.end = end;
            unsigned long int div = idx / num_bits;
            if ((graph_num[div] >> (idx++ % num_bits)) & 1) chords.insert(iter_chord);
        }
    }
}

std::vector<short> Hamiltonian::get_graph_iso_num() const
{
    Hamiltonian copy = *this;
    std::vector<short> min = get_graph_num();
    for (int i = 0; i < num_vertices-1; i++)
    {
        copy.rotate();
        std::vector<short> new_num = copy.get_graph_num();
        if (new_num < min) min = new_num;
    }
    copy.reflect();
    std::vector<short> new_num = copy.get_graph_num();
    if (new_num < min) min = new_num;
    for (int i = 0; i < num_vertices-1; i++)
    {
        copy.rotate();
        std::vector<short> new_num = copy.get_graph_num();
        if (new_num < min) min = new_num;
    }

    return min;
}

std::string Hamiltonian::get_graph_num_digs(int nverts, std::vector<short> graph_num)
{
    std::string ret_str;
    const unsigned int num_bits = 8 * sizeof(short);
    auto max_num_chords = Chord::max_num_chords(nverts);

    for (long unsigned int i = 0; i < max_num_chords; i++)
    {
        long unsigned int div = i / num_bits;
        ret_str += std::to_string((graph_num[div] >> (i % num_bits)) & 1);
    }
    return ret_str;
}


void Hamiltonian::chord_based_transform(std::function<Chord(Chord)> transform) 
{
    // TODO: Make more efficient? Each chord is copied four times.
    std::unordered_set<Chord> new_set;
    for (auto& chord : chords)
    {
        // First and second copy
        Chord new_chord = transform(chord);
        // Third copy
        new_set.insert(new_chord);
    }
    // Fourth copy
    chords = new_set;
}

void Hamiltonian::rotate(int rotation)
{
    auto rotator = [rotation](Chord c)
    {
        Chord copy = c;
        copy.rotate(rotation);
        return copy;
    };

    chord_based_transform(rotator);
}

void Hamiltonian::rotate() {rotate(1);}

void Hamiltonian::reflect(int vertex)
{
    auto reflector = [vertex](Chord c)
    {
        Chord copy = c;
        copy.reflect(vertex);
        return copy;
    };

    chord_based_transform(reflector);
}

void Hamiltonian::reflect() {reflect(0);}


std::string Hamiltonian::describe(bool w_graph_num, bool w_graph_iso_num) const
{
    std::stringstream out;
    out << "Number of vertices: " << num_vertices << std::endl;
    out << "Number of chords: " << chords.size() << std::endl;

    out << "Chords:" << std::endl;
    for (auto& chord : chords) out << '\t' << chord << std::endl;

    if (w_graph_num)
    {
        out << "Graph number: "<< std::endl;
        out << get_graph_num_digs(num_vertices, get_graph_num()) << std::endl;
    }
    if (w_graph_iso_num)
    {
        out << "Iso graph number: "<< std::endl;
        out << get_graph_num_digs(num_vertices, get_graph_iso_num()) << std::endl;
    }

    return out.str();
}
