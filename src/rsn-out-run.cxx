#include <TH1D.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("Hello ROOT tutorial !!!\n");

  for (int i = 0; i < argc; i++) {
    printf("argv[%d]=%s\n", i, argv[i]);
  }

  TH1D *h = new TH1D("h", "My Hist", 100, -10, 10);
  h->FillRandom("gaus", 1000);
  h->Print();

  return 0;
}
