// Copyright 2010-2016, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "base/util.h"

#include <climits>
#include <cstdlib>
#include <cstring>
#include <map>
#include <sstream>
#include <string>

#include "base/file_stream.h"
#include "base/file_util.h"
#include "base/logging.h"
#include "base/number_util.h"
#include "gtest/gtest.h"

namespace gbase {
namespace {

Util::CharacterSet GetExpectedCharacterSet(
    const std::map<char32, Util::CharacterSet> &test_map,
    char32 ucs4) {
  std::map<char32, Util::CharacterSet>::const_iterator itr =
      test_map.find(ucs4);
  if (test_map.find(ucs4) == test_map.end()) {
    // If the test data does not have an entry, it should be
    // interpreted as |Util::UNICODE_ONLY|.
    return Util::UNICODE_ONLY;
  }
  return itr->second;
}

}  // namespace

TEST(UtilTest, JoinStrings) {
  std::vector<string> input;
  input.push_back("ab");
  input.push_back("cdef");
  input.push_back("ghr");
  string output;
  Util::JoinStrings(input, ":", &output);
  EXPECT_EQ("ab:cdef:ghr", output);
}

TEST(UtilTest, JoinStringPieces) {
  {
    std::vector<StringPiece> input;
    input.push_back("ab");
    string output;
    Util::JoinStringPieces(input, ":", &output);
    EXPECT_EQ("ab", output);
  }
  {
    std::vector<StringPiece> input;
    input.push_back("ab");
    input.push_back("cdef");
    input.push_back("ghr");
    string output;
    Util::JoinStringPieces(input, ":", &output);
    EXPECT_EQ("ab:cdef:ghr", output);
  }
  {
    std::vector<StringPiece> input;
    input.push_back("ab");
    input.push_back("cdef");
    input.push_back("ghr");
    string output;
    Util::JoinStringPieces(input, "::", &output);
    EXPECT_EQ("ab::cdef::ghr", output);
  }
}

TEST(UtilTest, ConcatStrings) {
  string s;

  Util::ConcatStrings("", "", &s);
  EXPECT_TRUE(s.empty());

  Util::ConcatStrings("ABC", "", &s);
  EXPECT_EQ("ABC", s);

  Util::ConcatStrings("", "DEF", &s);
  EXPECT_EQ("DEF", s);

  Util::ConcatStrings("ABC", "DEF", &s);
  EXPECT_EQ("ABCDEF", s);
}

TEST(UtilTest, AppendStringWithDelimiter) {
  string result;
  string input;
  const char kDelemiter[] = ":";

  {
    result.clear();
    Util::AppendStringWithDelimiter(kDelemiter, "test", &result);
    EXPECT_EQ("test", result);
  }

  {
    result = "foo";
    Util::AppendStringWithDelimiter(kDelemiter, "test", &result);
    EXPECT_EQ("foo:test", result);
  }

  {
    result = "foo";
    Util::AppendStringWithDelimiter(kDelemiter, "", &result);
    EXPECT_EQ("foo:", result);
  }
}

TEST(UtilTest, SplitIterator_SingleDelimiter_SkipEmpty) {
  typedef SplitIterator<SingleDelimiter, SkipEmpty> SplitIterator;
  {
    SplitIterator iter("", " ");
    EXPECT_TRUE(iter.Done());
  }
  {
    SplitIterator iter(StringPiece(), " ");
    EXPECT_TRUE(iter.Done());
  }
  {
    const char *s = "a b cde";
    SplitIterator iter(s, " ");
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("a", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("b", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("cde", iter.Get());
    iter.Next();
    EXPECT_TRUE(iter.Done());
  }
  {
    const char *s = " a b  cde ";
    SplitIterator iter(s, " ");
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("a", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("b", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("cde", iter.Get());
    iter.Next();
    EXPECT_TRUE(iter.Done());
  }
  {
    StringPiece s("a b  cde ", 5);  // s = "a b  ";
    SplitIterator iter(s, " ");
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("a", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("b", iter.Get());
    iter.Next();
    EXPECT_TRUE(iter.Done());
  }
}

TEST(UtilTest, SplitIterator_MultiDelimiter_SkipEmpty) {
  typedef SplitIterator<MultiDelimiter, SkipEmpty> SplitIterator;
  {
    SplitIterator iter("", " \t,");
    EXPECT_TRUE(iter.Done());
  }
  {
    SplitIterator iter(StringPiece(), ",.");
    EXPECT_TRUE(iter.Done());
  }
  {
    const char *s = "a b\tcde:fg";
    SplitIterator iter(s, " \t:");
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("a", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("b", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("cde", iter.Get());
    EXPECT_FALSE(iter.Done());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("fg", iter.Get());
    iter.Next();
    EXPECT_TRUE(iter.Done());
  }
  {
    const char *s = "  \t:a b\t\tcde:fg:";
    SplitIterator iter(s, " \t:");
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("a", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("b", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("cde", iter.Get());
    EXPECT_FALSE(iter.Done());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("fg", iter.Get());
    iter.Next();
    EXPECT_TRUE(iter.Done());
  }
}

TEST(UtilTest, SplitIterator_SingleDelimiter_AllowEmpty) {
  typedef SplitIterator<SingleDelimiter, AllowEmpty> SplitIterator;
  {
    SplitIterator iter("", " ");
    EXPECT_TRUE(iter.Done());
  }
  {
    SplitIterator iter(StringPiece(), " ");
    EXPECT_TRUE(iter.Done());
  }
  {
    const char *s = "a b cde";
    SplitIterator iter(s, " ");
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("a", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("b", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("cde", iter.Get());
    iter.Next();
    EXPECT_TRUE(iter.Done());
  }
  {
    const char *s = " a b  cde ";
    SplitIterator iter(s, " ");
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("a", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("b", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("cde", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("", iter.Get());
    iter.Next();
    EXPECT_TRUE(iter.Done());
  }
  {
    StringPiece s("a b  cde ", 5);  // s = "a b  ";
    SplitIterator iter(s, " ");
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("a", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("b", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("", iter.Get());
    iter.Next();
    EXPECT_TRUE(iter.Done());
  }
}

TEST(UtilTest, SplitIterator_MultiDelimiter_AllowEmpty) {
  typedef SplitIterator<MultiDelimiter, AllowEmpty> SplitIterator;
  {
    SplitIterator iter("", " \t,");
    EXPECT_TRUE(iter.Done());
  }
  {
    SplitIterator iter(StringPiece(), ",.");
    EXPECT_TRUE(iter.Done());
  }
  {
    const char *s = "a b\tcde:fg";
    SplitIterator iter(s, " \t:");
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("a", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("b", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("cde", iter.Get());
    EXPECT_FALSE(iter.Done());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("fg", iter.Get());
    iter.Next();
    EXPECT_TRUE(iter.Done());
  }
  {
    const char *s = "a b\t\tcde:fg:";
    SplitIterator iter(s, " \t:");
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("a", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("b", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("cde", iter.Get());
    EXPECT_FALSE(iter.Done());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("fg", iter.Get());
    iter.Next();
    EXPECT_FALSE(iter.Done());
    EXPECT_EQ("", iter.Get());
    iter.Next();
    EXPECT_TRUE(iter.Done());
  }
}

TEST(UtilTest, SplitStringUsing) {
  {
    const string input = "a b  c def";
    std::vector<string> output;
    Util::SplitStringUsing(input, " ", &output);
    EXPECT_EQ(output.size(), 4);
    EXPECT_EQ("a", output[0]);
    EXPECT_EQ("b", output[1]);
    EXPECT_EQ("c", output[2]);
    EXPECT_EQ("def", output[3]);
  }
  {
    const string input = " a b  c";
    std::vector<string> output;
    Util::SplitStringUsing(input, " ", &output);
    EXPECT_EQ(output.size(), 3);
    EXPECT_EQ("a", output[0]);
    EXPECT_EQ("b", output[1]);
    EXPECT_EQ("c", output[2]);
  }
  {
    const string input = "a b  c ";
    std::vector<string> output;
    Util::SplitStringUsing(input, " ", &output);
    EXPECT_EQ(output.size(), 3);
    EXPECT_EQ("a", output[0]);
    EXPECT_EQ("b", output[1]);
    EXPECT_EQ("c", output[2]);
  }
  {
    const string input = "a:b  cd ";
    std::vector<string> output;
    Util::SplitStringUsing(input, ": ", &output);
    EXPECT_EQ(output.size(), 3);
    EXPECT_EQ("a", output[0]);
    EXPECT_EQ("b", output[1]);
    EXPECT_EQ("cd", output[2]);
  }
  {
    const string input = "Empty delimiter";
    std::vector<string> output;
    Util::SplitStringUsing(input, "", &output);
    EXPECT_EQ(output.size(), 1);
    EXPECT_EQ(input, output[0]);
  }
}

TEST(UtilTest, SplitStringAllowEmpty) {
  {
    const string input = "a b  c def";
    std::vector<string> output;
    Util::SplitStringAllowEmpty(input, " ", &output);
    EXPECT_EQ(output.size(), 5);
    EXPECT_EQ("a", output[0]);
    EXPECT_EQ("b", output[1]);
    EXPECT_EQ("", output[2]);
    EXPECT_EQ("c", output[3]);
    EXPECT_EQ("def", output[4]);
  }
  {
    const string input = " a b  c";
    std::vector<string> output;
    Util::SplitStringAllowEmpty(input, " ", &output);
    EXPECT_EQ(output.size(), 5);
    EXPECT_EQ("", output[0]);
    EXPECT_EQ("a", output[1]);
    EXPECT_EQ("b", output[2]);
    EXPECT_EQ("", output[3]);
    EXPECT_EQ("c", output[4]);
  }
  {
    const string input = "a b  c ";
    std::vector<string> output;
    Util::SplitStringAllowEmpty(input, " ", &output);
    EXPECT_EQ(output.size(), 5);
    EXPECT_EQ("a", output[0]);
    EXPECT_EQ("b", output[1]);
    EXPECT_EQ("", output[2]);
    EXPECT_EQ("c", output[3]);
    EXPECT_EQ("", output[4]);
  }
  {
    const string input = "a:b  c ";
    std::vector<string> output;
    Util::SplitStringAllowEmpty(input, ": ", &output);
    EXPECT_EQ(output.size(), 5);
    EXPECT_EQ("a", output[0]);
    EXPECT_EQ("b", output[1]);
    EXPECT_EQ("", output[2]);
    EXPECT_EQ("c", output[3]);
    EXPECT_EQ("", output[4]);
  }
  {
    const string input = "Empty delimiter";
    std::vector<string> output;
    Util::SplitStringAllowEmpty(input, "", &output);
    EXPECT_EQ(output.size(), 1);
    EXPECT_EQ(input, output[0]);
  }
}

TEST(UtilTest, StripWhiteSpaces) {
  // basic scenario.
  {
    const string input = "  foo   ";
    string output;
    Util::StripWhiteSpaces(input, &output);
    EXPECT_EQ("foo", output);
  }

  // no space means just copy.
  {
    const string input = "foo";
    string output;
    Util::StripWhiteSpaces(input, &output);
    EXPECT_EQ("foo", output);
  }

  // tabs and linebreaks are also spaces.
  {
    const string input = " \tfoo\n";
    string output;
    Util::StripWhiteSpaces(input, &output);
    EXPECT_EQ("foo", output);
  }

  // spaces in the middle remains.
  {
    const string input = " foo bar baz ";
    string output;
    Util::StripWhiteSpaces(input, &output);
    EXPECT_EQ("foo bar baz", output);
  }

  // all spaces means clear out output.
  {
    const string input = " \v \r ";
    string output;
    Util::StripWhiteSpaces(input, &output);
    EXPECT_TRUE(output.empty());
  }

  // empty input.
  {
    const string input = "";
    string output;
    Util::StripWhiteSpaces(input, &output);
    EXPECT_TRUE(output.empty());
  }

  // one character.
  {
    const string input = "a";
    string output;
    Util::StripWhiteSpaces(input, &output);
    EXPECT_EQ("a", output);
  }
}

TEST(UtilTest, SplitStringToUtf8Chars) {
  {
    std::vector<string> output;
    Util::SplitStringToUtf8Chars("", &output);
    EXPECT_EQ(0, output.size());
  }

  {
    // "a" "あ" "Ａ" "亜" "\n" "a"
    const string kInputs[] = {
      "a",
      "\xE3\x81\x82",
      "\xEF\xBC\xA1",
      "\xE4\xBA\x9C",
      "\n",
      "a",
    };
    string joined_string;
    for (int i = 0; i < arraysize(kInputs); ++i) {
      joined_string += kInputs[i];
    }

    std::vector<string> output;
    Util::SplitStringToUtf8Chars(joined_string, &output);
    EXPECT_EQ(arraysize(kInputs), output.size());

    for (size_t i = 0; i < output.size(); ++i) {
      EXPECT_EQ(kInputs[i], output[i]);
    }
  }
}

TEST(UtilTest, SplitCSV) {
  std::vector<string> answer_vector;

  Util::SplitCSV(
      "Google,x,\"Buchheit, Paul\",\"string with \"\" quote in it\"",
      &answer_vector);
  CHECK_EQ(answer_vector.size(), 4);
  CHECK_EQ(answer_vector[0], "Google");
  CHECK_EQ(answer_vector[1], "x");
  CHECK_EQ(answer_vector[2], "Buchheit, Paul");
  CHECK_EQ(answer_vector[3], "string with \" quote in it");

  Util::SplitCSV("Google,hello,",  &answer_vector);
  CHECK_EQ(answer_vector.size(), 3);
  CHECK_EQ(answer_vector[0], "Google");
  CHECK_EQ(answer_vector[1], "hello");
  CHECK_EQ(answer_vector[2], "");

  Util::SplitCSV("Google rocks,hello", &answer_vector);
  CHECK_EQ(answer_vector.size(), 2);
  CHECK_EQ(answer_vector[0], "Google rocks");
  CHECK_EQ(answer_vector[1], "hello");

  Util::SplitCSV(",,\"\",,", &answer_vector);
  CHECK_EQ(answer_vector.size(), 5);
  CHECK_EQ(answer_vector[0], "");
  CHECK_EQ(answer_vector[1], "");
  CHECK_EQ(answer_vector[2], "");
  CHECK_EQ(answer_vector[3], "");
  CHECK_EQ(answer_vector[4], "");

  // Test a string containing a comma.
  Util::SplitCSV("\",\",hello", &answer_vector);
  CHECK_EQ(answer_vector.size(), 2);
  CHECK_EQ(answer_vector[0], ",");
  CHECK_EQ(answer_vector[1], "hello");

  // Invalid CSV
  Util::SplitCSV("\"no,last,quote", &answer_vector);
  CHECK_EQ(answer_vector.size(), 1);
  CHECK_EQ(answer_vector[0], "no,last,quote");

  Util::SplitCSV("backslash\\,is,no,an,\"escape\"", &answer_vector);
  CHECK_EQ(answer_vector.size(), 5);
  CHECK_EQ(answer_vector[0], "backslash\\");
  CHECK_EQ(answer_vector[1], "is");
  CHECK_EQ(answer_vector[2], "no");
  CHECK_EQ(answer_vector[3], "an");
  CHECK_EQ(answer_vector[4], "escape");

  Util::SplitCSV("", &answer_vector);
  CHECK_EQ(answer_vector.size(), 0);
}

TEST(UtilTest, ReplaceString) {
  const string input = "foobarfoobar";
  string output;
  Util::StringReplace(input, "bar", "buz", true, &output);
  EXPECT_EQ("foobuzfoobuz", output);

  output.clear();
  Util::StringReplace(input, "bar", "buz", false, &output);
  EXPECT_EQ("foobuzfoobar", output);
}

TEST(UtilTest, LowerString) {
  string s = "TeSTtest";
  Util::LowerString(&s);
  EXPECT_EQ("testtest", s);

  // "ＴｅＳＴ＠ＡＢＣＸＹＺ［｀ａｂｃｘｙｚ｛"
  string s2 = "\xef\xbc\xb4\xef\xbd\x85\xef\xbc\xb3\xef\xbc\xb4\xef\xbc\xa0\xef"
              "\xbc\xa1\xef\xbc\xa2\xef\xbc\xa3\xef\xbc\xb8\xef\xbc\xb9\xef\xbc"
              "\xba\xef\xbc\xbb\xef\xbd\x80\xef\xbd\x81\xef\xbd\x82\xef\xbd\x83"
              "\xef\xbd\x98\xef\xbd\x99\xef\xbd\x9a\xef\xbd\x9b";
  Util::LowerString(&s2);
  // "ｔｅｓｔ＠ａｂｃｘｙｚ［｀ａｂｃｘｙｚ｛"
  EXPECT_EQ("\xef\xbd\x94\xef\xbd\x85\xef\xbd\x93\xef\xbd\x94\xef\xbc\xa0\xef"
            "\xbd\x81\xef\xbd\x82\xef\xbd\x83\xef\xbd\x98\xef\xbd\x99\xef\xbd"
            "\x9a\xef\xbc\xbb\xef\xbd\x80\xef\xbd\x81\xef\xbd\x82\xef\xbd\x83"
            "\xef\xbd\x98\xef\xbd\x99\xef\xbd\x9a\xef\xbd\x9b", s2);
}

TEST(UtilTest, UpperString) {
  string s = "TeSTtest";
  Util::UpperString(&s);
  EXPECT_EQ("TESTTEST", s);

  // "ＴｅＳＴ＠ＡＢＣＸＹＺ［｀ａｂｃｘｙｚ｛"
  string s2 = "\xef\xbc\xb4\xef\xbd\x85\xef\xbc\xb3\xef\xbc\xb4\xef\xbc\xa0\xef"
              "\xbc\xa1\xef\xbc\xa2\xef\xbc\xa3\xef\xbc\xb8\xef\xbc\xb9\xef\xbc"
              "\xba\xef\xbc\xbb\xef\xbd\x80\xef\xbd\x81\xef\xbd\x82\xef\xbd\x83"
              "\xef\xbd\x98\xef\xbd\x99\xef\xbd\x9a\xef\xbd\x9b";
  Util::UpperString(&s2);
  // "ＴＥＳＴ＠ＡＢＣＸＹＺ［｀ＡＢＣＸＹＺ｛"
  EXPECT_EQ("\xef\xbc\xb4\xef\xbc\xa5\xef\xbc\xb3\xef\xbc\xb4\xef\xbc\xa0\xef"
            "\xbc\xa1\xef\xbc\xa2\xef\xbc\xa3\xef\xbc\xb8\xef\xbc\xb9\xef\xbc"
            "\xba\xef\xbc\xbb\xef\xbd\x80\xef\xbc\xa1\xef\xbc\xa2\xef\xbc\xa3"
            "\xef\xbc\xb8\xef\xbc\xb9\xef\xbc\xba\xef\xbd\x9b", s2);
}

TEST(UtilTest, CapitalizeString) {
  string s = "TeSTtest";
  Util::CapitalizeString(&s);
  EXPECT_EQ("Testtest", s);

  // "ＴｅＳＴ＠ＡＢＣＸＹＺ［｀ａｂｃｘｙｚ｛"
  string s2 = "\xef\xbc\xb4\xef\xbd\x85\xef\xbc\xb3\xef\xbc\xb4\xef\xbc\xa0\xef"
              "\xbc\xa1\xef\xbc\xa2\xef\xbc\xa3\xef\xbc\xb8\xef\xbc\xb9\xef\xbc"
              "\xba\xef\xbc\xbb\xef\xbd\x80\xef\xbd\x81\xef\xbd\x82\xef\xbd\x83"
              "\xef\xbd\x98\xef\xbd\x99\xef\xbd\x9a\xef\xbd\x9b";
  Util::CapitalizeString(&s2);
  // "Ｔｅｓｔ＠ａｂｃｘｙｚ［｀ａｂｃｘｙｚ｛"
  EXPECT_EQ("\xef\xbc\xb4\xef\xbd\x85\xef\xbd\x93\xef\xbd\x94\xef\xbc\xa0\xef"
            "\xbd\x81\xef\xbd\x82\xef\xbd\x83\xef\xbd\x98\xef\xbd\x99\xef\xbd"
            "\x9a\xef\xbc\xbb\xef\xbd\x80\xef\xbd\x81\xef\xbd\x82\xef\xbd\x83"
            "\xef\xbd\x98\xef\xbd\x99\xef\xbd\x9a\xef\xbd\x9b", s2);
}

TEST(UtilTest, IsLowerAscii) {
  EXPECT_TRUE(Util::IsLowerAscii(""));
  EXPECT_TRUE(Util::IsLowerAscii("hello"));
  EXPECT_FALSE(Util::IsLowerAscii("HELLO"));
  EXPECT_FALSE(Util::IsLowerAscii("Hello"));
  EXPECT_FALSE(Util::IsLowerAscii("HeLlO"));
  EXPECT_FALSE(Util::IsLowerAscii("symbol!"));
  EXPECT_FALSE(Util::IsLowerAscii(  // "Ｈｅｌｌｏ"
      "\xEF\xBC\xA8\xEF\xBD\x85\xEF\xBD\x8C\xEF\xBD\x8C\xEF\xBD\x8F"));
}

TEST(UtilTest, IsUpperAscii) {
  EXPECT_TRUE(Util::IsUpperAscii(""));
  EXPECT_FALSE(Util::IsUpperAscii("hello"));
  EXPECT_TRUE(Util::IsUpperAscii("HELLO"));
  EXPECT_FALSE(Util::IsUpperAscii("Hello"));
  EXPECT_FALSE(Util::IsUpperAscii("HeLlO"));
  EXPECT_FALSE(Util::IsUpperAscii("symbol!"));
  EXPECT_FALSE(Util::IsUpperAscii(  // "Ｈｅｌｌｏ"
      "\xEF\xBC\xA8\xEF\xBD\x85\xEF\xBD\x8C\xEF\xBD\x8C\xEF\xBD\x8F"));
}

TEST(UtilTest, IsCapitalizedAscii) {
  EXPECT_TRUE(Util::IsCapitalizedAscii(""));
  EXPECT_FALSE(Util::IsCapitalizedAscii("hello"));
  EXPECT_FALSE(Util::IsCapitalizedAscii("HELLO"));
  EXPECT_TRUE(Util::IsCapitalizedAscii("Hello"));
  EXPECT_FALSE(Util::IsCapitalizedAscii("HeLlO"));
  EXPECT_FALSE(Util::IsCapitalizedAscii("symbol!"));
  EXPECT_FALSE(Util::IsCapitalizedAscii(  // "Ｈｅｌｌｏ"
      "\xEF\xBC\xA8\xEF\xBD\x85\xEF\xBD\x8C\xEF\xBD\x8C\xEF\xBD\x8F"));
}

TEST(UtilTest, IsLowerOrUpperAscii) {
  EXPECT_TRUE(Util::IsLowerOrUpperAscii(""));
  EXPECT_TRUE(Util::IsLowerOrUpperAscii("hello"));
  EXPECT_TRUE(Util::IsLowerOrUpperAscii("HELLO"));
  EXPECT_FALSE(Util::IsLowerOrUpperAscii("Hello"));
  EXPECT_FALSE(Util::IsLowerOrUpperAscii("HeLlO"));
  EXPECT_FALSE(Util::IsLowerOrUpperAscii("symbol!"));
  EXPECT_FALSE(Util::IsLowerOrUpperAscii(  // "Ｈｅｌｌｏ"
      "\xEF\xBC\xA8\xEF\xBD\x85\xEF\xBD\x8C\xEF\xBD\x8C\xEF\xBD\x8F"));
}

TEST(UtilTest, IsUpperOrCapitalizedAscii) {
  EXPECT_TRUE(Util::IsUpperOrCapitalizedAscii(""));
  EXPECT_FALSE(Util::IsUpperOrCapitalizedAscii("hello"));
  EXPECT_TRUE(Util::IsUpperOrCapitalizedAscii("HELLO"));
  EXPECT_TRUE(Util::IsUpperOrCapitalizedAscii("Hello"));
  EXPECT_FALSE(Util::IsUpperOrCapitalizedAscii("HeLlO"));
  EXPECT_FALSE(Util::IsUpperOrCapitalizedAscii("symbol!"));
  EXPECT_FALSE(Util::IsUpperOrCapitalizedAscii(  // "Ｈｅｌｌｏ"
      "\xEF\xBC\xA8\xEF\xBD\x85\xEF\xBD\x8C\xEF\xBD\x8C\xEF\xBD\x8F"));
}

void VerifyUTF8ToUCS4(const string &text, char32 expected_ucs4,
                      size_t expected_len) {
  const char *begin = text.data();
  const char *end = begin + text.size();
  size_t mblen = 0;
  char32 result = Util::UTF8ToUCS4(begin, end, &mblen);
  EXPECT_EQ(expected_ucs4, result) << text << " " << expected_ucs4;
  EXPECT_EQ(expected_len, mblen) << text << " " << expected_len;
}

TEST(UtilTest, UTF8ToUCS4) {
  VerifyUTF8ToUCS4("", 0, 0);
  VerifyUTF8ToUCS4("\x01", 1, 1);
  VerifyUTF8ToUCS4("\x7F", 0x7F, 1);
  VerifyUTF8ToUCS4("\xC2\x80", 0x80, 2);
  VerifyUTF8ToUCS4("\xDF\xBF", 0x7FF, 2);
  VerifyUTF8ToUCS4("\xE0\xA0\x80", 0x800, 3);
  VerifyUTF8ToUCS4("\xEF\xBF\xBF", 0xFFFF, 3);
  VerifyUTF8ToUCS4("\xF0\x90\x80\x80", 0x10000, 4);
  VerifyUTF8ToUCS4("\xF7\xBF\xBF\xBF", 0x1FFFFF, 4);
  // do not test 5-6 bytes because it's out of spec of UTF8.
}

TEST(UtilTest, UCS4ToUTF8) {
  string output;

  // Do nothing if |c| is NUL. Previous implementation of UCS4ToUTF8 worked like
  // this even though the reason is unclear.
  Util::UCS4ToUTF8(0, &output);
  EXPECT_TRUE(output.empty());

  Util::UCS4ToUTF8(0x7F, &output);
  EXPECT_EQ("\x7F", output);
  Util::UCS4ToUTF8(0x80, &output);
  EXPECT_EQ("\xC2\x80", output);
  Util::UCS4ToUTF8(0x7FF, &output);
  EXPECT_EQ("\xDF\xBF", output);
  Util::UCS4ToUTF8(0x800, &output);
  EXPECT_EQ("\xE0\xA0\x80", output);
  Util::UCS4ToUTF8(0xFFFF, &output);
  EXPECT_EQ("\xEF\xBF\xBF", output);
  Util::UCS4ToUTF8(0x10000, &output);
  EXPECT_EQ("\xF0\x90\x80\x80", output);
  Util::UCS4ToUTF8(0x1FFFFF, &output);
  EXPECT_EQ("\xF7\xBF\xBF\xBF", output);

  // Buffer version.
  char buf[7];

  EXPECT_EQ(0, Util::UCS4ToUTF8(0, buf));
  EXPECT_EQ(0, strcmp(buf, ""));

  EXPECT_EQ(1, Util::UCS4ToUTF8(0x7F, buf));
  EXPECT_EQ(0, strcmp("\x7F", buf));

  EXPECT_EQ(2, Util::UCS4ToUTF8(0x80, buf));
  EXPECT_EQ(0, strcmp("\xC2\x80", buf));

  EXPECT_EQ(2, Util::UCS4ToUTF8(0x7FF, buf));
  EXPECT_EQ(0, strcmp("\xDF\xBF", buf));

  EXPECT_EQ(3, Util::UCS4ToUTF8(0x800, buf));
  EXPECT_EQ(0, strcmp("\xE0\xA0\x80", buf));

  EXPECT_EQ(3, Util::UCS4ToUTF8(0xFFFF, buf));
  EXPECT_EQ(0, strcmp("\xEF\xBF\xBF", buf));

  EXPECT_EQ(4, Util::UCS4ToUTF8(0x10000, buf));
  EXPECT_EQ(0, strcmp("\xF0\x90\x80\x80", buf));

  EXPECT_EQ(4, Util::UCS4ToUTF8(0x1FFFFF, buf));
  EXPECT_EQ(0, strcmp("\xF7\xBF\xBF\xBF", buf));
}

TEST(UtilTest, CharsLen) {
  // "私の名前は中野です"
  const string src = "\xe7\xa7\x81\xe3\x81\xae\xe5\x90\x8d\xe5\x89\x8d\xe3\x81"
                     "\xaf\xe4\xb8\xad\xe9\x87\x8e\xe3\x81\xa7\xe3\x81\x99";
  EXPECT_EQ(Util::CharsLen(src.c_str(), src.size()), 9);
}

TEST(UtilTest, SubStringPiece) {
  // "私の名前は中野です"
  const string src = "\xe7\xa7\x81\xe3\x81\xae\xe5\x90\x8d\xe5\x89\x8d\xe3\x81"
                     "\xaf\xe4\xb8\xad\xe9\x87\x8e\xe3\x81\xa7\xe3\x81\x99";
  StringPiece result;

  result = Util::SubStringPiece(src, 0, 2);
  // "私の"
  EXPECT_EQ("\xe7\xa7\x81\xe3\x81\xae", result);
  // |result|'s data should point to the same memory block as src.
  EXPECT_LE(src.data(), result.data());

  result = Util::SubStringPiece(src, 4, 1);
  // "は"
  EXPECT_EQ("\xe3\x81\xaf", result);
  EXPECT_LE(src.data(), result.data());

  result = Util::SubStringPiece(src, 5, 3);
  // "中野で"
  EXPECT_EQ("\xe4\xb8\xad\xe9\x87\x8e\xe3\x81\xa7", result);
  EXPECT_LE(src.data(), result.data());

  result = Util::SubStringPiece(src, 6, 10);
  // "野です"
  EXPECT_EQ("\xe9\x87\x8e\xe3\x81\xa7\xe3\x81\x99", result);
  EXPECT_LE(src.data(), result.data());

  result = Util::SubStringPiece(src, 4, 2);
  // "は中"
  EXPECT_EQ("\xe3\x81\xaf\xe4\xb8\xad", result);
  EXPECT_LE(src.data(), result.data());

  result = Util::SubStringPiece(src, 2, string::npos);
  // "名前は中野です"
  EXPECT_EQ("\xe5\x90\x8d\xe5\x89\x8d\xe3\x81\xaf\xe4\xb8\xad\xe9\x87"
            "\x8e\xe3\x81\xa7\xe3\x81\x99",
            result);
  EXPECT_LE(src.data(), result.data());

  result = Util::SubStringPiece(src, 5, string::npos);
  // "中野です"
  EXPECT_EQ("\xe4\xb8\xad\xe9\x87\x8e\xe3\x81\xa7\xe3\x81\x99", result);
  EXPECT_LE(src.data(), result.data());
}

TEST(UtilTest, SubStringPiece2) {
  // "私はGoogleです"
  const string src =
      "\xE7\xA7\x81\xE3\x81\xAF\x47\x6F\x6F\x67\x6C\x65"
      "\xE3\x81\xA7\xE3\x81\x99";

  StringPiece result;

  result = Util::SubStringPiece(src, 0);
  EXPECT_EQ(src, result);

  result = Util::SubStringPiece(src, 5);
  // "gleです"
  EXPECT_EQ("\x67\x6C\x65\xE3\x81\xA7\xE3\x81\x99", result);

  result = Util::SubStringPiece(src, 10);
  EXPECT_TRUE(result.empty());

  result = Util::SubStringPiece(src, 13);
  EXPECT_TRUE(result.empty());
}

TEST(UtilTest, SubString) {
  // "私の名前は中野です"
  const string src = "\xe7\xa7\x81\xe3\x81\xae\xe5\x90\x8d\xe5\x89\x8d\xe3\x81"
                     "\xaf\xe4\xb8\xad\xe9\x87\x8e\xe3\x81\xa7\xe3\x81\x99";
  string result;

  result.clear();
  Util::SubString(src, 0, 2, &result);
  // "私の"
  EXPECT_EQ(result, "\xe7\xa7\x81\xe3\x81\xae");

  result.clear();
  Util::SubString(src, 4, 1, &result);
  // "は"
  EXPECT_EQ(result, "\xe3\x81\xaf");

  result.clear();
  Util::SubString(src, 5, 3, &result);
  // "中野で"
  EXPECT_EQ(result, "\xe4\xb8\xad\xe9\x87\x8e\xe3\x81\xa7");

  result.clear();
  Util::SubString(src, 6, 10, &result);
  // "野です"
  EXPECT_EQ(result, "\xe9\x87\x8e\xe3\x81\xa7\xe3\x81\x99");

  result.clear();
  Util::SubString(src, 4, 2, &result);
  // "は中"
  EXPECT_EQ(result, "\xe3\x81\xaf\xe4\xb8\xad");

  result.clear();
  Util::SubString(src, 2, string::npos, &result);
  // "名前は中野です"
  EXPECT_EQ(result, "\xe5\x90\x8d\xe5\x89\x8d\xe3\x81\xaf\xe4\xb8\xad\xe9\x87"
                    "\x8e\xe3\x81\xa7\xe3\x81\x99");

  result.clear();
  Util::SubString(src, 5, string::npos, &result);
  // "中野です"
  EXPECT_EQ(result, "\xe4\xb8\xad\xe9\x87\x8e\xe3\x81\xa7\xe3\x81\x99");

  // Doesn't clear result and call Util::SubString
  Util::SubString(src, 5, string::npos, &result);
  // "中野です"
  EXPECT_EQ(result, "\xe4\xb8\xad\xe9\x87\x8e\xe3\x81\xa7\xe3\x81\x99");
}

TEST(UtilTest, StartsWith) {
  const string str = "abcdefg";
  EXPECT_TRUE(Util::StartsWith(str, ""));
  EXPECT_TRUE(Util::StartsWith(str, "a"));
  EXPECT_TRUE(Util::StartsWith(str, "abc"));
  EXPECT_TRUE(Util::StartsWith(str, "abcdefg"));
  EXPECT_FALSE(Util::StartsWith(str, "abcdefghi"));
  EXPECT_FALSE(Util::StartsWith(str, "foobar"));
}

TEST(UtilTest, EndsWith) {
  const string str = "abcdefg";
  EXPECT_TRUE(Util::EndsWith(str, ""));
  EXPECT_TRUE(Util::EndsWith(str, "g"));
  EXPECT_TRUE(Util::EndsWith(str, "fg"));
  EXPECT_TRUE(Util::EndsWith(str, "abcdefg"));
  EXPECT_FALSE(Util::EndsWith(str, "aaabcdefg"));
  EXPECT_FALSE(Util::EndsWith(str, "foobar"));
  EXPECT_FALSE(Util::EndsWith(str, "foobarbuzbuz"));
}

TEST(UtilTest, StripUTF8BOM) {
  string line;

  // Should be stripped.
  line = "\xef\xbb\xbf" "abc";
  Util::StripUTF8BOM(&line);
  EXPECT_EQ("abc", line);

  // Should be stripped.
  line = "\xef\xbb\xbf";
  Util::StripUTF8BOM(&line);
  EXPECT_EQ("", line);

  // BOM in the middle of text. Shouldn't be stripped.
  line = "a" "\xef\xbb\xbf" "bc";
  Util::StripUTF8BOM(&line);
  EXPECT_EQ("a" "\xef\xbb\xbf" "bc", line);

  // Incomplete BOM. Shouldn't be stripped.
  line = "\xef\xbb" "abc";
  Util::StripUTF8BOM(&line);
  EXPECT_EQ("\xef\xbb" "abc", line);

  // String shorter than the BOM. Do nothing.
  line = "a";
  Util::StripUTF8BOM(&line);
  EXPECT_EQ("a", line);

  // Empty string. Do nothing.
  line = "";
  Util::StripUTF8BOM(&line);
  EXPECT_EQ("", line);
}

TEST(UtilTest, IsUTF16BOM) {
  EXPECT_FALSE(Util::IsUTF16BOM(""));
  EXPECT_FALSE(Util::IsUTF16BOM("abc"));
  EXPECT_TRUE(Util::IsUTF16BOM("\xfe\xff"));
  EXPECT_TRUE(Util::IsUTF16BOM("\xff\xfe"));
  EXPECT_TRUE(Util::IsUTF16BOM("\xfe\xff "));
  EXPECT_TRUE(Util::IsUTF16BOM("\xff\xfe "));
  EXPECT_FALSE(Util::IsUTF16BOM(" \xfe\xff"));
  EXPECT_FALSE(Util::IsUTF16BOM(" \xff\xfe"));
  EXPECT_FALSE(Util::IsUTF16BOM("\xff\xff"));
}

TEST(UtilTest, IsAndroidPuaEmoji) {
  EXPECT_FALSE(Util::IsAndroidPuaEmoji(""));
  EXPECT_FALSE(Util::IsAndroidPuaEmoji("A"));
  EXPECT_FALSE(Util::IsAndroidPuaEmoji("a"));

  string str;
  Util::UCS4ToUTF8(0xFDFFF, &str);
  EXPECT_FALSE(Util::IsAndroidPuaEmoji(str));
  Util::UCS4ToUTF8(0xFE000, &str);
  EXPECT_TRUE(Util::IsAndroidPuaEmoji(str));
  Util::UCS4ToUTF8(0xFE800, &str);
  EXPECT_TRUE(Util::IsAndroidPuaEmoji(str));
  Util::UCS4ToUTF8(0xFEEA0, &str);
  EXPECT_TRUE(Util::IsAndroidPuaEmoji(str));
  Util::UCS4ToUTF8(0xFEEA1, &str);
  EXPECT_FALSE(Util::IsAndroidPuaEmoji(str));

  // If it has two ucs4 chars (or more), just expect false.
  Util::UCS4ToUTF8(0xFE000, &str);
  Util::UCS4ToUTF8Append(0xFE000, &str);
  EXPECT_FALSE(Util::IsAndroidPuaEmoji(str));
}

TEST(UtilTest, StringPrintf) {
  // On GCC, |EXPECT_EQ("", Util::StringPrintf(""))| may cause
  // "warning: zero-length printf format string" so we disable this check.
  // TODO:
  // GBASE_GCC_DISABLE_WARNING_INLINE(format-zero-length);

  // strings
  // EXPECT_EQ("", Util::StringPrintf(""));
  EXPECT_EQ("", Util::StringPrintf("%s", ""));
  EXPECT_EQ("hello, world", Util::StringPrintf("hello, world"));
  EXPECT_EQ("hello, world", Util::StringPrintf("%s", "hello, world"));
  EXPECT_EQ("hello, world", Util::StringPrintf("%s, %s", "hello", "world"));
  const char kHello[] = "\xE3\x81\xAF\xE3\x82\x8D\xE3\x83\xBC"  // はろー
                        "\xE4\xB8\x96\xE7\x95\x8C";  // 世界
  EXPECT_EQ(kHello, Util::StringPrintf("%s", kHello));

  // 32-bit integers
  EXPECT_EQ("-2147483648", Util::StringPrintf("%d", kint32min));
  EXPECT_EQ("2147483647", Util::StringPrintf("%d", kint32max));
  EXPECT_EQ("4294967295", Util::StringPrintf("%u", kuint32max));
  EXPECT_EQ("80000000", Util::StringPrintf("%x", kint32min));
  EXPECT_EQ("7fffffff", Util::StringPrintf("%x", kint32max));
  EXPECT_EQ("FFFFFFFF", Util::StringPrintf("%X", kuint32max));

  // 64-bit integers
  EXPECT_EQ("-9223372036854775808",
            Util::StringPrintf("%" GG_LL_FORMAT "d", kint64min));
  EXPECT_EQ("9223372036854775807",
            Util::StringPrintf("%" GG_LL_FORMAT "d", kint64max));
  EXPECT_EQ("18446744073709551615",
            Util::StringPrintf("%" GG_LL_FORMAT "u", kuint64max));
  EXPECT_EQ("8000000000000000",
            Util::StringPrintf("%" GG_LL_FORMAT "x", kint64min));
  EXPECT_EQ("7fffffffffffffff",
            Util::StringPrintf("%" GG_LL_FORMAT "x", kint64max));
  EXPECT_EQ("FFFFFFFFFFFFFFFF",
            Util::StringPrintf("%" GG_LL_FORMAT "X", kuint64max));

  // Simple test for floating point numbers
  EXPECT_EQ("-1.75", Util::StringPrintf("%.2f", -1.75));

  // 4096 is greater than a temporary buffer size (1024 bytes)
  // which is used in StringPrintf().
  const string kLongStrA(4096, '.');
  const string kLongStrB(4096, '_');
  const string& result = Util::StringPrintf("%s\t%s\n",
                                            kLongStrA.c_str(),
                                            kLongStrB.c_str());
  EXPECT_EQ(kLongStrA + "\t" + kLongStrB + "\n", result);
}

TEST(UtilTest, BracketTest) {
  static const struct BracketType {
    const char *open_bracket;
    const char *close_bracket;
  } kBracketType[] = {
    //  { "（", "）" },
    //  { "〔", "〕" },
    //  { "［", "］" },
    //  { "｛", "｝" },
    //  { "〈", "〉" },
    //  { "《", "》" },
    //  { "「", "」" },
    //  { "『", "』" },
    //  { "【", "】" },
    //  { "〘", "〙" },
    //  { "〚", "〛" },
    { "\xEF\xBC\x88", "\xEF\xBC\x89" },
    { "\xE3\x80\x94", "\xE3\x80\x95" },
    { "\xEF\xBC\xBB", "\xEF\xBC\xBD" },
    { "\xEF\xBD\x9B", "\xEF\xBD\x9D" },
    { "\xE3\x80\x88", "\xE3\x80\x89" },
    { "\xE3\x80\x8A", "\xE3\x80\x8B" },
    { "\xE3\x80\x8C", "\xE3\x80\x8D" },
    { "\xE3\x80\x8E", "\xE3\x80\x8F" },
    { "\xE3\x80\x90", "\xE3\x80\x91" },
    { "\xe3\x80\x98", "\xe3\x80\x99" },
    { "\xe3\x80\x9a", "\xe3\x80\x9b" },
    { NULL, NULL },  // sentinel
  };

  string pair;
  for (size_t i = 0;
       (kBracketType[i].open_bracket != NULL ||
        kBracketType[i].close_bracket != NULL);
       ++i) {
    EXPECT_TRUE(Util::IsOpenBracket(kBracketType[i].open_bracket, &pair));
    EXPECT_EQ(kBracketType[i].close_bracket, pair);
    EXPECT_TRUE(Util::IsCloseBracket(kBracketType[i].close_bracket, &pair));
    EXPECT_EQ(kBracketType[i].open_bracket, pair);
    EXPECT_FALSE(Util::IsOpenBracket(kBracketType[i].close_bracket, &pair));
    EXPECT_FALSE(Util::IsCloseBracket(kBracketType[i].open_bracket, &pair));
  }
}

TEST(UtilTest, ChopReturns) {
  string line = "line\n";
  EXPECT_TRUE(Util::ChopReturns(&line));
  EXPECT_EQ("line", line);

  line = "line\r";
  EXPECT_TRUE(Util::ChopReturns(&line));
  EXPECT_EQ("line", line);

  line = "line\r\n";
  EXPECT_TRUE(Util::ChopReturns(&line));
  EXPECT_EQ("line", line);

  line = "line";
  EXPECT_FALSE(Util::ChopReturns(&line));
  EXPECT_EQ("line", line);

  line = "line1\nline2\n";
  EXPECT_TRUE(Util::ChopReturns(&line));
  EXPECT_EQ("line1\nline2", line);

  line = "line\n\n\n";
  EXPECT_TRUE(Util::ChopReturns(&line));
  EXPECT_EQ("line", line);
}

TEST(UtilTest, EncodeURI) {
  string encoded;
  // "もずく"
  Util::EncodeURI("\xe3\x82\x82\xe3\x81\x9a\xe3\x81\x8f", &encoded);
  EXPECT_EQ("%E3%82%82%E3%81%9A%E3%81%8F", encoded);

  encoded.clear();
  Util::EncodeURI("gbase", &encoded);
  EXPECT_EQ("gbase", encoded);

  encoded.clear();
  Util::EncodeURI("http://gbase/?q=Hello World", &encoded);
  EXPECT_EQ("http%3A%2F%2Fgbase%2F%3Fq%3DHello%20World", encoded);
}

TEST(UtilTest, DecodeURI) {
  string decoded;
  Util::DecodeURI("%E3%82%82%E3%81%9A%E3%81%8F", &decoded);
  // "もずく"
  EXPECT_EQ("\xe3\x82\x82\xe3\x81\x9a\xe3\x81\x8f", decoded);

  decoded.clear();
  Util::DecodeURI("gbase", &decoded);
  EXPECT_EQ("gbase", decoded);

  decoded.clear();
  Util::DecodeURI("http%3A%2F%2Fgbase%2F%3Fq%3DHello+World", &decoded);
  EXPECT_EQ("http://gbase/?q=Hello World", decoded);
}

TEST(UtilTest, AppendCGIParams) {
  std::vector<std::pair<string, string> > params;
  string url;
  Util::AppendCGIParams(params, &url);
  EXPECT_TRUE(url.empty());

  params.push_back(std::make_pair("foo", "b a+r"));
  url = "http://gbase.com?";
  Util::AppendCGIParams(params, &url);
  EXPECT_EQ("http://gbase.com?foo=b%20a%2Br", url);

  params.push_back(std::make_pair("buzz", "gbase"));
  url.clear();
  Util::AppendCGIParams(params, &url);
  EXPECT_EQ("foo=b%20a%2Br&buzz=gbase", url);
}

TEST(UtilTest, Escape) {
  string escaped;
  // "らむだ"
  Util::Escape("\xe3\x82\x89\xe3\x82\x80\xe3\x81\xa0", &escaped);
  EXPECT_EQ("\\xE3\\x82\\x89\\xE3\\x82\\x80\\xE3\\x81\\xA0", escaped);
}

TEST(UtilTest, Unescape) {
  string unescaped;
  // "らむだ"
  EXPECT_TRUE(Util::Unescape("\\xE3\\x82\\x89\\xE3\\x82\\x80\\xE3\\x81\\xA0",
                             &unescaped));
  EXPECT_EQ("\xe3\x82\x89\xe3\x82\x80\xe3\x81\xa0", unescaped);

  // A binary sequence (upper case)
  EXPECT_TRUE(Util::Unescape("\\x00\\x01\\xEF\\xFF", &unescaped));
  EXPECT_EQ(string("\x00\x01\xEF\xFF", 4), unescaped);

  // A binary sequence (lower case)
  EXPECT_TRUE(Util::Unescape("\\x00\\x01\\xef\\xff", &unescaped));
  EXPECT_EQ(string("\x00\x01\xEF\xFF", 4), unescaped);

  EXPECT_TRUE(Util::Unescape("", &unescaped));
  EXPECT_TRUE(unescaped.empty());

  EXPECT_FALSE(Util::Unescape("\\AB\\CD\\EFG", &unescaped));
  EXPECT_FALSE(Util::Unescape("\\01\\XY", &unescaped));
}

TEST(UtilTest, EscapeUrl) {
  string escaped;
  // "らむだ"
  Util::EscapeUrl("\xe3\x82\x89\xe3\x82\x80\xe3\x81\xa0", &escaped);
  EXPECT_EQ("%E3%82%89%E3%82%80%E3%81%A0", escaped);
  EXPECT_EQ("%E3%82%89%E3%82%80%E3%81%A0",
            Util::EscapeUrl("\xe3\x82\x89\xe3\x82\x80\xe3\x81\xa0"));
}

TEST(UtilTest, EscapeHtml) {
  string escaped;
  Util::EscapeHtml("<>&'\"abc", &escaped);
  EXPECT_EQ("&lt;&gt;&amp;&#39;&quot;abc", escaped);
}

TEST(UtilTest, EscapeCss) {
  string escaped;
  Util::EscapeCss("<>&'\"abc", &escaped);
  EXPECT_EQ("&lt;>&'\"abc", escaped);
}

TEST(UtilTest, ScriptType) {
  // "くどう"
  EXPECT_TRUE(Util::IsScriptType("\xe3\x81\x8f\xe3\x81\xa9\xe3\x81\x86",
                                 Util::HIRAGANA));
  // "京都"
  EXPECT_TRUE(Util::IsScriptType("\xe4\xba\xac\xe9\x83\xbd", Util::KANJI));
  // "人々" (b/4201140)
  EXPECT_TRUE(Util::IsScriptType("\xE4\xBA\xBA\xE3\x80\x85", Util::KANJI));
  // "モズク"
  EXPECT_TRUE(Util::IsScriptType("\xe3\x83\xa2\xe3\x82\xba\xe3\x82\xaf",
                                 Util::KATAKANA));
  // "モズクﾓｽﾞｸ"
  EXPECT_TRUE(Util::IsScriptType("\xe3\x83\xa2\xe3\x82\xba\xe3\x82\xaf\xef\xbe"
                                 "\x93\xef\xbd\xbd\xef\xbe\x9e\xef\xbd\xb8",
                                 Util::KATAKANA));
  // "ぐーぐる"
  EXPECT_TRUE(Util::IsScriptType("\xe3\x81\x90\xe3\x83\xbc\xe3\x81\x90\xe3\x82"
                                 "\x8b", Util::HIRAGANA));
  // "グーグル"
  EXPECT_TRUE(Util::IsScriptType("\xe3\x82\xb0\xe3\x83\xbc\xe3\x82\xb0\xe3\x83"
                                 "\xab", Util::KATAKANA));
  // "ゟ" U+309F: HIRAGANA DIGRAPH YORI
  EXPECT_TRUE(Util::IsScriptType("\xE3\x82\x9F", Util::HIRAGANA));
  // "ヿ" U+30FF: KATAKANA DIGRAPH KOTO
  EXPECT_TRUE(Util::IsScriptType("\xE3\x83\xBF", Util::KATAKANA));
  // "ヷヸヹヺㇰㇱㇲㇳㇴㇵㇶㇷㇸㇹㇺㇻㇼㇽㇾㇿ"
  EXPECT_TRUE(Util::IsScriptType(
      "\xE3\x83\xB7\xE3\x83\xB8\xE3\x83\xB9\xE3\x83\xBA\xE3\x87\xB0"
      "\xE3\x87\xB1\xE3\x87\xB2\xE3\x87\xB3\xE3\x87\xB4\xE3\x87\xB5"
      "\xE3\x87\xB6\xE3\x87\xB7\xE3\x87\xB8\xE3\x87\xB9\xE3\x87\xBA"
      "\xE3\x87\xBB\xE3\x87\xBC\xE3\x87\xBD\xE3\x87\xBE\xE3\x87\xBF",
      Util::KATAKANA));
  // "𛀀" U+1B000: KATAKANA LETTER ARCHAIC E
  EXPECT_TRUE(Util::IsScriptType("\xF0\x9B\x80\x80", Util::KATAKANA));
  // "𛀁" U+1B001: HIRAGANA LETTER ARCHAIC YE
  EXPECT_TRUE(Util::IsScriptType("\xF0\x9B\x80\x81", Util::HIRAGANA));

  EXPECT_TRUE(Util::IsScriptType("012", Util::NUMBER));
  // "０１２012"
  EXPECT_TRUE(Util::IsScriptType("\xef\xbc\x90\xef\xbc\x91\xef\xbc\x92\x30\x31"
                                 "\x32", Util::NUMBER));
  EXPECT_TRUE(Util::IsScriptType("abcABC", Util::ALPHABET));
  // "ＡＢＣＤ"
  EXPECT_TRUE(Util::IsScriptType("\xef\xbc\xa1\xef\xbc\xa2\xef\xbc\xa3\xef\xbc"
                                 "\xa4", Util::ALPHABET));
  EXPECT_TRUE(Util::IsScriptType("@!#", Util::UNKNOWN_SCRIPT));

  // "くどカう"
  EXPECT_FALSE(Util::IsScriptType("\xe3\x81\x8f\xe3\x81\xa9\xe3\x82\xab\xe3\x81"
                                  "\x86", Util::HIRAGANA));
  // "京あ都"
  EXPECT_FALSE(Util::IsScriptType("\xe4\xba\xac\xe3\x81\x82\xe9\x83\xbd",
                                  Util::KANJI));
  // "モズあク"
  EXPECT_FALSE(Util::IsScriptType("\xe3\x83\xa2\xe3\x82\xba\xe3\x81\x82\xe3\x82"
                                  "\xaf", Util::KATAKANA));
  // "モあズクﾓｽﾞｸ"
  EXPECT_FALSE(Util::IsScriptType("\xe3\x83\xa2\xe3\x81\x82\xe3\x82\xba\xe3\x82"
                                  "\xaf\xef\xbe\x93\xef\xbd\xbd\xef\xbe\x9e\xef"
                                  "\xbd\xb8", Util::KATAKANA));
  // "012あ"
  EXPECT_FALSE(Util::IsScriptType("\x30\x31\x32\xe3\x81\x82", Util::NUMBER));
  // "０１２あ012"
  EXPECT_FALSE(Util::IsScriptType("\xef\xbc\x90\xef\xbc\x91\xef\xbc\x92\xe3\x81"
                                  "\x82\x30\x31\x32", Util::NUMBER));
  // "abcABあC"
  EXPECT_FALSE(Util::IsScriptType("\x61\x62\x63\x41\x42\xe3\x81\x82\x43",
                                  Util::ALPHABET));
  // "ＡＢあＣＤ"
  EXPECT_FALSE(Util::IsScriptType("\xef\xbc\xa1\xef\xbc\xa2\xe3\x81\x82\xef\xbc"
                                  "\xa3\xef\xbc\xa4", Util::ALPHABET));
  // "ぐーぐるグ"
  EXPECT_FALSE(Util::IsScriptType("\xe3\x81\x90\xe3\x83\xbc\xe3\x81\x90\xe3\x82"
                                  "\x8b\xe3\x82\xb0", Util::HIRAGANA));
  // "グーグルぐ"
  EXPECT_FALSE(Util::IsScriptType("\xe3\x82\xb0\xe3\x83\xbc\xe3\x82\xb0\xe3\x83"
                                  "\xab\xe3\x81\x90", Util::KATAKANA));

  // "グーグルsuggest"
  EXPECT_TRUE(Util::ContainsScriptType("\xe3\x82\xb0\xe3\x83\xbc\xe3\x82\xb0"
                                       "\xe3\x83\xab\x73\x75\x67\x67\x65\x73"
                                       "\x74", Util::ALPHABET));
  // "グーグルサジェスト"
  EXPECT_FALSE(Util::ContainsScriptType("\xe3\x82\xb0\xe3\x83\xbc\xe3\x82\xb0"
                                        "\xe3\x83\xab\xe3\x82\xb5\xe3\x82\xb8"
                                        "\xe3\x82\xa7\xe3\x82\xb9\xe3\x83\x88",
                                        Util::ALPHABET));

  // "くどう"
  EXPECT_EQ(Util::HIRAGANA, Util::GetScriptType("\xe3\x81\x8f\xe3\x81\xa9\xe3"
                                                "\x81\x86"));
  // "京都"
  EXPECT_EQ(Util::KANJI, Util::GetScriptType("\xe4\xba\xac\xe9\x83\xbd"));
  // "人々" (b/4201140)
  EXPECT_EQ(Util::KANJI, Util::GetScriptType("\xE4\xBA\xBA\xE3\x80\x85"));
  // "モズク"
  EXPECT_EQ(Util::KATAKANA, Util::GetScriptType("\xe3\x83\xa2\xe3\x82\xba\xe3"
                                                "\x82\xaf"));
  // "モズクﾓｽﾞｸ"
  EXPECT_EQ(Util::KATAKANA, Util::GetScriptType("\xe3\x83\xa2\xe3\x82\xba\xe3"
                                                "\x82\xaf\xef\xbe\x93\xef\xbd"
                                                "\xbd\xef\xbe\x9e\xef\xbd"
                                                "\xb8"));
  // "ぐーぐる"
  EXPECT_EQ(Util::HIRAGANA, Util::GetScriptType("\xe3\x81\x90\xe3\x83\xbc\xe3"
                                                "\x81\x90\xe3\x82\x8b"));
  EXPECT_EQ(Util::HIRAGANA,
            Util::GetFirstScriptType("\xe3\x81\x90\xe3\x83\xbc\xe3\x81\x90"
                                     "\xe3\x82\x8b"));

  // "グーグル"
  EXPECT_EQ(Util::KATAKANA, Util::GetScriptType("\xe3\x82\xb0\xe3\x83\xbc\xe3"
                                                "\x82\xb0\xe3\x83\xab"));
  EXPECT_EQ(Util::KATAKANA,
            Util::GetFirstScriptType("\xe3\x82\xb0\xe3\x83\xbc\xe3\x82\xb0"
                                     "\xe3\x83\xab"));
  // "ゟ" U+309F HIRAGANA DIGRAPH YORI
  EXPECT_EQ(Util::HIRAGANA, Util::GetScriptType("\xE3\x82\x9F"));
  EXPECT_EQ(Util::HIRAGANA, Util::GetFirstScriptType("\xE3\x82\x9F"));

  // "ヿ" U+30FF KATAKANA DIGRAPH KOTO
  EXPECT_EQ(Util::KATAKANA, Util::GetScriptType("\xE3\x83\xBF"));
  // "ヷヸヹヺㇰㇱㇲㇳㇴㇵㇶㇷㇸㇹㇺㇻㇼㇽㇾㇿ"
  EXPECT_EQ(Util::KATAKANA, Util::GetScriptType(
      "\xE3\x83\xB7\xE3\x83\xB8\xE3\x83\xB9\xE3\x83\xBA\xE3\x87\xB0"
      "\xE3\x87\xB1\xE3\x87\xB2\xE3\x87\xB3\xE3\x87\xB4\xE3\x87\xB5"
      "\xE3\x87\xB6\xE3\x87\xB7\xE3\x87\xB8\xE3\x87\xB9\xE3\x87\xBA"
      "\xE3\x87\xBB\xE3\x87\xBC\xE3\x87\xBD\xE3\x87\xBE\xE3\x87\xBF"));
  // "𛀀" U+1B000 KATAKANA LETTER ARCHAIC E
  EXPECT_EQ(Util::KATAKANA, Util::GetScriptType("\xF0\x9B\x80\x80"));
  // "𛀁" U+1B001 HIRAGANA LETTER ARCHAIC YE
  EXPECT_EQ(Util::HIRAGANA, Util::GetScriptType("\xF0\x9B\x80\x81"));

  // "!グーグル"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptType("\x21\xe3\x82\xb0\xe3\x83"
                                                      "\xbc\xe3\x82\xb0\xe3\x83"
                                                      "\xab"));
  // "ー"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptType("\xe3\x83\xbc"));
  EXPECT_EQ(Util::KATAKANA, Util::GetFirstScriptType("\xe3\x83\xbc"));
  // "ーー"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptType("\xe3\x83\xbc\xe3\x83"
                                                      "\xbc"));
  EXPECT_EQ(Util::KATAKANA, Util::GetFirstScriptType("\xe3\x83\xbc\xe3"
                                                           "\x83\xbc"));
  // "゛"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptType("\xe3\x82\x9b"));
  // "゜"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptType("\xe3\x82\x9c"));

  EXPECT_EQ(Util::NUMBER, Util::GetScriptType("012"));
  // "０１２012"
  EXPECT_EQ(Util::NUMBER, Util::GetScriptType("\xef\xbc\x90\xef\xbc\x91\xef\xbc"
                                              "\x92\x30\x31\x32"));
  EXPECT_EQ(Util::ALPHABET, Util::GetScriptType("abcABC"));
  // "ＡＢＣＤ"
  EXPECT_EQ(Util::ALPHABET, Util::GetScriptType("\xef\xbc\xa1\xef\xbc\xa2\xef"
                                                "\xbc\xa3\xef\xbc\xa4"));
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptType("@!#"));
  // "＠！＃"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptType("\xef\xbc\xa0\xef\xbc\x81"
                                                      "\xef\xbc\x83"));

  // "ーひらがな"
  EXPECT_EQ(Util::HIRAGANA, Util::GetScriptType("\xe3\x83\xbc\xe3\x81\xb2\xe3"
                                                "\x82\x89\xe3\x81\x8c\xe3\x81"
                                                "\xaa"));
  EXPECT_EQ(Util::KATAKANA, Util::GetFirstScriptType("\xe3\x83\xbc\xe3\x81\xb2"
                                                     "\xe3\x82\x89\xe3\x81\x8c"
                                                     "\xe3\x81\xaa"));
  // "ーカタカナ"
  EXPECT_EQ(Util::KATAKANA, Util::GetScriptType("\xe3\x83\xbc\xe3\x82\xab\xe3"
                                                "\x82\xbf\xe3\x82\xab\xe3\x83"
                                                "\x8a"));
  // "ｰｶﾀｶﾅ"
  EXPECT_EQ(Util::KATAKANA, Util::GetScriptType("\xef\xbd\xb0\xef\xbd\xb6\xef"
                                                "\xbe\x80\xef\xbd\xb6\xef\xbe"
                                                "\x85"));
  // "ひらがなー"
  EXPECT_EQ(Util::HIRAGANA, Util::GetScriptType("\xe3\x81\xb2\xe3\x82\x89\xe3"
                                                "\x81\x8c\xe3\x81\xaa\xe3\x83"
                                                "\xbc"));
  // "カタカナー"
  EXPECT_EQ(Util::KATAKANA, Util::GetScriptType("\xe3\x82\xab\xe3\x82\xbf\xe3"
                                                "\x82\xab\xe3\x83\x8a\xe3\x83"
                                                "\xbc"));
  // "ｶﾀｶﾅｰ"
  EXPECT_EQ(Util::KATAKANA, Util::GetScriptType("\xef\xbd\xb6\xef\xbe\x80\xef"
                                                "\xbd\xb6\xef\xbe\x85\xef\xbd"
                                                "\xb0"));

  // "あ゛っ"
  EXPECT_EQ(Util::HIRAGANA, Util::GetScriptType("\xe3\x81\x82\xe3\x82\x9b\xe3"
                                                "\x81\xa3"));
  // "あ゜っ"
  EXPECT_EQ(Util::HIRAGANA, Util::GetScriptType("\xe3\x81\x82\xe3\x82\x9c\xe3"
                                                "\x81\xa3"));
  // "ア゛ッ"
  EXPECT_EQ(Util::KATAKANA, Util::GetScriptType("\xe3\x82\xa2\xe3\x82\x9b\xe3"
                                                "\x83\x83"));
  // "ア゜ッ"
  EXPECT_EQ(Util::KATAKANA, Util::GetScriptType("\xe3\x82\xa2\xe3\x82\x9c\xe3"
                                                "\x83\x83"));

  // "くどカう"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptType("\xe3\x81\x8f\xe3\x81\xa9"
                                                      "\xe3\x82\xab\xe3\x81"
                                                      "\x86"));
  // "京あ都"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptType("\xe4\xba\xac\xe3\x81\x82"
                                                      "\xe9\x83\xbd"));
  EXPECT_EQ(Util::KANJI, Util::GetFirstScriptType("\xe4\xba\xac\xe3\x81\x82"
                                                  "\xe9\x83\xbd"));

  // "モズあク"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptType("\xe3\x83\xa2\xe3\x82\xba"
                                                      "\xe3\x81\x82\xe3\x82"
                                                      "\xaf"));
  EXPECT_EQ(Util::KATAKANA, Util::GetFirstScriptType("\xe3\x83\xa2\xe3\x82\xba"
                                                     "\xe3\x81\x82\xe3\x82"
                                                     "\xaf"));

  // "モあズクﾓｽﾞｸ"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptType("\xe3\x83\xa2\xe3\x81\x82"
                                                      "\xe3\x82\xba\xe3\x82\xaf"
                                                      "\xef\xbe\x93\xef\xbd\xbd"
                                                      "\xef\xbe\x9e\xef\xbd"
                                                      "\xb8"));
  // "012あ"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptType("\x30\x31\x32\xe3\x81"
                                                      "\x82"));
  EXPECT_EQ(Util::NUMBER, Util::GetFirstScriptType("\x30\x31\x32\xe3\x81"
                                                   "\x82"));
  // "０１２あ012"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptType("\xef\xbc\x90\xef\xbc\x91"
                                                      "\xef\xbc\x92\xe3\x81\x82"
                                                      "\x30\x31\x32"));
  EXPECT_EQ(Util::NUMBER, Util::GetFirstScriptType("\xef\xbc\x90\xef\xbc\x91"
                                                   "\xef\xbc\x92\xe3\x81\x82"
                                                   "\x30\x31\x32"));
  // "abcABあC"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptType("\x61\x62\x63\x41\x42\xe3"
                                                      "\x81\x82\x43"));
  // "ＡＢあＣＤ"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptType("\xef\xbc\xa1\xef\xbc\xa2"
                                                      "\xe3\x81\x82\xef\xbc\xa3"
                                                      "\xef\xbc\xa4"));
  // "ぐーぐるグ"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptType("\xe3\x81\x90\xe3\x83\xbc"
                                                      "\xe3\x81\x90\xe3\x82\x8b"
                                                      "\xe3\x82\xb0"));
  // "グーグルぐ"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptType("\xe3\x82\xb0\xe3\x83\xbc"
                                                      "\xe3\x82\xb0\xe3\x83\xab"
                                                      "\xe3\x81\x90"));

  // "龦" U+9FA6
  EXPECT_EQ(Util::KANJI, Util::GetScriptType("\xE9\xBE\xA6"));
  // "龻" U+9FBB
  EXPECT_EQ(Util::KANJI, Util::GetScriptType("\xE9\xBE\xBB"));
  // U+9FFF is not assigned yet but reserved for CJK Unified Ideographs.
  EXPECT_EQ(Util::KANJI, Util::GetScriptType("\xE9\xBF\xBF"));
  // "𠮟咤" U+20B9F U+54A4
  EXPECT_EQ(Util::KANJI, Util::GetScriptType("\xF0\xA0\xAE\x9F\xE5\x92\xA4"));
  // "𠮷野" U+20BB7 U+91CE
  EXPECT_EQ(Util::KANJI, Util::GetScriptType("\xF0\xA0\xAE\xB7\xE9\x87\x8E"));
  // "巽" U+2F884
  EXPECT_EQ(Util::KANJI, Util::GetScriptType("\xF0\xAF\xA2\x84"));

  // U+1F466, BOY/smile emoji
  EXPECT_EQ(Util::EMOJI, Util::GetScriptType("\xF0\x9F\x91\xA6"));
  // U+FE003, Snow-man Android PUA emoji
  EXPECT_TRUE(Util::IsAndroidPuaEmoji("\xf3\xbe\x80\x83"));
  EXPECT_EQ(Util::EMOJI, Util::GetScriptType("\xf3\xbe\x80\x83"));
}


