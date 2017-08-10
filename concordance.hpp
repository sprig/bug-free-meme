#include<stdexcept>
#include<string>
#include<map>

namespace kk {
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
}
