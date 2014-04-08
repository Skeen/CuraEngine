#ifndef _RANGE_INSET
#define _RANGE_INSET

template <typename Collection>
class RangeInset
{
    public:
    RangeInset(Collection& c, int start_inset, int stop_inset)
        : c(c), start_inset(start_inset), stop_inset(stop_inset) {}

    /*typename Collection::iterator*/ auto begin() const { return c.begin() + start_inset; }
    /*typename Collection::iterator*/ auto end() const { return c.end() - stop_inset; }

    private:
    Collection& c;
    int start_inset;
    int stop_inset;
};

template<typename Collection>
auto make_range_inset(Collection& c, int start_inset = 0, int stop_inset = 0)
{
    return RangeInset<Collection>(c, start_inset, stop_inset);
}

#endif //_RANGE_INSET

