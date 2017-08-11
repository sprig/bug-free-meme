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
#include<sstream>

namespace kk {
  namespace concordance {

    malformed_input::malformed_input(unsigned long long byte) : runtime_error(""), where(byte) {}

    std::string malformed_input::what()
    {
      std::stringstream ss;
      ss <<  "Invalid UTF-8 string! Error encountered at byte "<<this->where<<std::endl;
      return ss.str();
    }

    Concordance::Concordance(std::string separators) : separators(separators), encoded_codepoint("\0\0\0\0"), current_byte(0) {}

    void Concordance::next_byte(char ch) {
      ++this->total_bytes;
      if (ch>>7) {
        // This is part of a multibyte codepoint

        switch (ch<<1>>5) {
          // This is the first byte of a multibyte codepoint
        case 4: this->codepoint_bytes = 2;
          break;
        case 6: this->codepoint_bytes = 3;
          break;
        case 7: this->codepoint_bytes = 4;
          break;
          // This is some other byte of a multibyte codepoint
        default: break;
        }

        if (this->current_byte >= this->codepoint_bytes) {
	  // The current byte is malformed as there are more "continuation" bytes than are specified.
          throw malformed_input(this->total_bytes);
        }

        this->encoded_codepoint[this->current_byte++] = ch;
      } else {
	// We have a single byte codepoint.
	this->encoded_codepoint[0] = ch;
	this->current_byte = 1;
	this->codepoint_bytes = 1;
      }


      if (this->current_byte == this->codepoint_bytes) {
        // We have a complete codepoint. Test whether it is a separator
        if (this->separators.find(this->encoded_codepoint, 0, this->codepoint_bytes) == std::string::npos) {
          // If the current codepoint is not a separator, prepare to accept another one and add the current one to the current word
          this->current_word.append(this->encoded_codepoint, this->codepoint_bytes);
        } else {
          // If the current codepoint is a separator, Add the current word
          ++this->words[this->current_word];
          this->current_word = "";
        }

        this->codepoint_bytes = 0;
        this->current_byte = 0;
      }
    }


    void Concordance::read_line(std::string line) {
      for(auto ch : line) {
        this->next_byte(ch);
      }
    }

  }
}
