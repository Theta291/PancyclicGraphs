#include "pancyclic.h"

Span::Span() : num_vertices(0) {}

Span::Span(int start, int end, int nvert) : start(start), end(end), num_vertices(nvert) {}

bool Span::contains(int vertex) const
{
    turning turn = get_turning(start, vertex, end);
    return turn == CLOCKWISE || turn == MID_IS_END || turn == MID_IS_START || turn == ALL_EQUAL;
}

bool Span::coincident(Span const& other) const
{
    return contains(other.start) && contains(other.end) && other.contains(start) && other.contains(end);
}