TEST(UtilTest, ScriptTypeWithoutSymbols) {
  // "くど う"
  EXPECT_EQ(Util::HIRAGANA, Util::GetScriptTypeWithoutSymbols(
      "\xe3\x81\x8f\xe3\x81\xa9 \xe3\x81\x86"));
  // "京 都"
  EXPECT_EQ(Util::KANJI, Util::GetScriptTypeWithoutSymbols(
      "\xe4\xba\xac \xe9\x83\xbd"));
  // "モズ ク"
  EXPECT_EQ(Util::KATAKANA, Util::GetScriptTypeWithoutSymbols(
      "\xe3\x83\xa2\xe3\x82\xba\xe3\x82\xaf"));
  // "モズ クﾓｽﾞｸ"
  EXPECT_EQ(Util::KATAKANA, Util::GetScriptTypeWithoutSymbols(
      "\xe3\x83\xa2\xe3\x82\xba \xe3\x82\xaf\xef\xbe\x93\xef\xbd"
      "\xbd\xef\xbe\x9e\xef\xbd\xb8"));
  // "Google Earth"
  EXPECT_EQ(Util::ALPHABET, Util::GetScriptTypeWithoutSymbols(
      "Google Earth"));
  // "Google "
  EXPECT_EQ(Util::ALPHABET, Util::GetScriptTypeWithoutSymbols(
      "Google "));
  // " Google"
  EXPECT_EQ(Util::ALPHABET, Util::GetScriptTypeWithoutSymbols(
      " Google"));
  // " Google "
  EXPECT_EQ(Util::ALPHABET, Util::GetScriptTypeWithoutSymbols(
      " Google "));
  // "     g"
  EXPECT_EQ(Util::ALPHABET, Util::GetScriptTypeWithoutSymbols(
      "     g"));
  // ""
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptTypeWithoutSymbols(
      ""));
  // " "
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptTypeWithoutSymbols(
      " "));
  // "  "
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptTypeWithoutSymbols(
      "   "));
  EXPECT_EQ(Util::ALPHABET, Util::GetScriptTypeWithoutSymbols("Hello!"));
  // "Hello!あ"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptTypeWithoutSymbols(
      "\x48\x65\x6c\x6c\x6f\x21\xe3\x81\x82"));
  EXPECT_EQ(Util::ALPHABET, Util::GetScriptTypeWithoutSymbols("CD-ROM"));
  // "CD-ROMア"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptTypeWithoutSymbols(
      "\x43\x44\x2d\x52\x4f\x4d\xe3\x82\xa2"));
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptTypeWithoutSymbols("-"));
  EXPECT_EQ(Util::ALPHABET, Util::GetScriptTypeWithoutSymbols("-A"));
  EXPECT_EQ(Util::ALPHABET, Util::GetScriptTypeWithoutSymbols("--A"));
  EXPECT_EQ(Util::ALPHABET, Util::GetScriptTypeWithoutSymbols("--A---"));
  // "--A-ｱ-"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptTypeWithoutSymbols(
      "\x2d\x2d\x41\x2d\xef\xbd\xb1\x2d"));
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptTypeWithoutSymbols("!"));
  // "・あ"
  EXPECT_EQ(Util::HIRAGANA, Util::GetScriptTypeWithoutSymbols(
      "\xe3\x83\xbb\xe3\x81\x82"));
  // "・・あ"
  EXPECT_EQ(Util::HIRAGANA, Util::GetScriptTypeWithoutSymbols(
      "\xe3\x83\xbb\xe3\x83\xbb\xe3\x81\x82"));
  // "コギト・エルゴ・スム"
  EXPECT_EQ(Util::KATAKANA, Util::GetScriptTypeWithoutSymbols(
      "\xe3\x82\xb3\xe3\x82\xae\xe3\x83\x88\xe3\x83\xbb\xe3\x82\xa8"
      "\xe3\x83\xab\xe3\x82\xb4\xe3\x83\xbb\xe3\x82\xb9\xe3\x83\xa0"));
  // "コギト・エルゴ・住む"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptTypeWithoutSymbols(
      "\xe3\x82\xb3\xe3\x82\xae\xe3\x83\x88\xe3\x83\xbb\xe3\x82\xa8"
      "\xe3\x83\xab\xe3\x82\xb4\xe3\x83\xbb\xe4\xbd\x8f\xe3\x82\x80"));
  // "人☆名"
  EXPECT_EQ(Util::KANJI, Util::GetScriptTypeWithoutSymbols(
      "\xe4\xba\xba\xe2\x98\x86\xe5\x90\x8d"));
  // "ひとの☆なまえ"
  EXPECT_EQ(Util::HIRAGANA, Util::GetScriptTypeWithoutSymbols(
      "\xe3\x81\xb2\xe3\x81\xa8\xe3\x81\xae\xe2\x98\x86\xe3\x81\xaa"
      "\xe3\x81\xbe\xe3\x81\x88"));
  // "超☆最高です"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptTypeWithoutSymbols(
      "\xe8\xb6\x85\xe2\x98\x86\xe6\x9c\x80\xe9\xab\x98\xe3\x81\xa7"
      "\xe3\x81\x99"));
  // "・--☆"
  EXPECT_EQ(Util::UNKNOWN_SCRIPT, Util::GetScriptTypeWithoutSymbols(
      "\xe3\x83\xbb\x2d\x2d\xe2\x98\x86"));
}

