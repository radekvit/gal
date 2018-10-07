#include <random.h>
#include <random>

class RandClass {
 public:
  static RandClass& instance() {
    static RandClass rs(std::random_device{}());
    return rs;
  }

  std::mt19937_64 engine;

 private:
  RandClass(unsigned seed) : engine(seed) {}
};

unsigned gal_rand(unsigned upTo) {
  std::uniform_int_distribution<unsigned> dis(0, upTo - 1);
  return dis(RandClass::instance().engine);
}

unsigned gal_rand(unsigned from, unsigned upTo) {
  std::uniform_int_distribution<unsigned> dis(from, upTo - 1);
  return dis(RandClass::instance().engine);
}

double gal_rand(double upTo) {
  std::uniform_real_distribution dis(0.0, upTo);
  return dis(RandClass::instance().engine);
}

double gal_rand(double from, double upTo) {
  std::uniform_real_distribution dis(from, upTo);
  return dis(RandClass::instance().engine);
}