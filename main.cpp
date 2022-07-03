#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>
#include "lyndon.hpp"

void verify(char const *text,  // the input text
            int *nss,          // next smaller suffixes (nss)
            int *nss_lce,      // lces associated with nss
            int *pss,          // previous smaller suffixes (pss)
            int *pss_lce,      // lces associated with pss
            int n) {
  std::cout << "Verifying..." << std::endl;
  if (n <= 20) {
    for (int i = 0; i < n; i++) std::cout << text[i] << ":" << nss[i] << ", ";
    std::cout << std::endl;
  }

  int rate = std::max(n / 10, 1);

  auto lce = [&](int i, int j) {
    int lce = 0;
    while (j + lce < n && text[i + lce] == text[j + lce]) ++lce;
    return lce;
  };

  for (int i = 0; i < n; ++i) {
    if ((n - i - 1) % rate == 0) 
      std::cout << 100 - 10 * ((n - i - 1) / rate) << "% "  << std::flush;

    // check pss. we already verified pss[i'] and plce[i'] for i' < i such that
    // we can use these values for verification of pss[i] and plce[i]

    int ps = pss[i];
    int pl = pss_lce[i];
    
    for (int j = i - 1; j > ps; j = pss[j]) {
      auto l = lce(j, i);
      if (i + l < n && text[j + l] < text[i + l]) {
        std::cout << "pss[" << i << "]=" << j << ", but algorithm claims "
                  << "pss[" << i << "]=" << pss[j] << std::endl;
        std::abort();
      }
    }

    if (ps >= 0) {
      auto l = lce(ps, i);
      if (i + l >= n || text[ps + l] > text[i + l]) {
        std::cout << "pss[" << i << "]<" << ps << ", but algorithm claims "
                  << "pss[" << i << "]=" << ps << std::endl;
        std::abort();
      }

      if (l != pl) {
        std::cout << "plce[" << i << "]=" << l << ", but algorithm claims "
                  << "plce[" << i << "]=" << pl << std::endl;
        std::abort();
      }
    }
  }
  std::cout << std::endl;

  for (int i = n - 1; i >= 0; --i) {
     if (i % rate == 0) 
       std::cout << 100 - (10 * (i / rate)) << "% "  << std::flush;

    // check nss. we already verified nss[i'] and nlce[i'] for i' > i such that
    // we can use these values for verification of nss[i] and nlce[i]

    int ns = nss[i];
    int nl = nss_lce[i];

    for (int j = i + 1; j < ns; j = nss[j]) {
      auto l = lce(i, j);
      if (j + l >= n || text[i + l] > text[j + l]) {
        std::cout << "nss[" << i << "]=" << j << ", but algorithm claims "
                  << "nss[" << i << "]=" << nss[j] << std::endl;
        std::abort();
      }
    }

    if (ns < n) {
      auto l = lce(i, ns);
      if (ns + l < n && text[i + l] < text[ns + l]) {
        std::cout << "nss[" << i << "]>" << ns << ", but algorithm claims "
                  << "nss[" << i << "]=" << ns << std::endl;
        std::abort();
      }

      if (l != nl) {
        std::cout << "nlce[" << i << "]=" << l << ", but algorithm claims "
                  << "nlce[" << i << "]=" << nl << std::endl;
        std::abort();
      }
    }
  }

  std::cout << "\nError free!" << std::endl;
}

int main(int argc, char *argv[]) {
  if (argc != 3 && argc != 2) {
    std::cerr << "Wrong number of parameters." << std::endl;
    return -1;
  }

  static std::random_device seed;
  static std::mt19937_64 g(seed());
  static std::uniform_int_distribution<uint8_t> d(97, 98);

  std::string string;
  uint64_t max_size = (argc == 3) ? 
    std::stoll(argv[2]) : std::numeric_limits<uint64_t>::max();
    
  if (max_size >= std::numeric_limits<int>::max()) {
    max_size = std::numeric_limits<int>::max() - 1;
    std::cerr << "Input truncated to length " << max_size << "." << std::endl;
  }

  std::string file = argv[1];

  if (file[0] == '*') {
    string.resize(file.size() - 1);
    for (size_t i = 0; i < string.size(); i++) string[i] = file[i + 1];
    std::cout << "Literal input string." << std::endl;
  } else if (file == std::string("random")) {
    string.resize(max_size);
    for (size_t i = 0; i < max_size; ++i) string[i] = d(g);
    std::cout << "Random string generated." << std::endl;
  } else {
    std::ifstream t(argv[1]);
    t.seekg(0, std::ios::end);
    uint64_t size = std::min((uint64_t)t.tellg(), max_size);
    t.seekg(0, std::ios::beg);
    string.resize(size);
    std::copy_n((std::istreambuf_iterator<char>(t)), size, string.begin());
    std::cout << "String loaded from file:\n" << file << std::endl;
  }

  int n = string.size();  
  std::cout << "n = " << n << std::endl;
  if (n <= 80) {
    for (int i = 0; i < n; i++) std::cout << string[i];
    std::cout << std::endl;
  }

  std::vector<int> nss(n);
  std::vector<int> nss_lce(n);
  std::vector<int> pss(n);
  std::vector<int> pss_lce(n);

  lyndon(string.data(), nss.data(), nss_lce.data(), pss.data(), pss_lce.data(),
         n);

  verify(string.data(), nss.data(), nss_lce.data(), pss.data(), pss_lce.data(),
         n);

  return 0;
}
