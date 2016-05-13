// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "morphodita_service.h"

namespace ufal {
namespace morphodita {

// Init the MorphoDiTa service -- load the models
bool morphodita_service::init(const vector<model_description>& model_descriptions) {
  if (model_descriptions.empty()) return false;

  // Load models
  models.clear();
  rest_models_map.clear();
  weblicht_models_map.clear();
  for (auto& model_description : model_descriptions) {
    Morpho* morpho = nullptr;
    Tagger* tagger = nullptr;

    // Heuristics -- if .dict, try morpho, otherwise tagger.
    if (model_description.file.size() >= 5 &&
        model_description.file.compare(model_description.file.size() - 5, 5, ".dict") == 0)
      morpho = morpho::load(model_description.file.c_str());
    else
      tagger = tagger::load(model_description.file.c_str());
    if (!morpho && !tagger) return false;

    // Store the model
    models.emplace_back(model_description.rest_id, model_description.weblicht_id, tagger, morpho, model_description.acknowledgements);
  }

  // Fill rest_models_map with model name and aliases
  for (auto& model : models) {
    // Fail if this model id is aready in use.
    if (!rest_models_map.emplace(model.rest_id, &model).second) return false;

    // Create (but not overwrite) id without version.
    for (unsigned i = 0; i+1+6 < model.rest_id.size(); i++)
      if (model.rest_id[i] == '-') {
        bool is_version = true;
        for (unsigned j = i+1; j < i+1+6; j++)
          is_version = is_version && model.rest_id[j] >= '0' && model.rest_id[j] <= '9';
        if (is_version)
          rest_models_map.emplace(model.rest_id.substr(0, i) + model.rest_id.substr(i+1+6), &model);
      }

    // Create (but not overwrite) hyphen-separated prefixes.
    for (unsigned i = 0; i < model.rest_id.size(); i++)
      if (model.rest_id[i] == '-')
        rest_models_map.emplace(model.rest_id.substr(0, i), &model);
  }
  // Default model
  rest_models_map.emplace(string(), &models.front());

  // Fill weblicht_models_map with weblicht_id
  for (auto& model : models)
    if (!model.weblicht_id.empty())
      // Fail if model with same weblicht_id already exists.
      if (!weblicht_models_map.emplace(model.weblicht_id, &model).second) return false;

  // Init REST service
  json_models.clear().object().indent().key("models").indent().object();
  for (auto& model : models) {
    json_models.indent().key(model.rest_id).indent().array();
    if (model.get_tagger()) json_models.value("tag");
    if (model.get_morpho()) json_models.value("analyze");
    if (model.get_morpho()) json_models.value("generate");
    if (model.can_tokenize) json_models.value("tokenize");
    json_models.close();
  }
  json_models.indent().close().indent().key("default_model").indent().value(model_descriptions.front().rest_id).finish(true);

  return true;
}

// Handlers with their URLs
unordered_map<string, bool (morphodita_service::*)(microrestd::rest_request&)> morphodita_service::handlers = {
  // REST service
  {"/models", &morphodita_service::handle_rest_models},
  {"/tag", &morphodita_service::handle_rest_tag},
  {"/analyze", &morphodita_service::handle_rest_analyze},
  {"/generate", &morphodita_service::handle_rest_generate},
  {"/tokenize", &morphodita_service::handle_rest_tokenize},
  // WebLicht service
  {"/weblicht/tokenize", &morphodita_service::handle_weblicht_tokenize},
};

// Handle a request using the specified URL/handler map
bool morphodita_service::handle(microrestd::rest_request& req) {
  auto handler_it = handlers.find(req.url);
  return handler_it == handlers.end() ? req.respond_not_found() : (this->*handler_it->second)(req);
}

// Load selected model
const morphodita_service::model_info* morphodita_service::load_rest_model(const string& rest_id, string& error) {
  auto model_it = rest_models_map.find(rest_id);
  if (model_it == rest_models_map.end())
    return error.assign("Requested model '").append(rest_id).append("' does not exist.\n"), nullptr;

  return model_it->second;
}
const morphodita_service::model_info* morphodita_service::load_weblicht_model(const string& weblicht_id, string& error) {
  auto model_it = weblicht_models_map.find(weblicht_id);
  if (model_it == weblicht_models_map.end())
    return error.assign("Requested model '").append(weblicht_id).append("' does not exist.\n"), nullptr;

  return model_it->second;
}

// REST service
inline microrestd::string_piece sp(string_piece str) { return microrestd::string_piece(str.str, str.len); }
inline microrestd::string_piece sp(const char* str, size_t len) { return microrestd::string_piece(str, len); }

const char* morphodita_service::json_mime = "application/json";
const char* morphodita_service::operation_not_supported = "Required operation is not supported by the chosen model.\n";

morphodita_service::rest_response_generator::rest_response_generator(const model_info* model, rest_output_mode output)
  : first(true), last(false), output(output) {
  json.object();
  json.indent().key("model").indent().value(model->rest_id);
  json.indent().key("acknowledgements").indent().array();
  json.indent().value("http://ufal.mff.cuni.cz/morphodita#morphodita_acknowledgements");
  if (!model->acknowledgements.empty()) json.indent().value(model->acknowledgements);
  json.indent().close().indent().key("result").indent();
  if (output.mode == JSON) json.array();
}

bool morphodita_service::rest_response_generator::generate() {
  if (last) return false;

  if (!next(first)) {
    if (output.mode == JSON) json.close();
    json.finish(true);
    last = true;
  }
  first = false;
  return true;
}

bool morphodita_service::rest_output_mode::parse(const string& str, rest_output_mode& output) {
  if (str.compare("json") == 0) return output.mode = JSON, true;
  if (str.compare("xml") == 0) return output.mode = XML, true;
  if (str.compare("vertical") == 0) return output.mode = VERTICAL, true;
  return false;
}

// REST service handlers

bool morphodita_service::handle_rest_models(microrestd::rest_request& req) {
  return req.respond(json_mime, json_models);
}

bool morphodita_service::handle_rest_tag(microrestd::rest_request& req) {
  string error;
  auto rest_id = get_rest_model_id(req);
  auto model = load_rest_model(rest_id, error);
  if (!model) return req.respond_error(error);
  if (!model->get_tagger()) return req.respond_error(operation_not_supported);

  auto data = get_data(req, error); if (!data) return req.respond_error(error);
  auto guesser = get_guesser(req, error); if (guesser < 0) return req.respond_error(error);
  unique_ptr<Tokenizer> tokenizer(get_tokenizer(req, model, error)); if (!tokenizer) return req.respond_error(error);
  unique_ptr<tagset_converter> converter(get_convert_tagset(req, *model->get_morpho(), error)); if (!converter) return req.respond_error(error);
  unique_ptr<derivation_formatter> derivation(get_derivation_formatter(req, *model->get_morpho(), error)); if (!derivation) return req.respond_error(error);
  rest_output_mode output(XML); if (!get_output_mode(req, output, error)) return req.respond_error(error);

  class generator : public rest_response_generator {
   public:
    generator(const model_info* model, const char* data, const Tagger* tagger, Guesser guesser, Tokenizer* tokenizer, tagset_converter* converter, derivation_formatter* derivation, rest_output_mode output)
        : rest_response_generator(model, output), tagger(tagger), guesser(guesser), tokenizer(tokenizer), converter(converter), derivation(derivation), unprinted(data) {
      tokenizer->set_text(data);
    }

    bool next(bool first) {
      if (!tokenizer->next_sentence(&forms, nullptr)) {
        if (output.mode == XML && *unprinted) json.value_xml_escape(unprinted, true);
        if (output.mode == JSON && !first && *unprinted) json.key("spaces").value(unprinted);
        if (output.mode == JSON && !first) json.close().close();
        return false;
      }

      tagger->tag(forms, tags, guesser);

      for (unsigned i = 0; i < forms.size(); i++) {
        converter->convert(tags[i]);
        derivation->format_derivation(tags[i].lemma);
        switch (output.mode) {
          case VERTICAL:
            json.value(sp(forms[i]), true).value("\t", true)
                .value(sp(tags[i].lemma), true).value("\t", true)
                .value(sp(tags[i].tag), true).value("\n", true);
            break;
          case XML:
            if (unprinted < forms[i].str) json.value_xml_escape(sp(unprinted, forms[i].str - unprinted), true);
            if (!i) json.value("<sentence>", true);
            json.value("<token lemma=\"", true).value_xml_escape(sp(tags[i].lemma), true)
                .value("\" tag=\"", true).value_xml_escape(sp(tags[i].tag), true)
                .value("\">", true).value_xml_escape(sp(forms[i]), true)
                .value("</token>", true);
            break;
          case JSON:
            if ((i || !first) && unprinted < forms[i].str) json.key("space").value(sp(unprinted, forms[i].str - unprinted));
            if (i || !first) json.close();
            if (!i && !first) json.close();
            if (!i) json.array();
            json.object().key("token").value(sp(forms[i])).key("lemma").value(tags[i].lemma).key("tag").value(tags[i].tag);
            break;
        }
        unprinted = forms[i].str + forms[i].len;
      }
      if (output.mode == VERTICAL) json.value("\n", true);
      if (output.mode == XML) json.value("</sentence>", true);

      return true;
    }

   private:
    const Tagger* tagger;
    Guesser guesser;
    vector<tagged_lemma> analyses;
    unique_ptr<Tokenizer> tokenizer;
    unique_ptr<tagset_converter> converter;
    unique_ptr<derivation_formatter> derivation;
    const char* unprinted;
    vector<string_piece> forms;
    vector<tagged_lemma> tags;
  };
  return req.respond(json_mime, new generator(model, data, model->get_tagger(), guesser, tokenizer.release(), converter.release(), derivation.release(), output));
}

bool morphodita_service::handle_rest_analyze(microrestd::rest_request& req) {
  string error;
  auto rest_id = get_rest_model_id(req);
  auto model = load_rest_model(rest_id, error);
  if (!model) return req.respond_error(error);
  if (!model->get_morpho()) return req.respond_error(operation_not_supported);

  auto data = get_data(req, error); if (!data) return req.respond_error(error);
  auto guesser = get_guesser(req, error); if (guesser < 0) return req.respond_error(error);
  unique_ptr<Tokenizer> tokenizer(get_tokenizer(req, model, error)); if (!tokenizer) return req.respond_error(error);
  unique_ptr<tagset_converter> converter(get_convert_tagset(req, *model->get_morpho(), error)); if (!converter) return req.respond_error(error);
  unique_ptr<derivation_formatter> derivation(get_derivation_formatter(req, *model->get_morpho(), error)); if (!derivation) return req.respond_error(error);
  rest_output_mode output(XML); if (!get_output_mode(req, output, error)) return req.respond_error(error);

  class generator : public rest_response_generator {
   public:
    generator(const model_info* model, const char* data, rest_output_mode output, const Morpho* morpho, Guesser guesser, Tokenizer* tokenizer, tagset_converter* converter, derivation_formatter* derivation)
        : rest_response_generator(model, output), morpho(morpho), guesser(guesser), tokenizer(tokenizer), converter(converter), derivation(derivation), unprinted(data) {
      tokenizer->set_text(data);
    }

    bool next(bool first) {
      if (!tokenizer->next_sentence(&forms, nullptr)) {
        if (output.mode == XML && *unprinted) json.value_xml_escape(unprinted, true);
        if (output.mode == JSON && !first && *unprinted) json.key("spaces").value(unprinted);
        if (output.mode == JSON && !first) json.close().close();
        return false;
      }

      for (unsigned i = 0; i < forms.size(); i++) {
        morpho->analyze(forms[i], guesser, tags);
        converter->convert_analyzed(tags);
        for (auto&& tag : tags) derivation->format_derivation(tag.lemma);
        switch (output.mode) {
          case VERTICAL:
            json.value(sp(forms[i]), true);
            for (auto&& tag : tags)
              json.value("\t", true).value(tag.lemma, true).value("\t", true).value(tag.tag, true);
            json.value("\n", true);
            break;
          case XML:
            if (unprinted < forms[i].str) json.value_xml_escape(sp(unprinted, forms[i].str - unprinted), true);
            if (!i) json.value("<sentence>", true);
            json.value("<token>", true);
            for (auto&& tag : tags)
              json.value("<analysis lemma=\"", true).value_xml_escape(sp(tag.lemma), true)
                    .value("\" tag=\"", true).value_xml_escape(sp(tag.tag), true).value("\"/>", true);
            json.value_xml_escape(sp(forms[i]), true).value("</token>", true);
            break;
          case JSON:
            if ((i || !first) && unprinted < forms[i].str) json.key("space").value(sp(unprinted, forms[i].str - unprinted));
            if (i || !first) json.close();
            if (!i && !first) json.close();
            if (!i) json.array();
            json.object().key("token").value(sp(forms[i])).key("analyses").array();
            for (auto&& tag : tags)
              json.object().key("lemma").value(tag.lemma).key("tag").value(tag.tag).close();
            json.close();
            break;
        }
        unprinted = forms[i].str + forms[i].len;
      }
      if (output.mode == VERTICAL) json.value("\n", true);
      if (output.mode == XML) json.value("</sentence>", true);

      return true;
    }

   private:
    const Morpho* morpho;
    Guesser guesser;
    unique_ptr<Tokenizer> tokenizer;
    unique_ptr<tagset_converter> converter;
    unique_ptr<derivation_formatter> derivation;
    const char* unprinted;
    vector<string_piece> forms;
    vector<tagged_lemma> tags;
  };
  return req.respond(json_mime, new generator(model, data, output, model->get_morpho(), guesser, tokenizer.release(), converter.release(), derivation.release()));
}

bool morphodita_service::handle_rest_generate(microrestd::rest_request& req) {
  string error;
  auto rest_id = get_rest_model_id(req);
  auto model = load_rest_model(rest_id, error);
  if (!model) return req.respond_error(error);
  if (!model->get_morpho()) return req.respond_error(operation_not_supported);

  auto data = get_data(req, error); if (!data) return req.respond_error(error);
  auto guesser = get_guesser(req, error); if (guesser < 0) return req.respond_error(error);
  unique_ptr<tagset_converter> converter(get_convert_tagset(req, *model->get_morpho(), error)); if (!converter) return req.respond_error(error);
  rest_output_mode output(VERTICAL); if (!get_output_mode(req, output, error)) return req.respond_error(error);
  if (output.mode == XML) return req.respond_error("The output 'xml' is not supported for 'generate' method.\n");

  class generator : public rest_response_generator {
   public:
    generator(const model_info* model, const char* data, rest_output_mode output, const Morpho* morpho, Guesser guesser, tagset_converter* converter)
        : rest_response_generator(model, output), data(data), morpho(morpho), guesser(guesser), converter(converter) {}

    bool next(bool /*first*/) {
      string_piece line;
      if (!get_line(data, line)) return false;

      if (output.mode == JSON) json.array();
      if (line.len) {
        string_piece lemma = line;
        const char* wildcard = (const char*) memchr((void*) line.str, '\t', line.len);
        if (wildcard)
          lemma.len = wildcard++ - lemma.str;

        morpho->generate(lemma, wildcard, guesser, forms);
        converter->convert_generated(forms);

        bool first = true;
        for (auto&& lemma : forms)
          for (auto&& form : lemma.forms) {
            switch (output.mode) {
              case JSON:
                json.object().key("form").value(form.form).key("lemma").value(lemma.lemma).key("tag").value(form.tag).close();
                break;
              case VERTICAL:
                if (!first) json.value("\t", true);
                json.value(form.form, true).value("\t", true).value(lemma.lemma, true).value("\t", true).value(form.tag, true);
                break;
              case XML: break; // Not possible, just to keep compiler happy
            }
            first = false;
          }
      }
      if (output.mode == JSON) json.close();
      if (output.mode == VERTICAL) json.value("\n", true);

      return true;
    }

   private:
    const char* data;
    const Morpho* morpho;
    Guesser guesser;
    unique_ptr<tagset_converter> converter;
    vector<tagged_lemma_forms> forms;
  };
  return req.respond(json_mime, new generator(model, data, output, model->get_morpho(), guesser, converter.release()));
}

bool morphodita_service::handle_rest_tokenize(microrestd::rest_request& req) {
  string error;
  auto rest_id = get_rest_model_id(req);
  auto model = load_rest_model(rest_id, error);
  if (!model) return req.respond_error(error);
  if (!model->can_tokenize) return req.respond_error(operation_not_supported);

  auto data = get_data(req, error); if (!data) return req.respond_error(error);
  rest_output_mode output(XML); if (!get_output_mode(req, output, error)) return req.respond_error(error);

  class generator : public rest_response_generator {
   public:
    generator(const model_info* model, const char* data, rest_output_mode output, Tokenizer* tokenizer)
        : rest_response_generator(model, output), tokenizer(tokenizer), unprinted(data) {
      tokenizer->set_text(data);
    }

    bool next(bool first) {
      if (!tokenizer->next_sentence(&forms, nullptr)) {
        if (output.mode == XML && *unprinted) json.value_xml_escape(unprinted, true);
        if (output.mode == JSON && !first && *unprinted) json.key("spaces").value(unprinted);
        if (output.mode == JSON && !first) json.close().close();
        return false;
      }

      for (unsigned i = 0; i < forms.size(); i++) {
        switch (output.mode) {
          case VERTICAL:
            json.value(sp(forms[i]), true).value("\n", true);
            break;
          case XML:
            if (unprinted < forms[i].str) json.value_xml_escape(sp(unprinted, forms[i].str - unprinted), true);
            if (!i) json.value("<sentence>", true);
            json.value("<token>", true).value_xml_escape(sp(forms[i]), true).value("</token>", true);
            break;
          case JSON:
            if ((i || !first) && unprinted < forms[i].str) json.key("space").value(sp(unprinted, forms[i].str - unprinted));
            if (i || !first) json.close();
            if (!i && !first) json.close();
            if (!i) json.array();
            json.object().key("token").value(sp(forms[i]));
            break;
        }
        unprinted = forms[i].str + forms[i].len;
      }
      if (output.mode == VERTICAL) json.value("\n", true);
      if (output.mode == XML) json.value("</sentence>", true);

      return true;
    }

   private:
    unique_ptr<Tokenizer> tokenizer;
    const char* unprinted;
    vector<string_piece> forms;
  };
  return req.respond(json_mime, new generator(model, data, output, model->get_tokenizer()));
}

// REST service helpers

const string& morphodita_service::get_rest_model_id(microrestd::rest_request& req) {
  static string empty;

  auto model_it = req.params.find("model");
  return model_it == req.params.end() ? empty : model_it->second;
}

const char* morphodita_service::get_data(microrestd::rest_request& req, string& error) {
  auto data_it = req.params.find("data");
  if (data_it == req.params.end()) return error.assign("Required argument 'data' is missing.\n"), nullptr;

  return data_it->second.c_str();
}

morpho::guesser_mode morphodita_service::get_guesser(microrestd::rest_request& req, string& error) {
  auto guesser_it = req.params.find("guesser");
  if (guesser_it == req.params.end()) return morpho::GUESSER;
  if (guesser_it->second.compare("yes") == 0) return morpho::GUESSER;
  if (guesser_it->second.compare("no") == 0) return morpho::NO_GUESSER;
  return error.assign("Value '").append(guesser_it->second).append("' of parameter guesser is not either 'yes' or 'no'.\n"), morpho::guesser_mode(-1);
}

tokenizer* morphodita_service::get_tokenizer(microrestd::rest_request& req, const model_info* model, string& error) {
  auto input_it = req.params.find("input");
  if (input_it == req.params.end() || input_it->second.compare("untokenized") == 0) {
    if (!model->can_tokenize) return error.assign("No tokenizer is defined for the requested model.\n"), nullptr;
    return model->get_tokenizer();
  }
  if (input_it->second.compare("vertical") == 0) return tokenizer::new_vertical_tokenizer();
  return error.assign("Value '").append(input_it->second).append("' of parameter input is not either 'untokenized' or 'vertical'.\n"), nullptr;
}

tagset_converter* morphodita_service::get_convert_tagset(microrestd::rest_request& req, const Morpho& morpho, string& error) {
  auto convert_tagset_it = req.params.find("convert_tagset");
  if (convert_tagset_it == req.params.end()) return tagset_converter::new_identity_converter();
  if (convert_tagset_it->second.compare("pdt_to_conll2009") == 0) return tagset_converter::new_pdt_to_conll2009_converter();
  if (convert_tagset_it->second.compare("strip_lemma_comment") == 0) return tagset_converter::new_strip_lemma_comment_converter(morpho);
  if (convert_tagset_it->second.compare("strip_lemma_id") == 0) return tagset_converter::new_strip_lemma_id_converter(morpho);
  return error.assign("Unknown tag set converter '").append(convert_tagset_it->second).append("'.\n"), nullptr;
}

derivation_formatter* morphodita_service::get_derivation_formatter(microrestd::rest_request& req, const Morpho& morpho, string& error) {
  auto derivation_it = req.params.find("derivation");
  if (derivation_it == req.params.end() || derivation_it->second.compare("none") == 0)
    return derivation_formatter::new_none_derivation_formatter();

  if (!morpho.get_derivator())
    return error.assign("The model does not include a morphological derivator, cannot perform requested derivation!\n"), nullptr;

  derivation_formatter* formatter = derivation_formatter::new_derivation_formatter(derivation_it->second, morpho.get_derivator());
  if (!formatter)
    return error.assign("Cannot create requested derivation formatter '").append(derivation_it->second).append("!\n"), nullptr;
  return formatter;
}

bool morphodita_service::get_output_mode(microrestd::rest_request& req, rest_output_mode& output, string& error) {
  auto output_it = req.params.find("output");
  if (output_it != req.params.end() && !rest_output_mode::parse(output_it->second, output))
    return error.assign("Unknown output mode '").append(output_it->second).append("'.\n"), false;
  return true;
}

bool morphodita_service::get_line(const char*& data, string_piece& line) {
  line.str = data;
  while (*data && *data != '\r' && *data != '\n') data++;
  line.len = data - line.str;

  // If end of data was reached
  if (!*data) return line.len;

  // Otherwise, just EOL. Return true and skip line ending (\r \r\n \n \n\r).
  if (*data == '\r') {
    data++;
    if (*data == '\n') data++;
  } else if (*data == '\n') {
    data++;
    if (*data == '\r') data++;
  }
  return true;
}

// WebLicht service
const char* morphodita_service::tcf_mime = "text/tcf+xml";

bool morphodita_service::handle_weblicht_tokenize(microrestd::rest_request& req) {
  string error;
  microrestd::pugi::xml_document tcf;
  if (!parse_tcf(req, tcf, error)) return req.respond_error(error);

  return respond_tcf(req, tcf);
}

bool morphodita_service::parse_tcf(const microrestd::rest_request& req, microrestd::pugi::xml_document& tcf, string& error) {
  if (req.content_type.find(tcf_mime) == string::npos)
    return error.assign("Unsupported content type'").append(req.content_type).append("', expected text/tcf+xml.\n"), false;

  auto result = tcf.load_buffer_inplace((void*) req.body.c_str(), req.body.size());
  if (result.status != microrestd::pugi::status_ok) return error.assign("Cannot parse the TCF: ").append(result.description()).append(".\n"), false;

  return true;
}

bool morphodita_service::respond_tcf(microrestd::rest_request& req, const microrestd::pugi::xml_document& tcf) {
  // Compute size of resulting XML
  struct size_writer : public microrestd::pugi::xml_writer {
    virtual void write(const void*, size_t size) override { this->size += size; }
    size_t get_size() const { return size; }
   private:
    size_t size = 0;
  };
  size_writer tcf_size;
  tcf.save(tcf_size);

  // Allocate the memory using malloc and transfer ownership to the response
  microrestd::string_piece tcf_buffer((const char*) malloc(tcf_size.get_size()), 0);
  if (!tcf_buffer.str) return req.respond_error("Cannot allocate memory for resulting TCF.\n");
  struct string_piece_writer : public microrestd::pugi::xml_writer {
    string_piece_writer(microrestd::string_piece& output) : output(output) { }
    virtual void write(const void* data, size_t size) override { memcpy((void*) (output.str + output.len), data, size); output.len += size; }
   private:
    microrestd::string_piece& output;
  };
  string_piece_writer tcf_buffer_writer(tcf_buffer);
  tcf.save(tcf_buffer_writer);

  return req.respond(tcf_mime, tcf_buffer, false);
}

} // namespace morphodita
} // namespace ufal
