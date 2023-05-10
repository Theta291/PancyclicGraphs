#ifndef PANCYCLIC_H
#define PANCYCLIC_H

#include <vector>
#include <unordered_set>
#include <functional>
#include <iostream>
#include <string>
#include <type_traits>


enum turning
{
    TURNING_ERR,
    ALL_EQUAL,
    MID_IS_START,
    MID_IS_END,
    CLOCKWISE,
    COUNTERCLOCKWISE
};

turning get_turning(int start, int mid, int end);


class Chord;
template<>
struct std::hash<Chord>;
class Chord
{
    private:
        // This will be an int for now so I can do start % num_vertices
        int num_vertices;
        int start;
        int end;

        void reorder();
    public:
        // For some reason, some random piece of code needs this
        Chord();
        Chord(int start, int end, int nvert);
        Chord(Chord const& other);

        void rotate(int rotation);
        void rotate();
        void reflect(int vertex);
        void reflect();

        bool crossing(Chord const& other) const;

        static unsigned long int max_num_chords(int nvert);

        friend bool operator<(Chord const& a, Chord const& b);
        friend bool operator==(Chord const& a, Chord const& b);
        friend bool operator!=(Chord const& a, Chord const& b) {return !(a==b);};
        friend std::ostream& operator<<(std::ostream& os, Chord const& chord);
        friend struct std::hash<Chord>;

        friend class Hamiltonian;
};

// For some reason, this has to be in the header
// I was trying to define it in chord.cpp, but that didn't work
namespace std {
    template<>
    struct hash<Chord>
    {
        std::size_t operator() (Chord const& c) const noexcept
        {
            // TODO: research into better hash function?
            // Don't care abt security, just have to minimize collisions, if possible
            std::size_t h1 = std::hash<int>{}(c.start);
            std::size_t h2 = std::hash<int>{}(c.end);
            // Not hashing num_vertices because I don't think I'll be using multiple chords of different num_vertices in the same table
            return h1 ^ (h2 << 1);
        }
    };
}


class Span {
    private:
        int num_vertices;
        int start;
        int end;
    public:
        Span();
        Span(int start, int end, int nvert);

        bool contains(int vertex) const;
        bool coincident(Span const& other) const;
};


class Hamiltonian
{
    using inner_AL_type = std::unordered_set<int>;
    using outer_AL_type = std::unordered_map<int, inner_AL_type>;
    using inner_AL_iter = decltype(std::declval<inner_AL_type>().cbegin());
    using outer_AL_iter = decltype(std::declval<outer_AL_type>().cbegin());
    

    private:
        int num_vertices;
        int num_chords;
        outer_AL_type chords_al;

        void reset_al();
        std::unordered_map<int, Hamiltonian*> get_crossing_comp_hamil_map() const;

    public:
        Hamiltonian();
        Hamiltonian(int nvert);
        Hamiltonian(int nvert, std::vector<short> const& graph_num);
        // Implement this constructor as a factory method because template constructor is not possible
        // Also have to put implementation in header since it's a nonspecialized template
        // TODO: How to guarantee Iter iterates over chords?
        template<typename Iter>
        static Hamiltonian from_iter(int nvert, Iter chords_begin, Iter chords_end)
        {
            Hamiltonian ret_graph(nvert);
            // TODO: check if chord has correct number of verts (?)
            for (auto i = chords_begin; i != chords_end; ++i) ret_graph.add_chord(*i);
            return ret_graph;
        }

        int get_num_vertices() const {return num_vertices;}
        int get_num_chords() const {return num_chords;}

        struct ChordsRange
        {
            private:
                Hamiltonian const* const hamil;
            public:
                ChordsRange(Hamiltonian const* theHamil) : hamil(theHamil) {};

                class ChordsALIterator
                {
                    private:
                        Chord* curr_chord;
                        outer_AL_type const* const outer;
                        std::unordered_set<int> const* inner;
                        outer_AL_iter outer_iter;
                        inner_AL_iter inner_iter;

                        void reset_inner();
                        void update_chord();
                        void seek_chord();

                    public:
                        ChordsALIterator(Hamiltonian const* hamil, bool end=false);
                        ~ChordsALIterator();
                        Chord operator*();
                        Chord* operator->();
                        ChordsALIterator& operator++();
                        ChordsALIterator operator++(int);

                        friend bool operator==(ChordsALIterator const& a, ChordsALIterator const& b);
                        friend bool operator!=(ChordsALIterator const& a, ChordsALIterator const& b) {return !(a==b);};
                };

                ChordsALIterator begin() const;
                ChordsALIterator end() const;
        };
        ChordsRange chords() const;

        std::vector<Hamiltonian> get_crossing_components() const;
        std::unordered_map<Chord, Hamiltonian*> get_crossing_components_map() const;

        // I use vectors of shorts to emulate arbitrary length nums
        // This is important because the number of bits is equal to the number of possible chords
        // So even a long int would only support a hamiltonian of degree up to 12
        std::vector<short> get_graph_num() const;
        std::vector<short> get_graph_iso_num() const;
        static std::string get_graph_num_digs(int nverts, std::vector<short> graph_num);

        // TODO: Check chord num verts
        void add_chord(Chord const& c);

        void chord_based_transform(std::function<Chord(Chord)> transform);
        void rotate(int rotation);
        void rotate();
        void reflect(int vertex);
        void reflect();

        std::string describe(bool w_graph_num=true, bool w_graph_iso_num=true) const;
};


#endif

// TODO: Consider the independent component rotations as permutations.
// The equivalence class is described as the orbit of the graph under the set of allowed permutations, which is closed.
// However, this doesn't apply when we move one chord component to touch another chord component

// TODO: Think of algo to make tree out of chord components
// Pretty sure it involves considering each vertex and checking which spans it falls into in a specific hamiltonian
// Idea: loop through each span in the hamiltonian, and check which chords create it, and which components they're in
// Those components are coincident
// Brainstorm:
// - 
