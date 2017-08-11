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

#include<stdexcept>
#include<string>
#include<map>

namespace kk {
#ifndef KK_CONCORDANCE
#define KK_CONCORDANCE
  namespace concordance {
    class Concordance {
    protected:
      std::string separators;
      // codepoint needs up to 4 chars + null terminator (not necessary, just for convenience).
      char encoded_codepoint[5];
      int current_byte;
      int codepoint_bytes;
      unsigned long long total_bytes;
      std::string current_word;
      std::map<std::string,unsigned long> words;
    public:
      Concordance(std::string separators = u8" \t\n\r,.:|!?'-[](){}`;\U0000FEFF");
      void next_byte(char ch);
      void read_line(std::string line);
      inline std::map<std::string,unsigned long> counts() {return this->words;};
    };

    class malformed_input : std::runtime_error {
    protected:
      unsigned long long where;
    public:
      malformed_input(unsigned long long byte);
      virtual std::string what();
      unsigned long long get_where() {return this->where;};
    };
  }
#endif // KK_CONCORDANCE
}
