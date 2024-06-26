#include "vdv/xml_in.h"

#include <iostream>

namespace vdv {

/*
msg_envelope parse(std::string const& str) {
  auto doc = pugi::xml_document{};
  auto result = doc.load_string(str.c_str());

  // Error Handling from https://pugixml.org/docs/manual.html#loading
  if (result) {
    std::cout << "XML [" << str << "] parsed without errors, attr value: ["
              << doc.child("node").attribute("attr").value() << "]\n\n";
  } else {
    std::cout << "XML [" << str << "] parsed with errors, attr value: ["
              << doc.child("node").attribute("attr").value() << "]\n";
    std::cout << "Error description: " << result.description() << "\n";
    std::cout << "Error offset: " << result.offset << " (error at [..."
              << (str.c_str() + result.offset) << "]\n\n";
  }
}
*/
}  // namespace vdv