#ifndef DRTEST_TESTS_MOCKER_IFILEEXTENSION_H
#define DRTEST_TESTS_MOCKER_IFILEEXTENSION_H

namespace outer { namespace inner {

class IFileExtension
{
public:
  virtual ~IFileExtension() = default;

  virtual void f() = 0;
};

}} // namespace outer::inner

#endif /* DRTEST_TESTS_MOCKER_IFILEEXTENSION_H */
