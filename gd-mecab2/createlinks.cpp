#include <marisa/trie.h>
#include <iostream>
#include <sstream>

std::string search(std::string searchString, marisa::Agent *agent, marisa::Trie *trie){
  std::size_t longestlen = 3;

  agent->set_query(searchString);
  while (trie->common_prefix_search(*agent))
    longestlen = agent->key().length();

  return searchString.substr(0,longestlen);
}

void print_css() {
  std::stringstream ss;
  ss << R"(
    <style>
      .gd-mecab {
        font-size: 2rem;
        margin-bottom: 0.05em;
        margin-top: -0.2em;
        color: #1268c3;
        font-weight: normal;
      }

      .gd-mecab a {
        display: inline-block;
        font-weight: normal;
        color: royalblue;
        text-decoration: none;
        border-bottom: dashed 2px currentColor;
      }

      .gd-mecab a b {
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
    std::cout << "You need to provide 2 arguments." << std::endl;
    return 1;
  }
  std::string gdword = argv[1];
  std::string gdsearch = argv[2];

  marisa::Trie trie;
  trie.load("words.dic");
  marisa::Agent agent;

  std::cout << "<div class=\"gd-mecab\">" << std::endl;
  for (size_t i = 0; i < gdsearch.length(); i+=3)
    std::cout << "<a href=\"bword:" << search(gdsearch.substr(i), &agent, &trie) << "\">" << gdsearch.substr(i,3) << "</a>";
  std::cout << "</div>" << std::endl;
  print_css();

  return 0;
}
