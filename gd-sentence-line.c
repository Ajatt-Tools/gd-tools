#include <ctype.h>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage(char *program_name) {
  printf("Usage: %s [--translate] [--difficulty] %%GDSEARCH%% %%GDWORD%%\n",
         program_name);
}

int calculate_difficulty(char *word) {
  int num_vowels = 0;
  for (int i = 0; i < strlen(word); i++) {
    char c = tolower(word[i]);
    if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') {
      num_vowels++;
    }
  }
  if (num_vowels <= 2) {
    return 1; // easy
  } else if (num_vowels <= 4) {
    return 2; // medium
  } else {
    return 3; // hard
  }
}

char *remove_punctuations(char *s) {
  int i, j = 0;
  char *result = (char *)malloc(strlen(s) + 1);

  for (i = 0; s[i]; i++) {
    if (!ispunct(s[i])) {
      result[j++] = s[i];
    }
  }
  result[j] = '\0';

  return result;
}

int main(int argc, char *argv[]) {
  int use_difficulty_colors = 0, use_simplytranslate = 0;
  if (argc < 3) {
    print_usage(argv[0]);
    return 1;
  }

  int cur_arg = 1;
  // Parse flags
  while (cur_arg < argc - 2) {
    if (strcmp(argv[cur_arg], "--translate") == 0) {
      use_simplytranslate = 1;
      cur_arg++;
    } else if (strcmp(argv[cur_arg], "--difficulty") == 0) {
      use_difficulty_colors = 1;
      cur_arg++;
    } else {
      printf("Unknown flag: %s\n", argv[cur_arg]);
      print_usage(argv[0]);
      return 1;
    }
  }

  char *input_copy = strdup(argv[cur_arg]);
  char *word = strtok(input_copy, " ");
  printf("<span class=\"gd-marisa\">\n");
  while (word != NULL) {
    char *clean_word = remove_punctuations(word);
    char *bg_color = "";
    if (use_difficulty_colors) {
      int difficulty = calculate_difficulty(word);
      if (difficulty >= 3) {
        bg_color = "style=\"background-color: #ffcccc;\"";
      } else if (difficulty >= 2) {
        bg_color = "style=\"background-color: #ffffcc;\"";
      } else if (difficulty >= 1) {
        /* bg_color = "style=\"background-color: #ccffcc;\""; */
      }
    }
    printf("<a href=\"bword:%s\"%s %s>%s</a> ", clean_word,
           strcmp(clean_word, argv[cur_arg + 1]) == 0 ? " class=\"active\"" : "", bg_color,
           word);
    word = strtok(NULL, " ");
    free(clean_word);
  }
  free(input_copy);
  printf("</span>\n");

  if (use_simplytranslate) {
    CURL *curl = curl_easy_init();
    if (curl) {
      char *encoded_text = curl_easy_escape(curl, argv[cur_arg], 0);
      char url[1024];
      snprintf(url, sizeof(url),
               "https://simplytranslate.pussthecat.org/"
               "?engine=google&text=%s&sl=auto&tl=en",
               encoded_text);
      printf("<span><a class=\"line\" href=\"%s\">🌐</a></span>\n", url);
      curl_free(encoded_text);
      curl_easy_cleanup(curl);
    }
  }

  printf("<style>\n");
  printf(" .line { text-decoration: none; }\n");
  printf(".gd-marisa {\n");
  printf(" font-size: 2rem; display: inline-block;\n");
  printf(" margin: 0.05em 0.2em 0.05em 0.05em;\n");
  printf(" color: #1268c3;\n");
  printf("}\n");
  printf(".gd-marisa a {\n");
  printf(" color: royalblue;\n");
  printf(" text-decoration: none;\n");
  printf(" border-bottom: dashed 2px currentColor;\n");
  printf("}\n");
  printf(".gd-marisa a:hover {\n");
  printf(" background-color: #ddeeff;\n");
  printf("}\n");
  printf(".gd-marisa a.active {\n");
  printf(" background-color: #f5f5f5;\n");
  printf("}\n");
  printf(".gd-marisa a.active::before {\n");
  printf(" content: '[';\n");
  printf("}\n");
  printf(".gd-marisa a.active::after {\n");
  printf(" content: ']';\n");
  printf("}\n");
  printf("</style>\n");

  return 0;
}
