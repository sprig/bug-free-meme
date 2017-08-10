#include"concordance.hpp"
#include<iostream>
#include<fstream>
#include<iterator>
#include<string>

int main(int argc, char** argv) {
  kk::concordance::Concordance c;
  std::istream* in;

  switch (argc) {
    // Read from standard input
  case 1: {
    // Don't skip reading whitespace
    in = &std::cin;
    break;
  }
    //Read from the given file
  case 2: {
    std::string filename = argv[1];
    in = new std::ifstream(filename, std::ios_base::binary);
    break;
  }
  default : {
    std::cerr << "Usage: conc [filename]" << std::endl;
    return 1;
  }
  }

  *in >> std::noskipws;
  for(std::istream_iterator<char> cit(*in), end = std::istream_iterator<char>(); cit != end; ++cit) {
    c.next_byte(*cit);
  }

  for (auto pair : c.counts()) {
    std::cout << pair.first << " " << pair.second << std::endl;
  }
  return 0;
}
