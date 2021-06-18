#ifndef DRMOCK_SRC_TEST_SKIP_TEST_H
#define DRMOCK_SRC_TEST_SKIP_TEST_H

#include <stdexcept>
#include <string>

namespace drtest {

class SkipTest : public std::exception
{
public:
  SkipTest(std::string expr);

  const char* what() const noexcept override;

private:
  std::string what_{};
};

} // namespace drtest

#endif /* DRMOCK_SRC_TEST_SKIP_TEST_H */
