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
  RandClass(size_t seed) : engine(seed) {}
};
/**
 * Get a random size_t in <0, upTo)
 */
size_t gal_rand(size_t upTo) {
  std::uniform_int_distribution<size_t> dis(0, upTo - 1);
  return dis(RandClass::instance().engine);
}
/**
 * Get a random size_t in <from, upTo)
 */
size_t gal_rand(size_t from, size_t upTo) {
  std::uniform_int_distribution<size_t> dis(from, upTo - 1);
  return dis(RandClass::instance().engine);
}
/**
 * Get a random double in <0, upTo)
 */
double gal_rand(double upTo) {
  std::uniform_real_distribution dis(0.0, upTo);
  return dis(RandClass::instance().engine);
}
/**
 * Get a random double in <from, upTo)
 */
double gal_rand(double from, double upTo) {
  std::uniform_real_distribution dis(from, upTo);
  return dis(RandClass::instance().engine);
}