#pragma once
#include "data.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "translator/annotation.h"
#include "translator/response.h"
#include "translator/response_options.h"

namespace lemonade {

#define LEMONADE_INLINE inline

///  JSONInterOperable
template <class T> void fromJSON(const std::string &json, T &out);
template <class T> std::string toJSON(const T &in);

LEMONADE_INLINE std::string asString(const rapidjson::Document &document) {
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  document.Accept(writer);
  return buffer.GetString();
}

#define LEMONADE_VALUE_INSERT(document, obj, attribute)                        \
  do {                                                                         \
    document.AddMember(#attribute, obj.attribute, document.GetAllocator());    \
  } while (0)

template <>
LEMONADE_INLINE void fromJSON<marian::bergamot::ResponseOptions>(
    const std::string &json, marian::bergamot::ResponseOptions &options) {}
template <>
LEMONADE_INLINE std::string toJSON<marian::bergamot::ResponseOptions>(
    const marian::bergamot::ResponseOptions &options) {
  rapidjson::Document document;
  document.SetObject();
  LEMONADE_VALUE_INSERT(document, options, qualityScores);
  LEMONADE_VALUE_INSERT(document, options, alignment);
  LEMONADE_VALUE_INSERT(document, options, sentenceMappings);
  LEMONADE_VALUE_INSERT(document, options, alignmentThreshold);
  LEMONADE_VALUE_INSERT(document, options, concatStrategy);
  return asString(document);
}

template <>
LEMONADE_INLINE std::string
toJSON<marian::bergamot::Response>(const marian::bergamot::Response &response) {
  rapidjson::Document document;
  rapidjson::Document::AllocatorType &allocator = document.GetAllocator();
  document.SetObject();

  auto addAnnotatedText =
      [&allocator](const marian::bergamot::AnnotatedText &in) {
        rapidjson::Value atext(rapidjson::kObjectType);
        atext.AddMember("text",
                        rapidjson::StringRef(in.text.data(), in.text.size()),
                        allocator);
        return atext;
      };

  document.AddMember("source", addAnnotatedText(response.source), allocator);
  document.AddMember("target", addAnnotatedText(response.target), allocator);

  return asString(document);
}

template <>
LEMONADE_INLINE std::string toJSON<Payload>(const Payload &payload) {
  rapidjson::Document document;
  rapidjson::Document::AllocatorType &allocator = document.GetAllocator();
  document.SetObject();

  document.AddMember(
      "source",
      rapidjson::StringRef(payload.source.data(), payload.source.size()),
      allocator);

  document.AddMember(
      "target",
      rapidjson::StringRef(payload.target.data(), payload.target.size()),
      allocator);

  document.AddMember(
      "query", rapidjson::StringRef(payload.query.data(), payload.query.size()),
      allocator);

  return asString(document);
}

template <>
LEMONADE_INLINE void fromJSON<Payload>(const std::string &json,
                                       Payload &payload) {
  rapidjson::Document document;
  document.Parse(json.c_str());
  payload.source = document["source"].GetString();
  payload.target = document["target"].GetString();
  payload.query = document["query"].GetString();
}

#undef LEMONADE_INLINE

} // namespace lemonade
