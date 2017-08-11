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
#include<algorithm>
#include<vector>
#include<unordered_map>

namespace kk {
  namespace concordance {

    struct _cImpl {
      unsigned long long total_bytes;
      std::string separators;
      // utf-8 codepoint requires up to 4 bytes + null terminator.
      char encoded_codepoint[5];
      int current_byte;
      int codepoint_bytes;
      std::string current_word;
      std::unordered_map<std::string,unsigned long> words;
      std::vector<VP> ordered_words;
      bool by_frequency;
      _cImpl() : encoded_codepoint("\0\0\0\0") {};
    };

    // Constructor for the exception that is thrown when the UTF-8 is invalid.
    // byte is the location in the stream where the error occured.
    malformed_input::malformed_input(unsigned long long byte) : runtime_error(""), where(byte) {}


    // Output an error string.
    std::string malformed_input::what()
    {
      std::stringstream ss;
      ss <<  "Invalid UTF-8 string! Error encountered at byte "<<this->where<<std::endl;
      return ss.str();
    }

    // Getter for `where` which contains the erroneous byte.
    unsigned long long malformed_input::get_where() {
      return this->where;
    }


    /// Constructor for the concordance class.
    /// `separators` are a string of characters that separate between words.
    /// `sort_by_frequency` sets the default sorting strategy.
    Concordance::Concordance(std::string separators, bool sort_by_frequency) {
      this->impl = new _cImpl();
      this->impl->separators = separators;
      this->impl->current_byte = 0;
      this->impl->by_frequency = sort_by_frequency;
    }
    Concordance::~Concordance() {
      delete this->impl;
    }

    /// Process the next byte in a UTF-8 stream.
    /// `ch` is a byte that is part of a utf-8 encoded stream.
    void Concordance::next_byte(unsigned char ch) {
      ++this->impl->total_bytes; // Byte counter

      if (ch>>7) {
        // This is part of a multibyte codepoint.
	// Ref: RFC-3629

        switch (ch>>4) {
	  // Look at the three bottom bites of the top four bits.
	  // This is the first byte of a multibyte codepoint.
        case 15: this->impl->codepoint_bytes = 4;
          break;
        case 14: this->impl->codepoint_bytes = 3;
          break;
        case 12:
	case 13: this->impl->codepoint_bytes = 2;
          break;

          // This is some other byte of a multibyte codepoint.
        default: break;
        }

        if (this->impl->current_byte >= this->impl->codepoint_bytes) {
          // The current byte is malformed as there are more "continuation" bytes than are specified. Bail.
          throw malformed_input(this->impl->total_bytes);
        }

        this->impl->encoded_codepoint[this->impl->current_byte++] = ch;
      } else {
        // We have a single byte codepoint.
        this->impl->encoded_codepoint[0] = ch;
        this->impl->current_byte = 1;
        this->impl->codepoint_bytes = 1;
      }

      if (this->impl->current_byte == this->impl->codepoint_bytes) {
        // We have a complete codepoint. Test whether it is a separator.
        if (this->impl->separators.find(this->impl->encoded_codepoint, 0, this->impl->codepoint_bytes) == std::string::npos) {
          // If the current codepoint is not a separator,
	  // prepare to accept another one and add the current one to the current word.
          this->impl->current_word.append(this->impl->encoded_codepoint, this->impl->codepoint_bytes);
        } else {
          // If the current codepoint is a separator, Add the current word.
          this->commit_current_word();
        }

	// Reset current character
        this->impl->codepoint_bytes = 0;
        this->impl->current_byte = 0;
      }
    }

    /// Add a word to the counter.
    /// `word` is the word to be added.
    void Concordance::add_word(std::string word) {
      auto temp_word = this->impl->current_word;
      this->impl->current_word = word;
      this->commit_current_word();
      this->impl->current_word = temp_word;
    }

    /// Commit the currently accumulating word to the counter
    ///
    void Concordance::commit_current_word() {
      if (this->impl->current_word.length() > 0){
        ++this->impl->words[this->impl->current_word];
        this->impl->current_word = "";
      }
    }

    /// process the given utf-8 string
    /// `line` is a string to be processed.
    /// Note: line does not have to end in a whole utf-8 character,
    /// so it is suitable for e.g. processing data from a network socket.
    void Concordance::process_string(std::string line) {
      for(auto ch : line) {
        this->next_byte(ch);
      }
    }

    /// Comparator between pair objects.
    /// When initialized with 'first' as true,
    /// it does lexicographic comparison (i.e. compare second element if first equal).
    /// Otherwise, reverse lexicographical (i.e. compare first element if second equal).
    struct cmp_pair {
    private:
      bool first;
    public:
      cmp_pair(bool first) : first(first) {};

      bool operator()(const std::pair<std::string, unsigned long>& l,
		      const std::pair<std::string, unsigned long>& r) {
        return (this->first ?
		((l.first < r.first) || ((l.first == r.first) && (l.second < r.second))) :
		((l.second < r.second) || ((l.second == r.second) && (l.first < r.first))));
      }
    };

    /// Return an iterator to the concordance table.
    /// `by_frequency` makes the order of the iterated elements be according to their appearence.
    Concordance::iterator Concordance::begin(bool by_frequency) {
      bool saved_by_freq = this->impl->by_frequency;
      this->impl->by_frequency = by_frequency;
      auto beg = this->begin();
      this->impl->by_frequency = saved_by_freq;
      return beg;
    }

    /// Return an iterator to the concordance table, sorted by the default method.
    ///
    Concordance::iterator Concordance::begin() {
      this->impl->ordered_words.assign(this->impl->words.begin(), this->impl->words.end());
      std::sort(this->impl->ordered_words.begin(), this->impl->ordered_words.end(), cmp_pair(!this->impl->by_frequency));
      return this->impl->ordered_words.begin();
    }

    /// Return an iterator to the end of the concordance table.
    ///
    Concordance::iterator Concordance::end() {
      return this->impl->ordered_words.end();
    }

    /// Set the default sorting method.
    /// If `freq` then sort by frequency.
    void Concordance::sort_by_frequency(bool freq) {
      this->impl->by_frequency = freq;
    }

    /// Return true if the bytes read so far make up a complete codepoint.
    ///
    bool Concordance::complete_codepoint_p() {
      return (this->impl->current_byte == this->impl->codepoint_bytes);
    }
  }
}
