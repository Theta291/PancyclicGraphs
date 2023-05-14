#include <stdexcept>
#include "pancyclic.h"

void Chord::reorder()
{
    start %= num_vertices;
    if (start < 0) start += num_vertices;
    end %= num_vertices;
    if (end < 0) end += num_vertices;
    if (end < start)
    {
        int tmp = start;
        start = end;
        end = tmp;
    }
}

Chord::Chord() : num_vertices(0), start(0), end(0) {}

Chord::Chord(int start, int end, int nvert) : start(start), end(end), num_vertices(nvert) {reorder();}

Chord::Chord(Chord const& other) : start(other.start), end(other.end), num_vertices(other.num_vertices) {}

void Chord::rotate(int rotate)
{
    start += rotate;
    end += rotate;
    reorder();
}

void Chord::rotate() {rotate(1);}

void Chord::reflect(int vertex)
{
    start = 2 * vertex - start;
    end = 2 * vertex - end;
    reorder();
}

void Chord::reflect() {reflect(0);}


bool Chord::crossing(Chord const& other) const
{
    // If the turnings are same and nondegenerate, then they are crossing
    turning t1 = get_turning(start, other.start, end);
    turning t2 = get_turning(end, other.end, start);
    return (t1 == t2) && (t1 == CLOCKWISE || t1 == COUNTERCLOCKWISE);
}


bool operator<(Chord const& a, Chord const& b)
{
    if (a.num_vertices != b.num_vertices)
        throw std::invalid_argument("Can only compare chords with the same number of vertices");
    return a.start == b.start ? a.end < b.end : a.start < b.start;
}

bool operator==(Chord const& a, Chord const& b)
{return a.num_vertices == b.num_vertices && a.start == b.start && a.end == b.end;}

std::ostream& operator<<(std::ostream& os, Chord const& chord)
{
    return os <<
        "Chord(num_verts: " << chord.num_vertices <<
        ", start: " << chord.start <<
        ", end: " << chord.end << ")";
}


unsigned long int Chord::max_num_chords(int nvert)
{return nvert * (nvert - 3) / 2;}
