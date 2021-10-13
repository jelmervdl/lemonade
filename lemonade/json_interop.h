#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "translator/annotation.h"
#include "translator/response.h"
#include "translator/response_options.h"

using namespace marian::bergamot;
using namespace rapidjson;

///  JSONInterOperable
template <class T> void fromJSON(const std::string &json, T &out);
template <class T> std::string toJSON(const T &in);

std::string asString(const Document &document) {
  StringBuffer buffer;
  PrettyWriter<StringBuffer> writer(buffer);
  document.Accept(writer);
  return buffer.GetString();
}

/*
template <>
void fromJSON<Response>(const std::string &json, Response &response) {}
template <> std::string toJSON<Response>(const Response &response) {}

*/

#define INSERT(document, obj, attribute)                                       \
  do {                                                                         \
    document.AddMember(#attribute, obj.attribute, document.GetAllocator());    \
  } while (0)

template <>
void fromJSON<ResponseOptions>(const std::string &json,
                               ResponseOptions &options) {}
template <>
std::string toJSON<ResponseOptions>(const ResponseOptions &options) {
  Document document;
  document.SetObject();
  INSERT(document, options, qualityScores);
  INSERT(document, options, alignment);
  INSERT(document, options, sentenceMappings);
  INSERT(document, options, alignmentThreshold);
  INSERT(document, options, concatStrategy);
  return asString(document);
}

template <> std::string toJSON<Response>(const Response &response) {
  Document document;
  Document::AllocatorType &allocator = document.GetAllocator();
  document.SetObject();

  auto addAnnotatedText = [&allocator](const AnnotatedText &in) {
    Value atext(kObjectType);
    atext.AddMember("text", StringRef(in.text.data(), in.text.size()),
                    allocator);
    return atext;
  };

  document.AddMember("source", addAnnotatedText(response.source), allocator);
  document.AddMember("target", addAnnotatedText(response.target), allocator);

  return asString(document);
}
