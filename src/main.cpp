#include <iostream>
#include <string>
#include <r2grap.h>

int main(int argc, char *argv[]){
  if(argc > 2 || argc == 1) return 0;
  auto grap = new R2grap::R2grapGl(std::string(argv[1]));
  //grap->run();
}

