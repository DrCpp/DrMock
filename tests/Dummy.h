#ifndef DRMOCK_TESTS_DUMMY_H
#define DRMOCK_TESTS_DUMMY_H

#include <vector>
#include <string>

#include <QObject>

namespace drmock {

class Dummy : public QObject
{
  Q_OBJECT
public slots:
  void bazzify(std::string&);

signals:
  void pass_by_ref(std::string&);
  void multiple_params(int, float&, const std::vector<int>&);
  void no_params();
};

} // namespace drmock

#endif /* DRMOCK_TESTS_DUMMY_H */
