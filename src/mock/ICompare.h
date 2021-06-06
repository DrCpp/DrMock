#ifndef DRMOCK_SRC_MOCK_ICOMPARE_H
#define DRMOCK_SRC_MOCK_ICOMPARE_H

namespace drmock {

template<typename Base>
class ICompare
{
public:
  virtual ~ICompare() = default;
  virtual bool invoke(const Base&) const = 0;
};

// Check if `T1` is a `ICompare<T2>`.
template<typename T1, typename T2>
struct is_compare
{
  static constexpr bool value = std::is_base_of_v<ICompare<T2>, T1>;
};

template<typename T1, typename T2>
inline constexpr bool is_compare_v = is_compare<T1, T2>::value;

} // namespace drmock

#endif /* DRMOCK_SRC_MOCK_ICOMPARE_H */
