// Copyright (c) 2017 Konstantin Kliakhandler

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include"concordance.hpp"
#include<iostream>
#include<fstream>
#include<iterator>
#include<string>

int main(int argc, char** argv) {
    std::istream* in;
    kk::concordance::Concordance c;

    switch (argc) {
    case 1: {
        // Read from standard input

        in = &std::cin;
        break;
    }
    case 2: {
        // Read from the given file or sort by frequency

        std::string arg = argv[1];
        if (arg == std::string("-F")) {
            // Read from input and sort by frequency
            c.sort_by_frequency(true);
            in = &std::cin;
        } else {
            // Read from file and sort by word
            in = new std::ifstream(arg, std::ios_base::binary);
        }
        break;
    }
    case 3: {
        // Read from the given file and sort by frequency

        std::string arg = argv[1];
        if (arg == std::string("-F")) {
            c.sort_by_frequency(true);
            std::string filename = argv[2];
            in = new std::ifstream(filename, std::ios_base::binary);
            break;
        }
    }
    default : {
        // Display help/usage
        std::cerr << "Usage: conc [-F] [filename]" << std::endl;
        return 1;
    }
    }

    *in >> std::noskipws; // Don't skip reading whitespace
    for(std::istream_iterator<char> cit(*in), end; cit != end; ++cit) {
        c.next_byte(*cit);
    }

    if (!c.complete_codepoint_p()) {
        std::cerr << "Incomplete UTF-8 codepoint at the end of output!";
        return 1;
    }

    // Output the sorted values
    for(auto pair : c) {
        std::cout << pair.first << " " << pair.second << std::endl;
    }

    return 0;
}
