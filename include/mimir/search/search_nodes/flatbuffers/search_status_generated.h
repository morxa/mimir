// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_SEARCHSTATUS_H_
#define FLATBUFFERS_GENERATED_SEARCHSTATUS_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 23 &&
              FLATBUFFERS_VERSION_MINOR == 5 &&
              FLATBUFFERS_VERSION_REVISION == 26,
             "Non-compatible flatbuffers version included");

enum SearchStatusFlat : int8_t {
  SearchStatusFlat_NEW = 0,
  SearchStatusFlat_OPEN = 1,
  SearchStatusFlat_CLOSED = 2,
  SearchStatusFlat_DEAD_END = 3,
  SearchStatusFlat_MIN = SearchStatusFlat_NEW,
  SearchStatusFlat_MAX = SearchStatusFlat_DEAD_END
};

inline const SearchStatusFlat (&EnumValuesSearchStatusFlat())[4] {
  static const SearchStatusFlat values[] = {
    SearchStatusFlat_NEW,
    SearchStatusFlat_OPEN,
    SearchStatusFlat_CLOSED,
    SearchStatusFlat_DEAD_END
  };
  return values;
}

inline const char * const *EnumNamesSearchStatusFlat() {
  static const char * const names[5] = {
    "NEW",
    "OPEN",
    "CLOSED",
    "DEAD_END",
    nullptr
  };
  return names;
}

inline const char *EnumNameSearchStatusFlat(SearchStatusFlat e) {
  if (::flatbuffers::IsOutRange(e, SearchStatusFlat_NEW, SearchStatusFlat_DEAD_END)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesSearchStatusFlat()[index];
}

#endif  // FLATBUFFERS_GENERATED_SEARCHSTATUS_H_
