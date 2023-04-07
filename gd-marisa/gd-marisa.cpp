#include <iostream>
#include <sstream>
#include <fstream>
#include <marisa/trie.h>

//TODO: Autosearch first bword
std::string search(std::string searchString, marisa::Agent *agent, marisa::Trie *trie, int byte){
  std::size_t longestlen = byte;

  agent->set_query(searchString.c_str());
  while (trie->common_prefix_search(*agent))
    longestlen = agent->key().length();

  return searchString.substr(0,longestlen);
}

void print_css() {
  std::stringstream ss;
  ss << R"(
    <style>
      .gd-marisa {
        font-size: 2rem;
        margin-bottom: 0.05em;
        margin-top: -0.2em;
        color: #1268c3;
        font-weight: normal;
      }

      .gd-marisa a {
        display: inline-block;
        font-weight: normal;
        color: royalblue;
        text-decoration: none;
        border-bottom: dashed 2px currentColor;
      }

      .gd-marisa a b {
        background-color: #ddeeff;
        border-radius: 0.2rem;
        font-weight: 500;
      }
    </style>
  )";

  std::cout << ss.str() << std::endl;
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " WORD SENTENCE [PATH_TO_DIC]" << std::endl;
    return 1;
  }
  std::string gdword = argv[1];
  std::string gdsearch = argv[2];
  const char* dict_path = argc > 3 ? argv[3] : "/usr/share/gd-tools/words.dic";

  marisa::Trie trie;

  std::ifstream file(dict_path);
  if (file.good())
    trie.load(dict_path);
  else
  {
    std::cout << "Error. The dictionary file: \"" << dict_path << "\" does not exist." << std::endl;
    return 1;
  }
  marisa::Agent agent;

  int byte, j = 0;
  std::cout << "<div class=\"gd-marisa\">" << std::endl;
  for (size_t i = 0; i < gdsearch.length(); i++)
  {
    // See `man charsets` -> Unicode for explanation of below
    if ((gdsearch[i] & 0xC0) == 0x80) // intermediate unicode char
      continue;
    else if ((gdsearch[i] & 0xF0) == 0xE0) // Start of 3 byte sequence
      byte = 3;
    else if ((gdsearch[i] & 0x80) == 0x00) // Start of ASCII character
      byte = 1;
    else if ((gdsearch[i] & 0xE0) == 0xC0)
      byte = 2;
    else if ((gdsearch[i] & 0xF8) == 0xF0)
      byte = 4;

    std::string bword = search(gdsearch.substr(i), &agent, &trie, byte);
    std::cout << "<a href=\"bword:" << bword << "\">";

    if (bword.compare(gdword) == 0)
      j = bword.length();
    if (j > 0)
    {
      std::cout << "<b>" << gdsearch.substr(i,byte) << "</b></a>";
      j-=byte;
    }
    else
      std::cout << gdsearch.substr(i,byte) << "</a>";
  }
  std::cout << "<br>";

  // Show available entries for smaller substrings
  std::string word;
  agent.set_query(gdword.c_str());
  while (trie.common_prefix_search(agent) && agent.key().length() != gdword.length())
  {
    word = std::string(agent.key().ptr(), agent.key().length());
    std::cout << "<a href=\"bword:" << word << "\">" << word << "</a><br>";
  }

  std::cout << "</div>" << std::endl;
  print_css();

  return 0;
}
