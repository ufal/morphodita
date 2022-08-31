// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <fstream>
#include <sstream>

#include "common.h"
#include "morphodita_service.h"
#include "utils/iostreams.h"
#include "utils/options.h"
#include "utils/parse_int.h"
#include "utils/path_from_utf8.h"
#include "version/version.h"

using namespace ufal::morphodita;

// On Linux define streambuf writing to syslog
#ifdef __linux__
#include <streambuf>
#include <syslog.h>
#include <unistd.h>

class syslog_streambuf : public streambuf {
 public:
  virtual ~syslog_streambuf() {
    syslog_write();
  }

 protected:
  virtual int overflow(int c) override {
    if (c != EOF && c != '\n')
      buffer.push_back(c);
    else
      syslog_write();
    return c;
  }

 private:
  string buffer;

  void syslog_write() {
    if (!buffer.empty()) {
      syslog(LOG_ERR, "%s", buffer.c_str());
      buffer.clear();
    }
  }
};
#endif

microrestd::rest_server server;
morphodita_service service;

int main(int argc, char* argv[]) {
  iostreams_init();

  options::map options;
  if (!options::parse({{"connection_timeout", options::value::any},
                       {"daemon", options::value::none},
                       {"log_file", options::value::any},
                       {"log_request_max_size", options::value::any},
                       {"max_connections", options::value::any},
                       {"max_request_size", options::value::any},
                       {"threads", options::value::any},
                       {"version", options::value::none},
                       {"help", options::value::none}}, argc, argv, options) ||
      options.count("help") ||
      ((argc < 2 || (argc % 3) != 2) && !options.count("version")))
    runtime_failure("Usage: " << argv[0] << " [options] port (model_name model_file acknowledgements)*\n"
                    "Options: --connection_timeout=maximum connection timeout [s] (default 60)\n"
                    "         --daemon (daemonize after start, supported on Linux only)\n"
                    "         --log_file=file path (no logging if empty, default morphodita_server.log)\n"
                    "         --log_request_max_size=max req log size [kB] (0 unlimited, default 64)\n"
                    "         --max_connections=maximum network connections (default 256)\n"
                    "         --max_request_size=maximum request size [kB] (default 1024)\n"
                    "         --threads=threads to use (default 0 means unlimitted)\n"
                    "         --version\n"
                    "         --help");
  if (options.count("version")) {
    ostringstream other_libraries;
    auto microrestd = microrestd::version::current();
    other_libraries << "MicroRestD " << microrestd.major << '.' << microrestd.minor << '.' << microrestd.patch;
    return cout << version::version_and_copyright(other_libraries.str()) << endl, 0;
  }

  // Process options
  int port = parse_int(argv[1], "port number");
  int connection_timeout = options.count("connection_timeout") ? parse_int(options["connection_timeout"], "connection timeout") : 60;
  int log_request_max_size = options.count("log_request_max_size") ? parse_int(options["log_request_max_size"], "log request maximum size") : 64;
  int max_connections = options.count("max_connections") ? parse_int(options["max_connections"], "maximum connections") : 256;
  int max_request_size = options.count("max_request_size") ? parse_int(options["max_request_size"], "maximum request size") : 1024;
  int threads = options.count("threads") ? parse_int(options["threads"], "number of threads") : 0;

#ifndef __linux__
  if (options.count("daemon")) runtime_failure("The --daemon option is currently supported on Linux only!");
#endif

  // Initialize the service
  vector<morphodita_service::model_description> models;
  for (int i = 2; i < argc; i += 3)
    models.emplace_back(argv[i], argv[i + 1], argv[i + 2]);

  if (!service.init(models))
    runtime_failure("Cannot load specified models!");

  // Open log file
  ofstream log_file;
  string log_file_name = options.count("log_file") ? options["log_file"] : string(argv[0]) + ".log";
  if (!log_file_name.empty()) {
    log_file.open(path_from_utf8(log_file_name).c_str(), ofstream::app);
    if (!log_file) runtime_failure("Cannot open log file '" << log_file_name << "' for writing!");
  }

  // Daemonize if requested
#ifdef __linux__
  if (options.count("daemon")) {
    if (daemon(1, 0) != 0)
      runtime_failure("Cannot daemonize in '" << argv[0] << "' executable!");

    // Redirect cerr to syslog
    openlog("morphodita_server", 0, LOG_USER);
    static syslog_streambuf syslog;
    cerr.rdbuf(&syslog);
  }
#endif

  // Start the server
  if (!log_file_name.empty())
    server.set_log_file(&log_file, log_request_max_size << 10);
  server.set_max_connections(max_connections);
  server.set_max_request_body_size(max_request_size << 10);
  server.set_min_generated(32 << 10);
  server.set_threads(threads);
  server.set_timeout(connection_timeout);

  if (!server.start(&service, port))
    runtime_failure("Cannot start morphodita_server'!");

  cerr << "Successfully started morphodita_server on port " << port << "." << endl;

  // Wait until finished
  server.wait_until_signalled();
  server.stop();

  return 0;
}
