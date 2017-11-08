#ifndef FEFISMS_H_09912EAE
#define FEFISMS_H_09912EAE

#include <random>
#include <string>
#include <verbly.h>
#include <stdexcept>
#include <memory>
#include <twitter.h>

class fefisms {
public:

  fefisms(
    std::string configFile,
    std::mt19937& rng);

  void run() const;

private:

  std::mt19937& rng_;
  std::unique_ptr<verbly::database> database_;
  std::unique_ptr<twitter::client> client_;

};

#endif /* end of include guard: FEFISMS_H_09912EAE */
