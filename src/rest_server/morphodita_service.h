// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <unordered_map>

#include "common.h"
#include "derivator/derivation_formatter.h"
#include "microrestd/microrestd.h"
#include "microrestd/pugixml.h"
#include "morpho/morpho.h"
#include "tagger/tagger.h"
#include "tagset_converter/tagset_converter.h"
#include "tokenizer/tokenizer.h"

namespace ufal {
namespace morphodita {

namespace microrestd = ufal::microrestd;

class morphodita_service : public microrestd::rest_service {
 public:
  typedef ufal::morphodita::morpho Morpho;
  typedef ufal::morphodita::morpho::guesser_mode Guesser;
  typedef ufal::morphodita::tagger Tagger;
  typedef ufal::morphodita::tokenizer Tokenizer;

  struct model_description {
    string rest_id, weblicht_id, file, acknowledgements;

    model_description(const string& rest_id, const string& weblicht_id, const string& file, const string& acknowledgements)
        : rest_id(rest_id), weblicht_id(weblicht_id), file(file), acknowledgements(acknowledgements) {}
  };

  bool init(const vector<model_description>& model_descriptions);

  virtual bool handle(microrestd::rest_request& req) override;

 private:
  static unordered_map<string, bool (morphodita_service::*)(microrestd::rest_request&)> handlers;

  // Models
  struct model_info {
    model_info(const string& rest_id, const string& weblicht_id, Tagger* tagger, Morpho* morpho, const string& acknowledgements)
        : rest_id(rest_id), weblicht_id(weblicht_id), acknowledgements(acknowledgements), tagger(tagger), morpho(morpho) {
      unique_ptr<Tokenizer> tokenizer(get_tokenizer());
      can_tokenize = tokenizer != nullptr;
    }

    string rest_id;
    string weblicht_id;
    const Tagger* get_tagger() const { return tagger.get(); }
    const Morpho* get_morpho() const { return tagger ? tagger->get_morpho() : morpho.get(); }
    Tokenizer* get_tokenizer() const { return tagger ? tagger->new_tokenizer() : morpho ? morpho->new_tokenizer() : nullptr; }
    bool can_tokenize;
    string acknowledgements;

   private:
    unique_ptr<Tagger> tagger;
    unique_ptr<Morpho> morpho;
  };
  vector<model_info> models;
  unordered_map<string, const model_info*> rest_models_map;
  unordered_map<string, const model_info*> weblicht_models_map;

  const model_info* load_rest_model(const string& rest_id, string& error);
  const model_info* load_weblicht_model(const string& weblicht_id, string& error);

  // REST service
  enum rest_output_mode_t {
    JSON,
    XML,
    VERTICAL,
  };
  struct rest_output_mode {
    rest_output_mode_t mode;

    rest_output_mode(rest_output_mode_t mode) : mode(mode) {}
    static bool parse(const string& mode, rest_output_mode& output);
  };

  class rest_response_generator : public microrestd::json_response_generator {
   public:
    rest_response_generator(const model_info* model, rest_output_mode output);

    virtual bool next(bool first) = 0;
    virtual bool generate() override;

   protected:
    bool first, last;
    rest_output_mode output;
  };

  bool handle_rest_models(microrestd::rest_request& req);
  bool handle_rest_tag(microrestd::rest_request& req);
  bool handle_rest_analyze(microrestd::rest_request& req);
  bool handle_rest_generate(microrestd::rest_request& req);
  bool handle_rest_tokenize(microrestd::rest_request& req);

  const string& get_rest_model_id(microrestd::rest_request& req);
  const char* get_data(microrestd::rest_request& req, string& error);
  morpho::guesser_mode get_guesser(microrestd::rest_request& req, string& error);
  tokenizer* get_tokenizer(microrestd::rest_request& req, const model_info* model, string& error);
  tagset_converter* get_convert_tagset(microrestd::rest_request& req, const Morpho& morpho, string& error);
  derivation_formatter* get_derivation_formatter(microrestd::rest_request& req, const Morpho& morpho, string& error);
  bool get_output_mode(microrestd::rest_request& req, rest_output_mode& mode, string& error);
  static bool get_line(const char*& data, string_piece& line);

  microrestd::json_builder json_models;
  static const char* json_mime;
  static const char* operation_not_supported;

  // WebLicht service
  bool handle_weblicht_tokenize(microrestd::rest_request& req);

  bool parse_tcf(const microrestd::rest_request& req, microrestd::pugi::xml_document& tcf, string& error);
  bool respond_tcf(microrestd::rest_request& req, const microrestd::pugi::xml_document& tcf);

  static const char* tcf_mime;
};

} // namespace morphodita
} // namespace ufal
