#include "docs/example_mock.h"

template class ::drmock::Method<ns::Interface, int, foo :: Foo>;
template class ::drmock::Method<ns::Interface, float, float>;
template class ::drmock::Method<ns::Interface, void, int, std :: vector < float >>;
template class ::drmock::Method<ns::Interface, void, int, std :: unordered_map < std :: string , std :: shared_ptr < double >>>;
template class ::drmock::Method<ns::Interface, void, std :: string>;
template class ::drmock::Method<ns::Interface, void, std :: string>;