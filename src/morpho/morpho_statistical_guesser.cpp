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

#include "morpho_statistical_guesser.h"
#include "utils/binary_decoder.h"
#include "utils/small_stringops.h"

namespace ufal {
namespace morphodita {

void morpho_statistical_guesser::load(binary_decoder& data) {
  // Load tags and default tag
  tags.resize(data.next_2B());
  for (auto& tag : tags) {
    tag.resize(data.next_1B());
    for (unsigned i = 0; i < tag.size(); i++)
      tag[i] = data.next_1B();
  }
  default_tag = data.next_2B();

  // Load rules
  rules.load(data);
}

// Helper method for analyze.
static bool contains(morpho_statistical_guesser::used_rules* used, const string& rule) {
  if (!used) return false;

  for (auto& used_rule : *used)
    if (used_rule == rule)
      return true;

  return false;
}

// Produces unique lemma-tag pairs.
void morpho_statistical_guesser::analyze(const char* form, int form_len, vector<tagged_lemma>& lemmas, morpho_statistical_guesser::used_rules* used) {
  unsigned lemmas_initial_size = lemmas.size();

  // We have rules in format "suffix prefix" in rules.
  // Find the matching rule with longest suffix and of those with longest prefix.
  string rule_label; rule_label.reserve(12);
  int suffix_len = 0;
  for (; suffix_len < form_len; suffix_len++) {
    rule_label.push_back(form[form_len - (suffix_len + 1)]);
    if (!rules.at(rule_label.c_str(), rule_label.size(), [](pointer_decoder& data){ data.next<char>(data.next_2B()); }))
      break;
  }

  for (; suffix_len >= 0; suffix_len--) {
    rule_label.resize(suffix_len);
    rule_label.push_back(' ');

    const unsigned char* rule = nullptr;
    int rule_prefix_len = 0;
    for (int prefix_len = 0; prefix_len + suffix_len < form_len; prefix_len++) {
      if (prefix_len) rule_label.push_back(form[prefix_len - 1]);
      const unsigned char* found = rules.at(rule_label.c_str(), rule_label.size(), [](pointer_decoder& data){ data.next<char>(data.next_2B()); });
      if (!found) break;
      if (*(found += sizeof(uint16_t))) {
        rule = found;
        rule_prefix_len = prefix_len;
      }
    }

    if (rule) {
      rule_label.resize(suffix_len + 1 + rule_prefix_len);
      if (rule_label.size() > 1 && !contains(used, rule_label)) { // ignore rule ' '
        if (used) used->push_back(rule_label);
        for (int rules_len = *rule++; rules_len; rules_len--) {
          int pref_del_len = *rule++; const char* pref_del = (const char*)rule; rule += pref_del_len;
          int pref_add_len = *rule++; const char* pref_add = (const char*)rule; rule += pref_add_len;
          int suff_del_len = *rule++; const char* suff_del = (const char*)rule; rule += suff_del_len;
          int suff_add_len = *rule++; const char* suff_add = (const char*)rule; rule += suff_add_len;
          int tags_len = *rule++; const uint16_t* tags = (const uint16_t*)rule; rule += tags_len * sizeof(uint16_t);

          if (pref_del_len + suff_del_len > form_len ||
              (pref_del_len && !small_memeq(pref_del, form, pref_del_len)) ||
              (suff_del_len && !small_memeq(suff_del, form + form_len - suff_del_len, suff_del_len)))
            continue;

          string lemma;
          lemma.reserve(pref_add_len - pref_del_len + form_len + suff_add_len - suff_del_len);
          if (pref_add_len) lemma.append(pref_add, pref_add_len);
          if (pref_del_len + suff_del_len < form_len) lemma.append(form + pref_del_len, form_len - pref_del_len - suff_del_len);
          if (suff_add_len) lemma.append(suff_add, suff_add_len);
          while (tags_len--)
            lemmas.emplace_back(lemma, this->tags[*tags++]);
        }
      }
      break;
    }
  }

  // If nothing was found, use default tag.
  if (lemmas.size() == lemmas_initial_size)
    if (!contains(used, string())) {
      if (used) used->push_back(string());
      lemmas.emplace_back(string(form, form_len), tags[default_tag]);
    }
}

} // namespace morphodita
} // namespace ufal
