#ifndef _RANGE_INSET
#define _RANGE_INSET

template <typename Collection>
struct RangeInset
{
    RangeInset(Collection& c, int start_inset = 0, int stop_inset = 0) : c(c), start_inset(start_inset), stop_inset(stop_inset) {}

    auto begin() const -> typename Collection::iterator
    { return c.begin() + start_inset; }
    auto end() const -> typename Collection::iterator
    { return c.end() - stop_inset; } 

    Collection& c;
    int start_inset;
    int stop_inset;
};

template<typename Collection>
auto make_range_inset(Collection& c, int start_inset = 0, int stop_inset = 0)
    -> decltype(RangeInset<Collection>(c, start_inset, stop_inset))
{
    return RangeInset<Collection>(c, start_inset, stop_inset);
}

#endif //_RANGE_INSET

