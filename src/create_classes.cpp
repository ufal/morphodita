// This file is part of MorphoDiTa.
//
// Copyright 2013 by Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// MorphoDiTa is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// MorphoDiTa is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with MorphoDiTa.  If not, see <http://www.gnu.org/licenses/>.

#include <algorithm>
#include <memory>
#include <unordered_map>

#include "common.h"
#include "morpho/raw_morpho_dictionary_reader.h"
#include "utils/new_unique_ptr.h"

using namespace ufal::morphodita;

typedef pair<string, string> tagged_form;

int max_suffix_len;

class trie {
 public:
  trie() : depth(0) {}

  void add(const char* str) {
    if (!*str) return;

    for (auto& child : children)
      if (child.first == *str) {
        child.second->add(str + 1);
        depth = max(depth, 1 + child.second->depth);
        return;
      }
    children.emplace_back(*str, new_unique_ptr<trie>());
    children.back().second->add(str + 1);
    depth = max(depth, 1 + children.back().second->depth);
  }

  string find_candidate_prefix() {
    string current, best;
    int best_length = 0;
    find_candidate_prefix(current, best, best_length, 0);
    return best;
  }
  void find_candidate_prefix(string& current, string& best, int& best_length, int length) {
    if (depth < max_suffix_len && length > best_length) {
      best = current;
      best_length = length;
    }
    for (auto& child : children) {
      current.push_back(child.first);
      child.second->find_candidate_prefix(current, best, best_length, children.size() == 1 ? length + 1 : 1);
      current.resize(current.size() - 1);
    }
  }

  vector<pair<char, unique_ptr<trie>>> children;
  int depth;
};

unordered_map<string, int> classes_map;
vector<string> classes;

void add_class(vector<tagged_form>::const_iterator begin, vector<tagged_form>::const_iterator end) {
  if (begin == end) runtime_errorf("Empty range in add_class!");

  int common_prefix = 0;
  while (common_prefix < int(begin->first.size()) && begin->first[common_prefix] == (end-1)->first[common_prefix]) common_prefix++;

  string clas;
  for (auto it = begin; it != end; it++) {
    if (!clas.empty()) clas.push_back('\t');
    clas.append(it->first, common_prefix, string::npos);
    clas.push_back('\t');
    clas.append(it->second);

  }

  auto it = classes_map.emplace(clas, classes.size());
  if (it.second)
    classes.push_back(clas);

  printf("\t%s\t%d", begin->first.substr(0, common_prefix).c_str(), it.first->second);
}

int main(int argc, char* argv[]) {
  if (argc <= 1) runtime_errorf("Usage: %s max_suffix_len", argv[0]);
  max_suffix_len = stoi(argv[1]);

  raw_morpho_dictionary_reader raw(stdin);
  string lemma;
  vector<tagged_form> forms;
  eprintf("Creating classes.\n");
  while(raw.next_lemma(lemma, forms)) {
    sort(forms.begin(), forms.end());
    auto forms_end = unique(forms.begin(), forms.end());
    if (forms_end != forms.end()) {
      eprintf("Warning: repeated form-tag in lemma %s.\n", lemma.c_str());
      forms.erase(forms_end, forms.end());
    }

    printf("%s", lemma.c_str());
    while (!forms.empty()) {
      trie t;
      for (auto& form : forms)
        t.add(form.first.c_str());

      string prefix = t.find_candidate_prefix();
      for (auto start = forms.begin(); start != forms.end(); start++)
        if (start->first.compare(0, prefix.size(), prefix) == 0) {
          auto end = start;
          while (end != forms.end() && end->first.compare(0, prefix.size(), prefix) == 0) end++;

          add_class(start, end);
          forms.erase(start, end);
          break;
        }
    }
    printf("\n");
  }
  eprintf("Done, %d classes created.\n", int(classes.size()));

  printf("\n");
  for (auto clas : classes)
    printf("%s\n", clas.c_str());

  return 0;
}
