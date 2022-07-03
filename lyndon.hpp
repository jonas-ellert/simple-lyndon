#pragma once

void lyndon(char const *text, int *nss, int *nlce, int *pss, int *plce, int n) {
  auto T_l = [&](int x) { return (x >= 0) ? ((int)text[x]) : ((int)-256); };
  auto T_r = [&](int x) { return (x < n) ? ((int)text[x]) : ((int)-256); };

  int d, rhs = -1;

  auto extend_lce = [&](int l, int r, int known_lce = 0) {
    if (r + known_lce < rhs) {
      known_lce = (nss[l - d] == r - d) ? nlce[l - d] : plce[r - d];
      if (r + known_lce < rhs) return known_lce;
      known_lce = rhs - r;
    }

    while (T_l(l + known_lce) == T_r(r + known_lce)) ++known_lce;

    rhs = r + known_lce;
    d = r - l;
    return known_lce;
  };

  for (int r = 0; r < n; ++r) {
    int l = r - 1;
    int lce = extend_lce(l, r, 0);
    while (T_l(l + lce) > T_r(r + lce)) {
      nss[l] = r;
      nlce[l] = lce;
      if (lce == plce[l]) {
        lce = extend_lce(pss[l], r, lce);
      } else if (lce > plce[l]) {
        lce = plce[l];
      }
      l = pss[l];
    }
    pss[r] = l;
    plce[r] = lce;
  }

  int l = n - 1;
  while (l >= 0) {
    nss[l] = n;
    nlce[l] = 0;
    l = pss[l];
  }
}
