/* Copyright 2015 The gbase Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef GBASE_BASE_ERRORS_H_
#define GBASE_BASE_ERRORS_H_

#include "base/status.h"
#include "base/macros.h"

namespace gbase {

namespace internal {
// Do not call directly - these are not part of the public API.
string CatPieces(std::initializer_list<StringPiece> pieces) {
  string result;
  size_t total_size = 0;
  for (const StringPiece piece : pieces) total_size += piece.size();
  result.resize(total_size);

  char *const begin = &*result.begin();
  char *out = begin;
  for (const StringPiece piece : pieces) {
    const size_t this_size = piece.size();
    memcpy(out, piece.data(), this_size);
    out += this_size;
  }
  DCHECK_EQ(out, begin + result.size());
  return result;
}
}  // namespace internal

template <typename... AV>
string StrCat(const AV &... args) {
  return internal::CatPieces({
                               static_cast<const StringPiece &>(args)...
                             });
}

namespace errors {

typedef ::gbase::error::Code Code;

// Append some context to an error message.  Each time we append
// context put it on a new line, since it is possible for there
// to be several layers of additional context.
template <typename... Args>
void AppendToMessage(::gbase::Status* status, Args... args) {
  *status = ::gbase::Status(
      status->code(),
      ::gbase::StrCat(status->error_message(), "\n\t", args...));
}

// For propagating errors when calling a function.
#define TF_RETURN_IF_ERROR(expr)                         \
  do {                                                   \
    const ::gbase::Status _status = (expr);         \
    if (TF_PREDICT_FALSE(!_status.ok())) return _status; \
  } while (0)

#define TF_RETURN_WITH_CONTEXT_IF_ERROR(expr, ...)                  \
  do {                                                              \
    ::gbase::Status _status = (expr);                          \
    if (TF_PREDICT_FALSE(!_status.ok())) {                          \
      ::gbase::errors::AppendToMessage(&_status, __VA_ARGS__); \
      return _status;                                               \
    }                                                               \
  } while (0)

// Convenience functions for generating and using error status.
// Example usage:
//   status.Update(errors::InvalidArgument("The ", foo, " isn't right."));
//   if (errors::IsInvalidArgument(status)) { ... }
//   switch (status.code()) { case error::INVALID_ARGUMENT: ... }

#define DECLARE_ERROR(FUNC, CONST)                                       \
  template <typename... Args>                                            \
  ::gbase::Status FUNC(Args... args) {                              \
    return ::gbase::Status(::gbase::error::CONST,              \
                                ::gbase::StrCat(args...)); \
  }                                                                      \
  inline bool Is##FUNC(const ::gbase::Status& status) {             \
    return status.code() == ::gbase::error::CONST;                  \
  }

DECLARE_ERROR(Cancelled, CANCELLED)
DECLARE_ERROR(InvalidArgument, INVALID_ARGUMENT)
DECLARE_ERROR(NotFound, NOT_FOUND)
DECLARE_ERROR(AlreadyExists, ALREADY_EXISTS)
DECLARE_ERROR(ResourceExhausted, RESOURCE_EXHAUSTED)
DECLARE_ERROR(Unavailable, UNAVAILABLE)
DECLARE_ERROR(FailedPrecondition, FAILED_PRECONDITION)
DECLARE_ERROR(OutOfRange, OUT_OF_RANGE)
DECLARE_ERROR(Unimplemented, UNIMPLEMENTED)
DECLARE_ERROR(Internal, INTERNAL)
DECLARE_ERROR(Aborted, ABORTED)
DECLARE_ERROR(DeadlineExceeded, DEADLINE_EXCEEDED)
DECLARE_ERROR(DataLoss, DATA_LOSS)
DECLARE_ERROR(Unknown, UNKNOWN)
DECLARE_ERROR(PermissionDenied, PERMISSION_DENIED)
DECLARE_ERROR(Unauthenticated, UNAUTHENTICATED)

#undef DECLARE_ERROR

#define TF_EXPECT_OK(statement) \
  EXPECT_EQ(::gbase::Status::OK(), (statement))
#define TF_ASSERT_OK(statement) \
  ASSERT_EQ(::gbase::Status::OK(), (statement))

// The CanonicalCode() for non-errors.
using ::gbase::error::OK;

}  // namespace errors
}  // namespace gbase

#endif  // GBASE_BASE_ERRORS_H_
