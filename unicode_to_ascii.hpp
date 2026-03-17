#ifndef UNICODE_TO_ASCII_HPP
#define UNICODE_TO_ASCII_HPP
#include <string>
std::string ascii_only_filter(const std::string &str);
std::string utf8_to_ascii(const std::string &str);
#endif