TEST(UtilTest, FormType) {
  // "くどう"
  EXPECT_EQ(Util::FULL_WIDTH, Util::GetFormType("\xe3\x81\x8f\xe3\x81\xa9\xe3"
                                                "\x81\x86"));
  // "京都"
  EXPECT_EQ(Util::FULL_WIDTH, Util::GetFormType("\xe4\xba\xac\xe9\x83\xbd"));
  // "モズク"
  EXPECT_EQ(Util::FULL_WIDTH, Util::GetFormType("\xe3\x83\xa2\xe3\x82\xba\xe3"
                                                "\x82\xaf"));
  // "ﾓｽﾞｸ"
  EXPECT_EQ(Util::HALF_WIDTH, Util::GetFormType("\xef\xbe\x93\xef\xbd\xbd\xef"
                                                "\xbe\x9e\xef\xbd\xb8"));
  // "ぐーぐる"
  EXPECT_EQ(Util::FULL_WIDTH, Util::GetFormType("\xe3\x81\x90\xe3\x83\xbc\xe3"
                                                "\x81\x90\xe3\x82\x8b"));
  // "グーグル"
  EXPECT_EQ(Util::FULL_WIDTH, Util::GetFormType("\xe3\x82\xb0\xe3\x83\xbc\xe3"
                                                "\x82\xb0\xe3\x83\xab"));
  // "ｸﾞｰｸﾞﾙ"
  EXPECT_EQ(Util::HALF_WIDTH, Util::GetFormType("\xef\xbd\xb8\xef\xbe\x9e\xef"
                                                "\xbd\xb0\xef\xbd\xb8\xef\xbe"
                                                "\x9e\xef\xbe\x99"));
  // "ｰ"
  EXPECT_EQ(Util::HALF_WIDTH, Util::GetFormType("\xef\xbd\xb0"));
  // "ー"
  EXPECT_EQ(Util::FULL_WIDTH, Util::GetFormType("\xe3\x83\xbc"));

  // "¢£¥¦¬¯"
  EXPECT_EQ(Util::HALF_WIDTH, Util::GetFormType("\xc2\xa2\xc2\xa3\xc2\xa5"
                                                "\xc2\xa6\xc2\xac\xc2\xaf"));

  // "￨￩￪￫￬￭￮"
  EXPECT_EQ(Util::HALF_WIDTH, Util::GetFormType(
      "\xef\xbf\xa8\xef\xbf\xa9\xef\xbf\xaa\xef\xbf\xab\xef\xbf\xac"
      "\xef\xbf\xad\xef\xbf\xae"));

  // Half-width mathematical symbols
  // [U+27E6, U+27ED], U+2985, and U+2986
  EXPECT_EQ(Util::HALF_WIDTH, Util::GetFormType(
      "\xe2\x9f\xa6\xe2\x9f\xa7\xe2\x9f\xa8\xe2\x9f\xa9\xe2\x9f\xaa\xe2"
      "\x9f\xab\xe2\x9f\xac\xe2\x9f\xad\xe2\xa6\x85\xe2\xa6\x86"));

  // Half-width hangul "ﾠﾡﾢ"
  EXPECT_EQ(Util::HALF_WIDTH, Util::GetFormType("\xef\xbe\xa0\xef\xbe\xa1"
                                                "\xef\xbe\xa2"));

  // Half-width won "₩"
  EXPECT_EQ(Util::HALF_WIDTH, Util::GetFormType("\xe2\x82\xa9"));

  EXPECT_EQ(Util::HALF_WIDTH, Util::GetFormType("012"));
  // "０１２012"
  EXPECT_EQ(Util::UNKNOWN_FORM, Util::GetFormType("\xef\xbc\x90\xef\xbc\x91\xef"
                                                  "\xbc\x92\x30\x31\x32"));
  EXPECT_EQ(Util::HALF_WIDTH, Util::GetFormType("abcABC"));
  // "ＡＢＣＤ"
  EXPECT_EQ(Util::FULL_WIDTH, Util::GetFormType("\xef\xbc\xa1\xef\xbc\xa2\xef"
                                                "\xbc\xa3\xef\xbc\xa4"));
  EXPECT_EQ(Util::HALF_WIDTH, Util::GetFormType("@!#"));
}

