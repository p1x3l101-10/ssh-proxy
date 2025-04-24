#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <iostream>
#include "license.h"
#include "config.hpp"
#include "buildinfo.hpp"

namespace po = boost::program_options;

boost::program_options::variables_map args;

void argProcesser(std::pair<int,char**> arg) {
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "prints this help message")
    ("version", "print the version info")
    ("license", "print the full license info for this program")
    ("config", po::value<std::string>(), "path to config file")
    ("logfile", po::value<std::string>(), "path to log file")
    ("loglevel", po::value<std::string>(), "minimum loglevel to use")
    ("buildinfo", "the config values used to build this binary")
    ("daemon", "run the daemon")
  ;
  try {
    po::store(po::parse_command_line(arg.first, arg.second, desc), args);
  } catch (po::unknown_option &uo) {
    std::cerr << uo.what() << "\n"
         << desc << std::endl;
    exit(1);
  }
  // Fail fast if nothing is called
  if (args.empty()) {
    std::cerr << desc << std::endl;
    exit(1);
  }
  // Arg options before logging
  if (args.count("help")) {
    std::cout << desc << std::endl;
    exit(0);
  }
  if (args.count("version")) {
    std::cout << NAME << " (version: " << VERSION << ")\n"
              << DESCRIPTION << "\n"
              << HOMEPAGE_URL << "\n"
              << "\tCopyright 2025 Scott Blatt, SPDX short identifier: BSD-3-Clause" << std::endl;
    exit(0);
  }
  if (args.count("license")) {
    std::cout << NAME << " (version: " << VERSION << "):\n"
              << LICENSE_TEXT << std::endl;
    exit(0);
  }
  if (args.count("buildinfo")) {
    std::cout << "Build config: ";
    for (const auto& config : ::configInfo) {
      std::cout << "\n\t" << config.first << "=" << config.second;
    }
    std::cout << std::endl;
    exit(0);
  }
}