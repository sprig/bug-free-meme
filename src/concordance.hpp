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
#include<vector>
#include<utility>
#include<sstream>

namespace kk {
#ifndef KK_CONCORDANCE
#define KK_CONCORDANCE
  namespace concordance {

    typedef std::pair<std::string, unsigned long> VP;

    class malformed_input : std::runtime_error {
    private:
      std::stringstream ss;
    protected:
      unsigned long long where;
      std::string error;
    public:
      malformed_input(unsigned long long byte);
      virtual const char* what() const _NOEXCEPT;
      unsigned long long get_where();
    };

    struct _cImpl;

    class Concordance {
    private:
      _cImpl* impl;
    public:
      Concordance(std::string separators = u8" \t\n\r,.:|!?-––<>[](){}`;*&^%$#@~1234567890=/\U0000FEFF",
                  bool sort_by_frequency = false);
      ~Concordance();
      void sort_by_frequency(bool by_frequency);

      void next_byte(unsigned char ch);
      void process_string(std::string line);
      void add_word(std::string word);

      void commit_current_word();
      bool complete_codepoint_p();

      typedef std::vector<VP>::iterator iterator;
      iterator begin(bool by_frequency);
      iterator begin();
      iterator end();
    };

  }
#endif // KK_CONCORDANCE
}
