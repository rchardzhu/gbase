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

#include "base/number_util.h"

#include "base/port.h"
#include "gtest/gtest.h"

namespace gbase {
namespace {

TEST(NumberUtilTest, SimpleItoa) {
  EXPECT_EQ("0",   NumberUtil::SimpleItoa(0));
  EXPECT_EQ("123", NumberUtil::SimpleItoa(123));
  EXPECT_EQ("-1",  NumberUtil::SimpleItoa(-1));

  EXPECT_EQ("2147483647",  NumberUtil::SimpleItoa(kint32max));
  EXPECT_EQ("-2147483648", NumberUtil::SimpleItoa(kint32min));
  EXPECT_EQ("4294967295",  NumberUtil::SimpleItoa(kuint32max));

  EXPECT_EQ("9223372036854775807",  NumberUtil::SimpleItoa(kint64max));
  EXPECT_EQ("-9223372036854775808", NumberUtil::SimpleItoa(kint64min));
  EXPECT_EQ("18446744073709551615", NumberUtil::SimpleItoa(kuint64max));
}

TEST(NumberUtilTest, SimpleAtoi) {
  EXPECT_EQ(0, NumberUtil::SimpleAtoi("0"));
  EXPECT_EQ(123, NumberUtil::SimpleAtoi("123"));
  EXPECT_EQ(-1, NumberUtil::SimpleAtoi("-1"));
}

TEST(NumberUtilTest, SafeStrToInt16) {
  int16 value = 0x4321;

  EXPECT_TRUE(NumberUtil::SafeStrToInt16("0", &value));
  EXPECT_EQ(0, value);
  value = 0x4321;
  EXPECT_TRUE(NumberUtil::SafeStrToInt16("+0", &value));
  EXPECT_EQ(0, value);
  value = 0x4321;
  EXPECT_TRUE(NumberUtil::SafeStrToInt16("-0", &value));
  EXPECT_EQ(0, value);
  value = 0x4321;
  EXPECT_TRUE(NumberUtil::SafeStrToInt16(" \t\r\n\v\f0 \t\r\n\v\f", &value));
  EXPECT_EQ(0, value);
  value = 0x4321;
  EXPECT_TRUE(NumberUtil::SafeStrToInt16(" \t\r\n\v\f-0 \t\r\n\v\f", &value));
  EXPECT_EQ(0, value);
  value = 0x4321;
  EXPECT_TRUE(NumberUtil::SafeStrToInt16("012345", &value));
  EXPECT_EQ(12345, value);
  value = 0x4321;
  EXPECT_TRUE(NumberUtil::SafeStrToInt16("-012345", &value));
  EXPECT_EQ(-12345, value);
  value = 0x4321;
  EXPECT_TRUE(NumberUtil::SafeStrToInt16("-32768", &value));
  EXPECT_EQ(kint16min, value);  // min of 16-bit signed integer
  value = 0x4321;
  EXPECT_TRUE(NumberUtil::SafeStrToInt16("32767", &value));
  EXPECT_EQ(kint16max, value);  // max of 16-bit signed integer
  value = 0x4321;
  EXPECT_TRUE(NumberUtil::SafeStrToInt16(" 1", &value));
  EXPECT_EQ(1, value);
  value = 0x4321;
  EXPECT_TRUE(NumberUtil::SafeStrToInt16("2 ", &value));
  EXPECT_EQ(2, value);

  EXPECT_FALSE(NumberUtil::SafeStrToInt16("0x1234", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToInt16("-32769", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToInt16("32768", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToInt16("18446744073709551616", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToInt16("3e", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToInt16("0.", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToInt16(".0", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToInt16("", &value));

  // Test for StringPiece input.
  const char *kString = "123 abc 789";
  EXPECT_TRUE(NumberUtil::SafeStrToInt16(StringPiece(kString, 3),
                                         &value));
  EXPECT_EQ(123, value);
  EXPECT_FALSE(NumberUtil::SafeStrToInt16(StringPiece(kString + 4, 3),
                                          &value));
  EXPECT_TRUE(NumberUtil::SafeStrToInt16(StringPiece(kString + 8, 3),
                                         &value));
  EXPECT_EQ(789, value);
  EXPECT_TRUE(NumberUtil::SafeStrToInt16(StringPiece(kString + 7, 4),
                                         &value));
  EXPECT_EQ(789, value);
}

TEST(NumberUtilTest, SafeStrToInt32) {
  int32 value = 0xDEADBEEF;

  EXPECT_TRUE(NumberUtil::SafeStrToInt32("0", &value));
  EXPECT_EQ(0, value);
  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToInt32("+0", &value));
  EXPECT_EQ(0, value);
  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToInt32("-0", &value));
  EXPECT_EQ(0, value);
  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToInt32(" \t\r\n\v\f0 \t\r\n\v\f", &value));
  EXPECT_EQ(0, value);
  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToInt32(" \t\r\n\v\f-0 \t\r\n\v\f", &value));
  EXPECT_EQ(0, value);
  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToInt32("012345678", &value));
  EXPECT_EQ(12345678, value);
  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToInt32("-012345678", &value));
  EXPECT_EQ(-12345678, value);
  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToInt32("-2147483648", &value));
  EXPECT_EQ(kint32min, value);  // min of 32-bit signed integer
  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToInt32("2147483647", &value));
  EXPECT_EQ(kint32max, value);  // max of 32-bit signed integer
  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToInt32(" 1", &value));
  EXPECT_EQ(1, value);
  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToInt32("2 ", &value));
  EXPECT_EQ(2, value);

  EXPECT_FALSE(NumberUtil::SafeStrToInt32("0x1234", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToInt32("-2147483649", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToInt32("2147483648", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToInt32("18446744073709551616", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToInt32("3e", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToInt32("0.", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToInt32(".0", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToInt32("", &value));

  // Test for StringPiece input.
  const char *kString = "123 abc 789";
  EXPECT_TRUE(NumberUtil::SafeStrToInt32(StringPiece(kString, 3),
                                         &value));
  EXPECT_EQ(123, value);
  EXPECT_FALSE(NumberUtil::SafeStrToInt32(StringPiece(kString + 4, 3),
                                          &value));
  EXPECT_TRUE(NumberUtil::SafeStrToInt32(StringPiece(kString + 8, 3),
                                         &value));
  EXPECT_EQ(789, value);
  EXPECT_TRUE(NumberUtil::SafeStrToInt32(StringPiece(kString + 7, 4),
                                         &value));
  EXPECT_EQ(789, value);
}

TEST(NumberUtilTest, SafeStrToInt64) {
  int64 value = 0xDEADBEEF;

  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToInt64("0", &value));
  EXPECT_EQ(0, value);
  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToInt64("+0", &value));
  EXPECT_EQ(0, value);
  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToInt64("-0", &value));
  EXPECT_EQ(0, value);
  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToInt64(" \t\r\n\v\f0 \t\r\n\v\f", &value));
  EXPECT_EQ(0, value);
  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToInt64(" \t\r\n\v\f-0 \t\r\n\v\f", &value));
  EXPECT_EQ(0, value);
  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToInt64("012345678", &value));
  EXPECT_EQ(12345678, value);
  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToInt64("-012345678", &value));
  EXPECT_EQ(-12345678, value);
  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToInt64("-9223372036854775808", &value));
  EXPECT_EQ(kint64min, value);  // min of 64-bit signed integer
  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToInt64("9223372036854775807", &value));
  EXPECT_EQ(kint64max, value);  // max of 64-bit signed integer

  EXPECT_FALSE(NumberUtil::SafeStrToInt64("-9223372036854775809",  // overflow
                                          &value));
  EXPECT_FALSE(NumberUtil::SafeStrToInt64("9223372036854775808",  // overflow
                                          &value));
  EXPECT_FALSE(NumberUtil::SafeStrToInt64("0x1234", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToInt64("3e", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToInt64("0.", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToInt64(".0", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToInt64("", &value));

  // Test for StringPiece input.
  const char *kString = "123 abc 789";
  EXPECT_TRUE(NumberUtil::SafeStrToInt64(StringPiece(kString, 3),
                                         &value));
  EXPECT_EQ(123, value);
  EXPECT_FALSE(NumberUtil::SafeStrToInt64(StringPiece(kString + 4, 3),
                                          &value));
  EXPECT_TRUE(NumberUtil::SafeStrToInt64(StringPiece(kString + 8, 3),
                                         &value));
  EXPECT_EQ(789, value);
}

TEST(NumberUtilTest, SafeStrToUInt16) {
  uint16 value = 0xBEEF;

  EXPECT_TRUE(NumberUtil::SafeStrToUInt16("0", &value));
  EXPECT_EQ(0, value);

  value = 0xBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToUInt16(" \t\r\n\v\f0 \t\r\n\v\f", &value));
  EXPECT_EQ(0, value);

  value = 0xBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToUInt16("012345", &value));
  EXPECT_EQ(12345, value);

  value = 0xBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToUInt16("65535", &value));
  EXPECT_EQ(65535u, value);  // max of 16-bit unsigned integer

  value = 0xBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToUInt16(" 1", &value));
  EXPECT_EQ(1, value);

  value = 0xBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToUInt16("2 ", &value));
  EXPECT_EQ(2, value);

  EXPECT_FALSE(NumberUtil::SafeStrToUInt16("-0", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToUInt16("0x1234", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToUInt16("65536", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToUInt16("18446744073709551616", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToUInt16("3e", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToUInt16("0.", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToUInt16(".0", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToUInt16("", &value));

  // Test for StringPiece input.
  const char *kString = "123 abc 789";
  EXPECT_TRUE(NumberUtil::SafeStrToUInt16(StringPiece(kString, 3),
                                          &value));
  EXPECT_EQ(123, value);
  EXPECT_FALSE(NumberUtil::SafeStrToUInt16(StringPiece(kString + 4, 3),
                                           &value));
  EXPECT_TRUE(NumberUtil::SafeStrToUInt16(StringPiece(kString + 8, 3),
                                          &value));
  EXPECT_EQ(789, value);
  EXPECT_TRUE(NumberUtil::SafeStrToUInt16(StringPiece(kString + 7, 4),
                                          &value));
  EXPECT_EQ(789, value);
}

TEST(NumberUtilTest, SafeStrToUInt32) {
  uint32 value = 0xDEADBEEF;

  EXPECT_TRUE(NumberUtil::SafeStrToUInt32("0", &value));
  EXPECT_EQ(0, value);

  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToUInt32(" \t\r\n\v\f0 \t\r\n\v\f", &value));
  EXPECT_EQ(0, value);

  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToUInt32("012345678", &value));
  EXPECT_EQ(12345678, value);

  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToUInt32("4294967295", &value));
  EXPECT_EQ(4294967295u, value);  // max of 32-bit unsigned integer

  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToUInt32(" 1", &value));
  EXPECT_EQ(1, value);

  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeStrToUInt32("2 ", &value));
  EXPECT_EQ(2, value);

  EXPECT_FALSE(NumberUtil::SafeStrToUInt32("-0", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToUInt32("0x1234", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToUInt32("4294967296", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToUInt32("18446744073709551616", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToUInt32("3e", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToUInt32("0.", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToUInt32(".0", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToUInt32("", &value));

  // Test for StringPiece input.
  const char *kString = "123 abc 789";
  EXPECT_TRUE(NumberUtil::SafeStrToUInt32(StringPiece(kString, 3),
                                          &value));
  EXPECT_EQ(123, value);
  EXPECT_FALSE(NumberUtil::SafeStrToUInt32(StringPiece(kString + 4, 3),
                                           &value));
  EXPECT_TRUE(NumberUtil::SafeStrToUInt32(StringPiece(kString + 8, 3),
                                          &value));
  EXPECT_EQ(789, value);
  EXPECT_TRUE(NumberUtil::SafeStrToUInt32(StringPiece(kString + 7, 4),
                                          &value));
  EXPECT_EQ(789, value);
}

TEST(NumberUtilTest, SafeHexStrToUInt32) {
  uint32 value = 0xDEADBEEF;

  EXPECT_TRUE(NumberUtil::SafeHexStrToUInt32("0", &value));
  EXPECT_EQ(0, value);

  value = 0xDEADBEEF;
  EXPECT_TRUE(
      NumberUtil::SafeHexStrToUInt32(" \t\r\n\v\f0 \t\r\n\v\f", &value));
  EXPECT_EQ(0, value);

  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeHexStrToUInt32("0ABCDE", &value));
  EXPECT_EQ(0xABCDE, value);

  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeHexStrToUInt32("0abcde", &value));
  EXPECT_EQ(0xABCDE, value);

  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeHexStrToUInt32("FFFFFFFF", &value));
  EXPECT_EQ(0xFFFFFFFF, value);  // max of 32-bit unsigned integer

  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeHexStrToUInt32("ffffffff", &value));
  EXPECT_EQ(0xFFFFFFFF, value);  // max of 32-bit unsigned integer

  EXPECT_FALSE(NumberUtil::SafeHexStrToUInt32("-0", &value));
  EXPECT_FALSE(
      NumberUtil::SafeHexStrToUInt32("100000000", &value));  // overflow
  EXPECT_FALSE(NumberUtil::SafeHexStrToUInt32("GHIJK", &value));
  EXPECT_FALSE(NumberUtil::SafeHexStrToUInt32("0.", &value));
  EXPECT_FALSE(NumberUtil::SafeHexStrToUInt32(".0", &value));
  EXPECT_FALSE(NumberUtil::SafeHexStrToUInt32("", &value));

  // Test for StringPiece input.
  const char *kString = "123 abc 5x";
  EXPECT_TRUE(NumberUtil::SafeHexStrToUInt32(StringPiece(kString, 3),
                                             &value));
  EXPECT_EQ(291, value);
  EXPECT_TRUE(NumberUtil::SafeHexStrToUInt32(StringPiece(kString + 4, 3),
                                             &value));
  EXPECT_EQ(2748, value);
  EXPECT_FALSE(NumberUtil::SafeHexStrToUInt32(StringPiece(kString + 8, 2),
                                              &value));
}

TEST(NumberUtilTest, SafeOctStrToUInt32) {
  uint32 value = 0xDEADBEEF;

  EXPECT_TRUE(NumberUtil::SafeOctStrToUInt32("0", &value));
  EXPECT_EQ(0, value);

  value = 0xDEADBEEF;
  EXPECT_TRUE(
      NumberUtil::SafeOctStrToUInt32(" \t\r\n\v\f0 \t\r\n\v\f", &value));
  EXPECT_EQ(0, value);

  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeOctStrToUInt32("012345", &value));
  EXPECT_EQ(012345, value);

  value = 0xDEADBEEF;
  EXPECT_TRUE(NumberUtil::SafeOctStrToUInt32("37777777777", &value));
  EXPECT_EQ(0xFFFFFFFF, value);  // max of 32-bit unsigned integer

  EXPECT_FALSE(NumberUtil::SafeOctStrToUInt32("-0", &value));
  EXPECT_FALSE(
      NumberUtil::SafeOctStrToUInt32("40000000000", &value));  // overflow
  EXPECT_FALSE(NumberUtil::SafeOctStrToUInt32("9AB", &value));
  EXPECT_FALSE(NumberUtil::SafeOctStrToUInt32("0.", &value));
  EXPECT_FALSE(NumberUtil::SafeOctStrToUInt32(".0", &value));
  EXPECT_FALSE(NumberUtil::SafeOctStrToUInt32("", &value));

  // Test for StringPiece input.
  const char *kString = "123 456 789";
  EXPECT_TRUE(NumberUtil::SafeOctStrToUInt32(StringPiece(kString, 3),
                                             &value));
  EXPECT_EQ(83, value);
  EXPECT_TRUE(NumberUtil::SafeOctStrToUInt32(StringPiece(kString + 4, 3),
                                             &value));
  EXPECT_EQ(302, value);
  EXPECT_FALSE(NumberUtil::SafeOctStrToUInt32(StringPiece(kString + 8, 3),
                                              &value));
}

TEST(NumberUtilTest, SafeStrToUInt64) {
  uint64 value = 0xDEADBEEF;

  EXPECT_TRUE(NumberUtil::SafeStrToUInt64("0", &value));
  EXPECT_EQ(0, value);
  EXPECT_TRUE(NumberUtil::SafeStrToUInt64(" \t\r\n\v\f0 \t\r\n\v\f", &value));
  EXPECT_EQ(0, value);
  EXPECT_TRUE(NumberUtil::SafeStrToUInt64("012345678", &value));
  EXPECT_EQ(12345678, value);
  EXPECT_TRUE(NumberUtil::SafeStrToUInt64("18446744073709551615", &value));
  EXPECT_EQ(18446744073709551615ull, value);  // max of 64-bit unsigned integer

  EXPECT_FALSE(NumberUtil::SafeStrToUInt64("-0", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToUInt64("18446744073709551616",  // overflow
                                     &value));
  EXPECT_FALSE(NumberUtil::SafeStrToUInt64("0x1234", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToUInt64("3e", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToUInt64("0.", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToUInt64(".0", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToUInt64("", &value));

  // Test for StringPiece input.
  const char *kString = "123 abc 789";
  EXPECT_TRUE(NumberUtil::SafeStrToUInt64(StringPiece(kString, 3),
                                          &value));
  EXPECT_EQ(123, value);
  EXPECT_FALSE(NumberUtil::SafeStrToUInt64(StringPiece(kString + 4, 3),
                                           &value));
  EXPECT_TRUE(NumberUtil::SafeStrToUInt64(StringPiece(kString + 8, 3),
                                          &value));
  EXPECT_EQ(789, value);
}

TEST(NumberUtilTest, SafeStrToDouble) {
  double value = 1.0;

  EXPECT_TRUE(NumberUtil::SafeStrToDouble("0", &value));
  EXPECT_EQ(0.0, value);
  EXPECT_TRUE(NumberUtil::SafeStrToDouble(" \t\r\n\v\f0 \t\r\n\v\f", &value));
  EXPECT_EQ(0.0, value);
  EXPECT_TRUE(NumberUtil::SafeStrToDouble("-0", &value));
  EXPECT_EQ(0.0, value);
  EXPECT_TRUE(NumberUtil::SafeStrToDouble("1.0e1", &value));
  EXPECT_EQ(10.0, value);
  EXPECT_TRUE(NumberUtil::SafeStrToDouble("-5.0e-1", &value));
  EXPECT_EQ(-0.5, value);
  EXPECT_TRUE(NumberUtil::SafeStrToDouble(".0", &value));
  EXPECT_EQ(0.0, value);
  EXPECT_TRUE(NumberUtil::SafeStrToDouble("0.", &value));
  EXPECT_EQ(0.0, value);
  EXPECT_TRUE(NumberUtil::SafeStrToDouble("0.0", &value));
  EXPECT_EQ(0.0, value);
  EXPECT_TRUE(NumberUtil::SafeStrToDouble("1.7976931348623158e308", &value));
  EXPECT_EQ(1.7976931348623158e308, value);  // approximated representation
                                             // of max of double
  EXPECT_TRUE(NumberUtil::SafeStrToDouble("-1.7976931348623158e308", &value));
  EXPECT_EQ(-1.7976931348623158e308, value);

  EXPECT_TRUE(NumberUtil::SafeStrToDouble("0x1234", &value));
  EXPECT_EQ(static_cast<double>(0x1234), value);

  EXPECT_FALSE(
      NumberUtil::SafeStrToDouble("1.7976931348623159e308",
                                  &value));  // overflow
  EXPECT_FALSE(NumberUtil::SafeStrToDouble("-1.7976931348623159e308", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToDouble("NaN", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToDouble("3e", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToDouble(".", &value));
  EXPECT_FALSE(NumberUtil::SafeStrToDouble("", &value));

  // Test for StringPiece input.
  const char *kString = "0.01 3.1415 double";
  EXPECT_TRUE(NumberUtil::SafeStrToDouble(StringPiece(kString, 4),
                                          &value));
  EXPECT_EQ(0.01, value);
  EXPECT_TRUE(NumberUtil::SafeStrToDouble(StringPiece(kString + 5, 6),
                                          &value));
  EXPECT_EQ(3.1415, value);
  EXPECT_FALSE(NumberUtil::SafeStrToDouble(StringPiece(kString + 12, 6),
                                           &value));
}

TEST(NumberUtilTest, IsArabicNumber) {
  EXPECT_FALSE(NumberUtil::IsArabicNumber(""));

  EXPECT_TRUE(NumberUtil::IsArabicNumber("0"));
  EXPECT_TRUE(NumberUtil::IsArabicNumber("1"));
  EXPECT_TRUE(NumberUtil::IsArabicNumber("2"));
  EXPECT_TRUE(NumberUtil::IsArabicNumber("3"));
  EXPECT_TRUE(NumberUtil::IsArabicNumber("4"));
  EXPECT_TRUE(NumberUtil::IsArabicNumber("5"));
  EXPECT_TRUE(NumberUtil::IsArabicNumber("6"));
  EXPECT_TRUE(NumberUtil::IsArabicNumber("7"));
  EXPECT_TRUE(NumberUtil::IsArabicNumber("8"));
  EXPECT_TRUE(NumberUtil::IsArabicNumber("9"));

  EXPECT_TRUE(NumberUtil::IsArabicNumber("\xEF\xBC\x90"));  // ０
  EXPECT_TRUE(NumberUtil::IsArabicNumber("\xEF\xBC\x91"));  // １
  EXPECT_TRUE(NumberUtil::IsArabicNumber("\xEF\xBC\x92"));  // ２
  EXPECT_TRUE(NumberUtil::IsArabicNumber("\xEF\xBC\x93"));  // ３
  EXPECT_TRUE(NumberUtil::IsArabicNumber("\xEF\xBC\x94"));  // ４
  EXPECT_TRUE(NumberUtil::IsArabicNumber("\xEF\xBC\x95"));  // ５
  EXPECT_TRUE(NumberUtil::IsArabicNumber("\xEF\xBC\x96"));  // ６
  EXPECT_TRUE(NumberUtil::IsArabicNumber("\xEF\xBC\x97"));  // ７
  EXPECT_TRUE(NumberUtil::IsArabicNumber("\xEF\xBC\x98"));  // ８
  EXPECT_TRUE(NumberUtil::IsArabicNumber("\xEF\xBC\x99"));  // ９

  EXPECT_TRUE(NumberUtil::IsArabicNumber("0123456789"));
  EXPECT_TRUE(NumberUtil::IsArabicNumber("01234567890123456789"));

  EXPECT_TRUE(NumberUtil::IsArabicNumber("\xEF\xBC\x91\xEF\xBC\x90"));  // １０

  EXPECT_FALSE(NumberUtil::IsArabicNumber("abc"));
  EXPECT_FALSE(NumberUtil::IsArabicNumber("\xe5\x8d\x81"));  // 十
  EXPECT_FALSE(NumberUtil::IsArabicNumber("\xe5\x84\x84"));  // 億
  // グーグル
  EXPECT_FALSE(NumberUtil::IsArabicNumber(
      "\xe3\x82\xb0\xe3\x83\xbc\xe3\x82\xb0\xe3\x83\xab"));
}

TEST(NumberUtilTest, IsDecimalInteger) {
  EXPECT_FALSE(NumberUtil::IsDecimalInteger(""));

  EXPECT_TRUE(NumberUtil::IsDecimalInteger("0"));
  EXPECT_TRUE(NumberUtil::IsDecimalInteger("1"));
  EXPECT_TRUE(NumberUtil::IsDecimalInteger("2"));
  EXPECT_TRUE(NumberUtil::IsDecimalInteger("3"));
  EXPECT_TRUE(NumberUtil::IsDecimalInteger("4"));
  EXPECT_TRUE(NumberUtil::IsDecimalInteger("5"));
  EXPECT_TRUE(NumberUtil::IsDecimalInteger("6"));
  EXPECT_TRUE(NumberUtil::IsDecimalInteger("7"));
  EXPECT_TRUE(NumberUtil::IsDecimalInteger("8"));
  EXPECT_TRUE(NumberUtil::IsDecimalInteger("9"));
  EXPECT_TRUE(NumberUtil::IsDecimalInteger("0123456789"));
  EXPECT_TRUE(NumberUtil::IsDecimalInteger("01234567890123456789"));

  EXPECT_FALSE(NumberUtil::IsDecimalInteger("\xEF\xBC\x90"));  // ０
  EXPECT_FALSE(NumberUtil::IsDecimalInteger("\xEF\xBC\x91"));  // １
  EXPECT_FALSE(NumberUtil::IsDecimalInteger("\xEF\xBC\x92"));  // ２
  EXPECT_FALSE(NumberUtil::IsDecimalInteger("\xEF\xBC\x93"));  // ３
  EXPECT_FALSE(NumberUtil::IsDecimalInteger("\xEF\xBC\x94"));  // ４
  EXPECT_FALSE(NumberUtil::IsDecimalInteger("\xEF\xBC\x95"));  // ５
  EXPECT_FALSE(NumberUtil::IsDecimalInteger("\xEF\xBC\x96"));  // ６
  EXPECT_FALSE(NumberUtil::IsDecimalInteger("\xEF\xBC\x97"));  // ７
  EXPECT_FALSE(NumberUtil::IsDecimalInteger("\xEF\xBC\x98"));  // ８
  EXPECT_FALSE(NumberUtil::IsDecimalInteger("\xEF\xBC\x99"));  // ９

  // １０
  EXPECT_FALSE(NumberUtil::IsDecimalInteger("\xEF\xBC\x91\xEF\xBC\x90"));
  EXPECT_FALSE(NumberUtil::IsDecimalInteger("\xe5\x8d\x81"));  // 十
  EXPECT_FALSE(NumberUtil::IsDecimalInteger("\xe5\x84\x84"));  // 億
  EXPECT_FALSE(NumberUtil::IsDecimalInteger("abc"));
  // グーグル
  EXPECT_FALSE(NumberUtil::IsDecimalInteger(
      "\xe3\x82\xb0\xe3\x83\xbc\xe3\x82\xb0\xe3\x83\xab"));
}

TEST(NumberUtilTest, ArabicToWideArabicTest) {
  string arabic;
  std::vector<NumberUtil::NumberString> output;

  arabic = "12345";
  output.clear();
  EXPECT_TRUE(NumberUtil::ArabicToWideArabic(arabic, &output));
  ASSERT_EQ(output.size(), 2);
  // "１２３４５"
  EXPECT_EQ("\xE4\xB8\x80\xE4\xBA\x8C"
            "\xE4\xB8\x89\xE5\x9B\x9B\xE4\xBA\x94", output[0].value);
  EXPECT_EQ(NumberUtil::NumberString::NUMBER_KANJI_ARABIC, output[0].style);
  // "一二三四五"
  EXPECT_EQ("\xEF\xBC\x91\xEF\xBC\x92"
            "\xEF\xBC\x93\xEF\xBC\x94\xEF\xBC\x95", output[1].value);
  EXPECT_EQ(NumberUtil::NumberString::DEFAULT_STYLE, output[1].style);

  arabic = "00123";
  output.clear();
  EXPECT_TRUE(NumberUtil::ArabicToWideArabic(arabic, &output));
  ASSERT_EQ(output.size(), 2);
  // "００１２３"
  EXPECT_EQ("\xE3\x80\x87\xE3\x80\x87"
            "\xE4\xB8\x80\xE4\xBA\x8C\xE4\xB8\x89", output[0].value);
  EXPECT_EQ(NumberUtil::NumberString::NUMBER_KANJI_ARABIC, output[0].style);
  // "〇〇一二三"
  EXPECT_EQ("\xEF\xBC\x90\xEF\xBC\x90"
            "\xEF\xBC\x91\xEF\xBC\x92\xEF\xBC\x93", output[1].value);
  EXPECT_EQ(NumberUtil::NumberString::DEFAULT_STYLE, output[1].style);

  arabic = "abcde";
  output.clear();
  EXPECT_FALSE(NumberUtil::ArabicToWideArabic(arabic, &output));
  EXPECT_EQ(output.size(), 0);

  arabic = "012abc345";
  output.clear();
  EXPECT_FALSE(NumberUtil::ArabicToWideArabic(arabic, &output));
  EXPECT_EQ(output.size(), 0);

  arabic = "0.001";
  output.clear();
  EXPECT_FALSE(NumberUtil::ArabicToWideArabic(arabic, &output));
  EXPECT_EQ(output.size(), 0);

  arabic = "-100";
  output.clear();
  EXPECT_FALSE(NumberUtil::ArabicToWideArabic(arabic, &output));
  EXPECT_EQ(output.size(), 0);

  arabic = "18446744073709551616";  // UINT_MAX + 1
  EXPECT_TRUE(NumberUtil::ArabicToWideArabic(arabic, &output));
  // "１８４４６７４４０７３７０９５５１６１６"
  EXPECT_EQ("\xE4\xB8\x80\xE5\x85\xAB\xE5\x9B\x9B\xE5\x9B\x9B\xE5\x85"
            "\xAD\xE4\xB8\x83\xE5\x9B\x9B\xE5\x9B\x9B\xE3\x80\x87\xE4"
            "\xB8\x83\xE4\xB8\x89\xE4\xB8\x83\xE3\x80\x87\xE4\xB9\x9D"
            "\xE4\xBA\x94\xE4\xBA\x94\xE4\xB8\x80\xE5\x85\xAD\xE4\xB8"
            "\x80\xE5\x85\xAD",
            output[0].value);
  EXPECT_EQ(NumberUtil::NumberString::NUMBER_KANJI_ARABIC, output[0].style);
}

namespace {
const int kMaxCandsInArabicToKanjiTest = 4;
struct ArabicToKanjiTestData {
  const char *input;
  const int expect_num;
  const char *expect_value[kMaxCandsInArabicToKanjiTest];
  const NumberUtil::NumberString::Style
  expect_style[kMaxCandsInArabicToKanjiTest];
};
}  // namespace

// ArabicToKanji TEST
TEST(NumberUtilTest, ArabicToKanjiTest) {
  const NumberUtil::NumberString::Style kOldKanji =
      NumberUtil::NumberString::NUMBER_OLD_KANJI;
  const NumberUtil::NumberString::Style kKanji =
      NumberUtil::NumberString::NUMBER_KANJI;
  const NumberUtil::NumberString::Style kHalfArabicKanji =
      NumberUtil::NumberString::NUMBER_ARABIC_AND_KANJI_HALFWIDTH;
  const NumberUtil::NumberString::Style kFullArabicKanji =
      NumberUtil::NumberString::NUMBER_ARABIC_AND_KANJI_FULLWIDTH;

  const ArabicToKanjiTestData kData[] = {
    // "零"
    {"0", 1, {"\xE9\x9B\xB6"}, {kOldKanji}},
    // "零"
    {"00000", 1, {"\xE9\x9B\xB6"}, {kOldKanji}},
    // "二", "弐"
    {"2", 2, {"\xE4\xBA\x8C", "\xE5\xBC\x90"}, {kKanji, kOldKanji}},
    // "壱拾" is needed to avoid mistakes. Please refer http://b/6422355
    // for details.
    // "十", "壱拾", "拾"
    {"10", 3, {"\xE5\x8D\x81", "\xE5\xA3\xB1\xE6\x8B\xBE", "\xE6\x8B\xBE"},
     {kKanji, kOldKanji, kOldKanji}},
    // "百", "壱百"
    {"100", 2, {"\xE7\x99\xBE", "\xE5\xA3\xB1\xE7\x99\xBE"},
     {kKanji, kOldKanji}},
    // "千", "壱阡", "阡"
    {"1000", 3, {"\xE5\x8D\x83", "\xE5\xA3\xB1\xE9\x98\xA1", "\xE9\x98\xA1"},
     {kKanji, kOldKanji, kOldKanji}},
    {"20", 3,
     // "二十", "弐拾", "廿"
     {"\xE4\xBA\x8C\xE5\x8D\x81", "\xE5\xBC\x90\xE6\x8B\xBE", "\xE5\xBB\xBF"},
     {kKanji, kOldKanji, kOldKanji}},
    {"11111", 4,
     // "1万1111"
     {"1" "\xE4\xB8\x87" "1111",
      // "１万１１１１"
      "\xEF\xBC\x91\xE4\xB8\x87\xEF\xBC\x91\xEF\xBC\x91\xEF\xBC\x91"
      "\xEF\xBC\x91",
      // "一万千百十一"
      "\xE4\xB8\x80\xE4\xB8\x87\xE5\x8D\x83\xE7\x99\xBE\xE5\x8D\x81"
      "\xE4\xB8\x80",
      // "壱萬壱阡壱百壱拾壱"
      "\xE5\xA3\xB1\xE8\x90\xAC\xE5\xA3\xB1\xE9\x98\xA1\xE5\xA3\xB1"
      "\xE7\x99\xBE\xE5\xA3\xB1\xE6\x8B\xBE\xE5\xA3\xB1"},
     {kHalfArabicKanji, kFullArabicKanji, kKanji, kOldKanji}},
    {"12345", 4,
     // "1万2345"
     {"1" "\xE4\xB8\x87" "2345",
      // "１万２３４５"
      "\xEF\xBC\x91\xE4\xB8\x87\xEF\xBC\x92\xEF\xBC\x93\xEF\xBC\x94"
      "\xEF\xBC\x95",
      // "一万二千三百四十五"
      "\xE4\xB8\x80\xE4\xB8\x87\xE4\xBA\x8C\xE5\x8D\x83\xE4\xB8\x89"
      "\xE7\x99\xBE\xE5\x9B\x9B\xE5\x8D\x81\xE4\xBA\x94",
      // "壱萬弐阡参百四拾五"
      "\xE5\xA3\xB1\xE8\x90\xAC\xE5\xBC\x90\xE9\x98\xA1\xE5\x8F\x82"
      "\xE7\x99\xBE\xE5\x9B\x9B\xE6\x8B\xBE\xE4\xBA\x94"},
     {kHalfArabicKanji, kFullArabicKanji, kKanji, kOldKanji}},
    {"100002345", 4,
     // "1億2345"
     {"1" "\xE5\x84\x84" "2345",
      // "１億２３４５"
      "\xEF\xBC\x91\xE5\x84\x84\xEF\xBC\x92\xEF\xBC\x93\xEF\xBC\x94"
      "\xEF\xBC\x95",
      // "一億二千三百四十五"
      "\xE4\xB8\x80\xE5\x84\x84\xE4\xBA\x8C\xE5\x8D\x83\xE4\xB8\x89"
      "\xE7\x99\xBE\xE5\x9B\x9B\xE5\x8D\x81\xE4\xBA\x94",
      // "壱億弐阡参百四拾五"
      "\xE5\xA3\xB1\xE5\x84\x84\xE5\xBC\x90\xE9\x98\xA1\xE5\x8F\x82"
      "\xE7\x99\xBE\xE5\x9B\x9B\xE6\x8B\xBE\xE4\xBA\x94"},
     {kHalfArabicKanji, kFullArabicKanji, kKanji, kOldKanji}},
    {"18446744073709551615", 4,
     // "1844京6744兆737億955万1615"
     {"1844" "\xE4\xBA\xAC" "6744" "\xE5\x85\x86" "737" "\xE5\x84\x84"
      "955" "\xE4\xB8\x87" "1615",
      // "１８４４京６７４４兆７３７億９５５万１６１５"
      "\xEF\xBC\x91\xEF\xBC\x98\xEF\xBC\x94\xEF\xBC\x94\xE4\xBA\xAC"
      "\xEF\xBC\x96\xEF\xBC\x97\xEF\xBC\x94\xEF\xBC\x94\xE5\x85\x86"
      "\xEF\xBC\x97\xEF\xBC\x93\xEF\xBC\x97\xE5\x84\x84\xEF\xBC\x99"
      "\xEF\xBC\x95\xEF\xBC\x95\xE4\xB8\x87\xEF\xBC\x91\xEF\xBC\x96"
      "\xEF\xBC\x91\xEF\xBC\x95",
      // "千八百四十四京六千七百四十四兆七百三十七億九百五十五万千六百十五"
      "\xE5\x8D\x83\xE5\x85\xAB\xE7\x99\xBE\xE5\x9B\x9B\xE5\x8D\x81"
      "\xE5\x9B\x9B\xE4\xBA\xAC\xE5\x85\xAD\xE5\x8D\x83\xE4\xB8\x83"
      "\xE7\x99\xBE\xE5\x9B\x9B\xE5\x8D\x81\xE5\x9B\x9B\xE5\x85\x86"
      "\xE4\xB8\x83\xE7\x99\xBE\xE4\xB8\x89\xE5\x8D\x81\xE4\xB8\x83"
      "\xE5\x84\x84\xE4\xB9\x9D\xE7\x99\xBE\xE4\xBA\x94\xE5\x8D\x81"
      "\xE4\xBA\x94\xE4\xB8\x87\xE5\x8D\x83\xE5\x85\xAD\xE7\x99\xBE"
      "\xE5\x8D\x81\xE4\xBA\x94",
      // "壱阡八百四拾四京六阡七百四拾四兆七百参拾七億九百五拾五萬"
      // "壱阡六百壱拾五"
      "\xE5\xA3\xB1\xE9\x98\xA1\xE5\x85\xAB\xE7\x99\xBE\xE5\x9B\x9B"
      "\xE6\x8B\xBE\xE5\x9B\x9B\xE4\xBA\xAC\xE5\x85\xAD\xE9\x98\xA1"
      "\xE4\xB8\x83\xE7\x99\xBE\xE5\x9B\x9B\xE6\x8B\xBE\xE5\x9B\x9B"
      "\xE5\x85\x86\xE4\xB8\x83\xE7\x99\xBE\xE5\x8F\x82\xE6\x8B\xBE"
      "\xE4\xB8\x83\xE5\x84\x84\xE4\xB9\x9D\xE7\x99\xBE\xE4\xBA\x94"
      "\xE6\x8B\xBE\xE4\xBA\x94\xE8\x90\xAC\xE5\xA3\xB1\xE9\x98\xA1"
      "\xE5\x85\xAD\xE7\x99\xBE\xE5\xA3\xB1\xE6\x8B\xBE\xE4\xBA\x94"},
     {kHalfArabicKanji, kFullArabicKanji, kKanji, kOldKanji}},
  };

  for (size_t i = 0; i < arraysize(kData); ++i) {
    std::vector<NumberUtil::NumberString> output;
    ASSERT_LE(kData[i].expect_num, kMaxCandsInArabicToKanjiTest);
    EXPECT_TRUE(NumberUtil::ArabicToKanji(kData[i].input, &output));
    ASSERT_EQ(output.size(), kData[i].expect_num)
        << "on conversion of '" << kData[i].input << "'";
    for (int j = 0; j < kData[i].expect_num; ++j) {
      EXPECT_EQ(kData[i].expect_value[j], output[j].value)
          << "input : " << kData[i].input << "\nj : " << j;
      EXPECT_EQ(kData[i].expect_style[j], output[j].style)
          << "input : " << kData[i].input << "\nj : " << j;
    }
  }

  const char *kFailInputs[] = {
    "asf56789", "0.001", "-100", "123456789012345678901"
  };
  for (size_t i = 0; i < arraysize(kFailInputs); ++i) {
    std::vector<NumberUtil::NumberString> output;
    EXPECT_FALSE(NumberUtil::ArabicToKanji(kFailInputs[i], &output));
    ASSERT_EQ(output.size(), 0) << "input : " << kFailInputs[i];
  }
}

// ArabicToSeparatedArabic TEST
TEST(NumberUtilTest, ArabicToSeparatedArabicTest) {
  string arabic;
  std::vector<NumberUtil::NumberString> output;

  // Test data expected to succeed
  const char* kSuccess[][3] = {
    // "４"
    {"4", "4", "\xEF\xBC\x94"},
    // "１２３，４５６，７８９"
    {"123456789", "123,456,789", "\xEF\xBC\x91\xEF\xBC\x92\xEF\xBC\x93"
     "\xEF\xBC\x8C\xEF\xBC\x94\xEF\xBC\x95\xEF\xBC\x96\xEF\xBC\x8C"
     "\xEF\xBC\x97\xEF\xBC\x98\xEF\xBC\x99"},
    // "１，２３４，５６７．８９"
    {"1234567.89", "1,234,567.89", "\xEF\xBC\x91\xEF\xBC\x8C\xEF\xBC\x92"
     "\xEF\xBC\x93\xEF\xBC\x94\xEF\xBC\x8C\xEF\xBC\x95\xEF\xBC\x96"
     "\xEF\xBC\x97\xEF\xBC\x8E\xEF\xBC\x98\xEF\xBC\x99"},
    // UINT64_MAX + 1
    {"18446744073709551616", "18,446,744,073,709,551,616", nullptr},
  };

  for (size_t i = 0; i < arraysize(kSuccess); ++i) {
    arabic = kSuccess[i][0];
    output.clear();
    EXPECT_TRUE(NumberUtil::ArabicToSeparatedArabic(arabic, &output));
    ASSERT_EQ(output.size(), 2);
    EXPECT_EQ(kSuccess[i][1], output[0].value);
    EXPECT_EQ(NumberUtil::NumberString::NUMBER_SEPARATED_ARABIC_HALFWIDTH,
              output[0].style);
    if (kSuccess[i][2]) {
      EXPECT_EQ(kSuccess[i][2], output[1].value);
      EXPECT_EQ(NumberUtil::NumberString::NUMBER_SEPARATED_ARABIC_FULLWIDTH,
                output[1].style);
    }
  }

  // Test data expected to fail
  const char* kFail[] = {
    "0123456789", "asdf0123456789", "0.001", "-100",
  };

  for (size_t i = 0; i < arraysize(kFail); ++i) {
    arabic = kFail[i];
    output.clear();
    EXPECT_FALSE(NumberUtil::ArabicToSeparatedArabic(arabic, &output));
    ASSERT_EQ(output.size(), 0);
  }
}

// ArabicToOtherForms
TEST(NumberUtilTest, ArabicToOtherFormsTest) {
  string arabic;
  std::vector<NumberUtil::NumberString> output;

  arabic = "5";
  output.clear();
  EXPECT_TRUE(NumberUtil::ArabicToOtherForms(arabic, &output));
  ASSERT_EQ(output.size(), 3);

  // "Ⅴ"
  EXPECT_EQ("\xE2\x85\xA4", output[0].value);
  EXPECT_EQ(NumberUtil::NumberString::NUMBER_ROMAN_CAPITAL, output[0].style);

  // "ⅴ"
  EXPECT_EQ("\xE2\x85\xB4", output[1].value);
  EXPECT_EQ(NumberUtil::NumberString::NUMBER_ROMAN_SMALL, output[1].style);

  // "⑤"
  EXPECT_EQ("\xE2\x91\xA4", output[2].value);
  EXPECT_EQ(NumberUtil::NumberString::NUMBER_CIRCLED, output[2].style);

  arabic = "0123456789";
  output.clear();
  EXPECT_FALSE(NumberUtil::ArabicToOtherForms(arabic, &output));
  ASSERT_EQ(output.size(), 0);

  arabic = "asdf0123456789";
  output.clear();
  EXPECT_FALSE(NumberUtil::ArabicToOtherForms(arabic, &output));
  ASSERT_EQ(output.size(), 0);

  arabic = "0.001";
  output.clear();
  EXPECT_FALSE(NumberUtil::ArabicToOtherForms(arabic, &output));
  ASSERT_EQ(output.size(), 0);

  arabic = "-100";
  output.clear();
  EXPECT_FALSE(NumberUtil::ArabicToOtherForms(arabic, &output));
  ASSERT_EQ(output.size(), 0);

  arabic = "18446744073709551616";  // UINT64_MAX + 1
  output.clear();
  EXPECT_FALSE(NumberUtil::ArabicToOtherForms(arabic, &output));
}

// ArabicToOtherRadixes
TEST(NumberUtilTest, ArabicToOtherRadixesTest) {
  string arabic;
  std::vector<NumberUtil::NumberString> output;

  arabic = "1";
  output.clear();
  // "1" is "1" in any radixes.
  EXPECT_FALSE(NumberUtil::ArabicToOtherRadixes(arabic, &output));

  arabic = "2";
  output.clear();
  EXPECT_TRUE(NumberUtil::ArabicToOtherRadixes(arabic, &output));
  ASSERT_EQ(output.size(), 1);

  arabic = "8";
  output.clear();
  EXPECT_TRUE(NumberUtil::ArabicToOtherRadixes(arabic, &output));
  ASSERT_EQ(output.size(), 2);
  EXPECT_EQ("010", output[0].value);
  EXPECT_EQ(NumberUtil::NumberString::NUMBER_OCT, output[0].style);
  EXPECT_EQ("0b1000", output[1].value);
  EXPECT_EQ(NumberUtil::NumberString::NUMBER_BIN, output[1].style);

  arabic = "16";
  output.clear();
  EXPECT_TRUE(NumberUtil::ArabicToOtherRadixes(arabic, &output));
  ASSERT_EQ(output.size(), 3);
  EXPECT_EQ("0x10", output[0].value);
  EXPECT_EQ(NumberUtil::NumberString::NUMBER_HEX, output[0].style);
  EXPECT_EQ("020", output[1].value);
  EXPECT_EQ(NumberUtil::NumberString::NUMBER_OCT, output[1].style);
  EXPECT_EQ("0b10000", output[2].value);
  EXPECT_EQ(NumberUtil::NumberString::NUMBER_BIN, output[2].style);

  arabic = "asdf0123456789";
  output.clear();
  EXPECT_FALSE(NumberUtil::ArabicToOtherRadixes(arabic, &output));
  ASSERT_EQ(output.size(), 0);

  arabic = "0.001";
  output.clear();
  EXPECT_FALSE(NumberUtil::ArabicToOtherRadixes(arabic, &output));
  ASSERT_EQ(output.size(), 0);

  arabic = "-100";
  output.clear();
  EXPECT_FALSE(NumberUtil::ArabicToOtherRadixes(arabic, &output));
  ASSERT_EQ(output.size(), 0);

  arabic = "18446744073709551616";  // UINT64_MAX + 1
  output.clear();
  EXPECT_FALSE(NumberUtil::ArabicToOtherRadixes(arabic, &output));
}

}  // namespace
}  // namespace gbase
