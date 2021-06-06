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

} // namespace drmock

#endif /* DRMOCK_SRC_MOCK_ICOMPARE_H */
