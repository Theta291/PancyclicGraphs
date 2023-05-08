#include "pancyclic.h"

turning get_turning(int start, int mid, int end)
{
    // This is basically detecting the parity of start, mid, end as a permutation

    // Degenerate cases:
    if ((start == mid) && (mid == end)) return ALL_EQUAL;
    if (start == mid) return MID_IS_START;
    if (mid == end) return MID_IS_END;


    // For it to represent a clockwise ordering, then some rotation must be sorted:
    if ((start < mid && mid < end) || (mid < end && end < start) || (end < start && start < mid))
        return CLOCKWISE;

    // Similar for counterclockwise
    if ((start > mid && mid > end) || (mid > end && end > start) || (end > start && start > mid))
        return COUNTERCLOCKWISE;

    // Just in case (but I think it's impossible)
    return TURNING_ERR;
}
