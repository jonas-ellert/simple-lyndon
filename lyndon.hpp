#pragma once
#include <cstdint>
#include <limits>

void lyndon(uint8_t const *text,  // the input text
            int *nss,             // next smaller suffixes (nss)
            int *nss_lce,         // lces associated with nss
            int *pss,             // previous smaller suffixes (pss)
            int *pss_lce,         // lces associated with pss
            int n) {
  //
  // virtually append and prepend infinitely small sentinel symbols
  auto T_l = [&](int index) {
    return (index >= 0) ? ((int)text[index]) : ((int)-1);
  };
  auto T_r = [&](int index) -> int {
    return (index < n) ? ((int)text[index]) : ((int)-1);
  };

  // rhs = right hand side, the rightmost index that we already touched
  int rhs = -1;
  int d = 0;

  // the basic variables used throughout the algorithm
  int right, left, lce;

  // function for computing LCEs
  // - if possible, copy previously computed LCE
  // - otherwise, continue scanning at rhs
  auto compute_lce = [&](int left, int right, int known_lce = 0) {
    if (right + known_lce < rhs) {
      // one of the following conditions always holds:
      // nss[left - d] == right - d, or
      // pss[right - d] == left - d

      int copy_lce =
          (nss[left - d] == right - d) ? nss_lce[left - d] : pss_lce[right - d];

      // lce might be safe to entirely copy
      if (right + copy_lce < rhs) return copy_lce;

      // otherwise lce extends at least to right hand side
      known_lce = rhs - right;
    }

    // compute LCE by scanning; note that right + known_lce >= rhs
    while (T_l(left + known_lce) == T_r(right + known_lce)) {
      ++known_lce;
    }
    rhs = right + known_lce;
    d = right - left;

    return known_lce;
  };

  pss[0] = -1;
  pss_lce[0] = 0;

  // in one iteration: 
  // find pss[right], and all values left with nss[left] = right
  for (right = 1; right < n; ++right) {
    left = right - 1;
    lce = compute_lce(left, right, 0);

    // determine lex. order by comparing first mismatching symbol
    while (T_l(left + lce) > T_r(right + lce)) {
      nss[left] = right;
      nss_lce[left] = lce;

      if (lce == pss_lce[left]) {
        lce = compute_lce(pss[left], right, lce);
      } else if (lce > pss_lce[left]) {
        lce = pss_lce[left];
      }
      left = pss[left];
    }

    pss[right] = left;
    pss_lce[right] = lce;
  }

  // some indices do not have NSS
  left = n - 1;
  while (left >= 0) {
    nss[left] = n;
    nss_lce[left] = 0;
    left = pss[left];
  }
}