TEST(UtilTest, SplitFirstChar32) {
  StringPiece rest;
  char32 c = 0;

  rest = StringPiece();
  c = 0;
  EXPECT_FALSE(Util::SplitFirstChar32("", &c, &rest));
  EXPECT_EQ(0, c);
  EXPECT_TRUE(rest.empty());

  // Allow NULL to ignore the matched value.
  rest = StringPiece();
  EXPECT_TRUE(Util::SplitFirstChar32("01", NULL, &rest));
  EXPECT_EQ("1", rest);

  // Allow NULL to ignore the matched value.
  c = 0;
  EXPECT_TRUE(Util::SplitFirstChar32("01", &c, NULL));
  EXPECT_EQ('0', c);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitFirstChar32("\x01 ", &c, &rest));
  EXPECT_EQ(1, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitFirstChar32("\x7F ", &c, &rest));
  EXPECT_EQ(0x7F, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitFirstChar32("\xC2\x80 ", &c, &rest));
  EXPECT_EQ(0x80, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitFirstChar32("\xDF\xBF ", &c, &rest));
  EXPECT_EQ(0x7FF, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitFirstChar32("\xE0\xA0\x80 ", &c, &rest));
  EXPECT_EQ(0x800, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitFirstChar32("\xEF\xBF\xBF ", &c, &rest));
  EXPECT_EQ(0xFFFF, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitFirstChar32("\xF0\x90\x80\x80 ", &c, &rest));
  EXPECT_EQ(0x10000, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitFirstChar32("\xF7\xBF\xBF\xBF ", &c, &rest));
  EXPECT_EQ(0x1FFFFF, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitFirstChar32("\xF8\x88\x80\x80\x80 ", &c, &rest));
  EXPECT_EQ(0x200000, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitFirstChar32("\xFB\xBF\xBF\xBF\xBF ", &c, &rest));
  EXPECT_EQ(0x3FFFFFF, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitFirstChar32("\xFC\x84\x80\x80\x80\x80 ", &c, &rest));
  EXPECT_EQ(0x4000000, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitFirstChar32("\xFD\xBF\xBF\xBF\xBF\xBF ", &c, &rest));
  EXPECT_EQ(0x7FFFFFFF, c);
  EXPECT_EQ(" ", rest);

  // If there is any invalid sequence, the entire text should be treated as
  // am empty string.
  {
    c = 0;
    EXPECT_FALSE(Util::SplitFirstChar32("\xC2 ", &c, &rest));
    EXPECT_EQ(0, c);

    c = 0;
    EXPECT_FALSE(Util::SplitFirstChar32("\xC2\xC2 ", &c, &rest));
    EXPECT_EQ(0, c);

    c = 0;
    EXPECT_FALSE(Util::SplitFirstChar32("\xE0 ", &c, &rest));
    EXPECT_EQ(0, c);

    c = 0;
    EXPECT_FALSE(Util::SplitFirstChar32("\xE0\xE0\xE0 ", &c, &rest));
    EXPECT_EQ(0, c);

    c = 0;
    EXPECT_FALSE(Util::SplitFirstChar32("\xF0 ", &c, &rest));
    EXPECT_EQ(0, c);

    c = 0;
    EXPECT_FALSE(Util::SplitFirstChar32("\xF0\xF0\xF0\xF0 ", &c, &rest));
    EXPECT_EQ(0, c);
  }

  // BOM should be treated as invalid byte.
  {
    c = 0;
    EXPECT_FALSE(Util::SplitFirstChar32("\xFF ", &c, &rest));
    EXPECT_EQ(0, c);

    c = 0;
    EXPECT_FALSE(Util::SplitFirstChar32("\xFE ", &c, &rest));
    EXPECT_EQ(0, c);
  }

  // Invalid sequence for U+002F (redundant encoding)
  {
    c = 0;
    EXPECT_FALSE(Util::SplitFirstChar32("\xC0\xAF", &c, &rest));
    EXPECT_EQ(0, c);

    c = 0;
    EXPECT_FALSE(Util::SplitFirstChar32("\xE0\x80\xAF", &c, &rest));
    EXPECT_EQ(0, c);

    c = 0;
    EXPECT_FALSE(Util::SplitFirstChar32("\xF0\x80\x80\xAF", &c, &rest));
    EXPECT_EQ(0, c);
  }
}

TEST(UtilTest, SplitLastChar32) {
  StringPiece rest;
  char32 c = 0;

  rest = StringPiece();
  c = 0;
  EXPECT_FALSE(Util::SplitLastChar32("", &rest, &c));
  EXPECT_EQ(0, c);
  EXPECT_TRUE(rest.empty());

  // Allow NULL to ignore the matched value.
  c = 0;
  EXPECT_TRUE(Util::SplitLastChar32("01", NULL, &c));
  EXPECT_EQ('1', c);

  // Allow NULL to ignore the matched value.
  rest = StringPiece();
  EXPECT_TRUE(Util::SplitLastChar32("01", &rest, NULL));
  EXPECT_EQ("0", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitLastChar32(" \x01", &rest, &c));
  EXPECT_EQ(1, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitLastChar32(" \x7F", &rest, &c));
  EXPECT_EQ(0x7F, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitLastChar32(" \xC2\x80", &rest, &c));
  EXPECT_EQ(0x80, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitLastChar32(" \xDF\xBF", &rest, &c));
  EXPECT_EQ(0x7FF, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitLastChar32(" \xE0\xA0\x80", &rest, &c));
  EXPECT_EQ(0x800, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitLastChar32(" \xEF\xBF\xBF", &rest, &c));
  EXPECT_EQ(0xFFFF, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitLastChar32(" \xF0\x90\x80\x80", &rest, &c));
  EXPECT_EQ(0x10000, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitLastChar32(" \xF7\xBF\xBF\xBF", &rest, &c));
  EXPECT_EQ(0x1FFFFF, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitLastChar32(" \xF8\x88\x80\x80\x80", &rest, &c));
  EXPECT_EQ(0x200000, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitLastChar32(" \xFB\xBF\xBF\xBF\xBF", &rest, &c));
  EXPECT_EQ(0x3FFFFFF, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitLastChar32(" \xFC\x84\x80\x80\x80\x80", &rest, &c));
  EXPECT_EQ(0x4000000, c);
  EXPECT_EQ(" ", rest);

  rest = StringPiece();
  c = 0;
  EXPECT_TRUE(Util::SplitLastChar32(" \xFD\xBF\xBF\xBF\xBF\xBF", &rest, &c));
  EXPECT_EQ(0x7FFFFFFF, c);
  EXPECT_EQ(" ", rest);

  // If there is any invalid sequence, the entire text should be treated as
  // am empty string.
  {
    c = 0;
    EXPECT_FALSE(Util::SplitLastChar32(" \xC2", &rest, &c));
    EXPECT_EQ(0, c);

    c = 0;
    EXPECT_FALSE(Util::SplitLastChar32(" \xC2\xC2", &rest, &c));
    EXPECT_EQ(0, c);

    c = 0;
    EXPECT_FALSE(Util::SplitLastChar32(" \xE0", &rest, &c));
    EXPECT_EQ(0, c);

    c = 0;
    EXPECT_FALSE(Util::SplitLastChar32(" \xE0\xE0\xE0", &rest, &c));
    EXPECT_EQ(0, c);

    c = 0;
    EXPECT_FALSE(Util::SplitLastChar32(" \xF0", &rest, &c));
    EXPECT_EQ(0, c);

    c = 0;
    EXPECT_FALSE(Util::SplitLastChar32(" \xF0\xF0\xF0\xF0", &rest, &c));
    EXPECT_EQ(0, c);
  }

  // BOM should be treated as invalid byte.
  {
    c = 0;
    EXPECT_FALSE(Util::SplitLastChar32(" \xFF", &rest, &c));
    EXPECT_EQ(0, c);

    c = 0;
    EXPECT_FALSE(Util::SplitLastChar32(" \xFE", &rest, &c));
    EXPECT_EQ(0, c);
  }

  // Invalid sequence for U+002F (redundant encoding)
  {
    c = 0;
    EXPECT_FALSE(Util::SplitLastChar32("\xC0\xAF", &rest, &c));
    EXPECT_EQ(0, c);

    c = 0;
    EXPECT_FALSE(Util::SplitLastChar32("\xE0\x80\xAF", &rest, &c));
    EXPECT_EQ(0, c);

    c = 0;
    EXPECT_FALSE(Util::SplitLastChar32("\xF0\x80\x80\xAF", &rest, &c));
    EXPECT_EQ(0, c);
  }
}

}  // namespace gbase
