#ifdef __COVERITY__
#else
// Copyright 2007, Google Inc.
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
//
// Author: wan@google.com (Zhanyong Wan)

// Google Mock - a framework for writing C++ mock classes.
//
// This file defines some utilities useful for implementing Google
// Mock.  They are subject to change without notice, so please DO NOT
// USE THEM IN USER CODE.

#ifndef GMOCK_INCLUDE_GMOCK_INTERNAL_GMOCK_INTERNAL_UTILS_H_
#define GMOCK_INCLUDE_GMOCK_INTERNAL_GMOCK_INTERNAL_UTILS_H_

#include <stdio.h>
#include <ostream>  // NOLINT
#include <string>

#include <gmock/internal/gmock-generated-internal-utils.h>
#include <gmock/internal/gmock-port.h>
#include <gtest/gtest.h>

// Concatenates two pre-processor symbols; works for concatenating
// built-in macros like __FILE__ and __LINE__.
#define GMOCK_CONCAT_TOKEN_IMPL_(foo, bar) foo##bar
#define GMOCK_CONCAT_TOKEN_(foo, bar) GMOCK_CONCAT_TOKEN_IMPL_(foo, bar)

#ifdef __GNUC__
#define GMOCK_ATTRIBUTE_UNUSED_ __attribute__ ((unused))
#else
#define GMOCK_ATTRIBUTE_UNUSED_
#endif  // __GNUC__

class ProtocolMessage;
namespace proto2 { class Message; }

namespace testing {
namespace internal {

// Converts an identifier name to a space-separated list of lower-case
// words.  Each maximum substring of the form [A-Za-z][a-z]*|\d+ is
// treated as one word.  For example, both "FooBar123" and
// "foo_bar_123" are converted to "foo bar 123".
string ConvertIdentifierNameToWords(const char* id_name);

// Defining a variable of type CompileAssertTypesEqual<T1, T2> will cause a
// compiler error iff T1 and T2 are different types.
template <typename T1, typename T2>
struct CompileAssertTypesEqual;

template <typename T>
struct CompileAssertTypesEqual<T, T> {
};

// Removes the reference from a type if it is a reference type,
// otherwise leaves it unchanged.  This is the same as
// tr1::remove_reference, which is not widely available yet.
template <typename T>
struct RemoveReference { typedef T type; };  // NOLINT
template <typename T>
struct RemoveReference<T&> { typedef T type; };  // NOLINT

// A handy wrapper around RemoveReference that works when the argument
// T depends on template parameters.
#define GMOCK_REMOVE_REFERENCE_(T) \
    typename ::testing::internal::RemoveReference<T>::type

// Removes const from a type if it is a const type, otherwise leaves
// it unchanged.  This is the same as tr1::remove_const, which is not
// widely available yet.
template <typename T>
struct RemoveConst { typedef T type; };  // NOLINT
template <typename T>
struct RemoveConst<const T> { typedef T type; };  // NOLINT

// A handy wrapper around RemoveConst that works when the argument
// T depends on template parameters.
#define GMOCK_REMOVE_CONST_(T) \
    typename ::testing::internal::RemoveConst<T>::type

// Adds reference to a type if it is not a reference type,
// otherwise leaves it unchanged.  This is the same as
// tr1::add_reference, which is not widely available yet.
template <typename T>
struct AddReference { typedef T& type; };  // NOLINT
template <typename T>
struct AddReference<T&> { typedef T& type; };  // NOLINT

// A handy wrapper around AddReference that works when the argument T
// depends on template parameters.
#define GMOCK_ADD_REFERENCE_(T) \
    typename ::testing::internal::AddReference<T>::type

// Adds a reference to const on top of T as necessary.  For example,
// it transforms
//
//   char         ==> const char&
//   const char   ==> const char&
//   char&        ==> const char&
//   const char&  ==> const char&
//
// The argument T must depend on some template parameters.
#define GMOCK_REFERENCE_TO_CONST_(T) \
    GMOCK_ADD_REFERENCE_(const GMOCK_REMOVE_REFERENCE_(T))

// PointeeOf<Pointer>::type is the type of a value pointed to by a
// Pointer, which can be either a smart pointer or a raw pointer.  The
// following default implementation is for the case where Pointer is a
// smart pointer.
template <typename Pointer>
struct PointeeOf {
  // Smart pointer classes define type element_type as the type of
  // their pointees.
  typedef typename Pointer::element_type type;
};
// This specialization is for the raw pointer case.
template <typename T>
struct PointeeOf<T*> { typedef T type; };  // NOLINT

// GetRawPointer(p) returns the raw pointer underlying p when p is a
// smart pointer, or returns p itself when p is already a raw pointer.
// The following default implementation is for the smart pointer case.
template <typename Pointer>
inline typename Pointer::element_type* GetRawPointer(const Pointer& p) {
  return p.get();
}
// This overloaded version is for the raw pointer case.
template <typename Element>
inline Element* GetRawPointer(Element* p) { return p; }

// This comparator allows linked_ptr to be stored in sets.
template <typename T>
struct LinkedPtrLessThan {
  bool operator()(const ::testing::internal::linked_ptr<T>& lhs, 
                  const ::testing::internal::linked_ptr<T>& rhs) const {
    return lhs.get() < rhs.get();
  }
};

// ImplicitlyConvertible<From, To>::value is a compile-time bool
// constant that's true iff type From can be implicitly converted to
// type To.
template <typename From, typename To>
class ImplicitlyConvertible {
 private:
  // We need the following helper functions only for their types.
  // They have no implementations.

  // MakeFrom() is an expression whose type is From.  We cannot simply
  // use From(), as the type From may not have a public default
  // constructor.
  static From MakeFrom();

  // These two functions are overloaded.  Given an expression
  // Helper(x), the compiler will pick the first version if x can be
  // implicitly converted to type To; otherwise it will pick the
  // second version.
  //
  // The first version returns a value of size 1, and the second
  // version returns a value of size 2.  Therefore, by checking the
  // size of Helper(x), which can be done at compile time, we can tell
  // which version of Helper() is used, and hence whether x can be
  // implicitly converted to type To.
  static char Helper(To);
  static char (&Helper(...))[2];  // NOLINT

  // We have to put the 'public' section after the 'private' section,
  // or MSVC refuses to compile the code.
 public:
  // MSVC warns about implicitly converting from double to int for
  // possible loss of data, so we need to temporarily disable the
  // warning.
#ifdef _MSC_VER
#pragma warning(push)          // Saves the current warning state.
#pragma warning(disable:4244)  // Temporarily disables warning 4244.
  static const bool value =
      sizeof(Helper(ImplicitlyConvertible::MakeFrom())) == 1;
#pragma warning(pop)           // Restores the warning state.
#else
  static const bool value =
      sizeof(Helper(ImplicitlyConvertible::MakeFrom())) == 1;
#endif  // _MSV_VER
};
template <typename From, typename To>
const bool ImplicitlyConvertible<From, To>::value;

// IsAProtocolMessage<T>::value is a compile-time bool constant that's
// true iff T is type ProtocolMessage, proto2::Message, or a subclass
// of those.
template <typename T>
struct IsAProtocolMessage {
  static const bool value =
      ImplicitlyConvertible<const T*, const ::ProtocolMessage*>::value ||
      ImplicitlyConvertible<const T*, const ::proto2::Message*>::value;
};
template <typename T>
const bool IsAProtocolMessage<T>::value;

// When the compiler sees expression IsContainerTest<C>(0), the first
// overload of IsContainerTest will be picked if C is an STL-style
// container class (since C::const_iterator* is a valid type and 0 can
// be converted to it), while the second overload will be picked
// otherwise (since C::const_iterator will be an invalid type in this
// case).  Therefore, we can determine whether C is a container class
// by checking the type of IsContainerTest<C>(0).  The value of the
// expression is insignificant.
typedef int IsContainer;
template <class C>
IsContainer IsContainerTest(typename C::const_iterator*) { return 0; }

typedef char IsNotContainer;
template <class C>
IsNotContainer IsContainerTest(...) { return '\0'; }

// This interface knows how to report a Google Mock failure (either
// non-fatal or fatal).
class FailureReporterInterface {
 public:
  // The type of a failure (either non-fatal or fatal).
  enum FailureType {
    NONFATAL, FATAL
  };

  virtual ~FailureReporterInterface() {}

  // Reports a failure that occurred at the given source file location.
  virtual void ReportFailure(FailureType type, const char* file, int line,
                             const string& message) = 0;
};

// Returns the failure reporter used by Google Mock.
FailureReporterInterface* GetFailureReporter();

// Asserts that condition is true; aborts the process with the given
// message if condition is false.  We cannot use LOG(FATAL) or CHECK()
// as Google Mock might be used to mock the log sink itself.  We
// inline this function to prevent it from showing up in the stack
// trace.
inline void Assert(bool condition, const char* file, int line,
                   const string& msg) {
  if (!condition) {
    GetFailureReporter()->ReportFailure(FailureReporterInterface::FATAL,
                                        file, line, msg);
  }
}
inline void Assert(bool condition, const char* file, int line) {
  Assert(condition, file, line, "Assertion failed.");
}

// Verifies that condition is true; generates a non-fatal failure if
// condition is false.
inline void Expect(bool condition, const char* file, int line,
                   const string& msg) {
  if (!condition) {
    GetFailureReporter()->ReportFailure(FailureReporterInterface::NONFATAL,
                                        file, line, msg);
  }
}
inline void Expect(bool condition, const char* file, int line) {
  Expect(condition, file, line, "Expectation failed.");
}

// Severity level of a log.
enum LogSeverity {
  INFO = 0,
  WARNING = 1,
};

// Valid values for the --gmock_verbose flag.

// All logs (informational and warnings) are printed.
const char kInfoVerbosity[] = "info";
// Only warnings are printed.
const char kWarningVerbosity[] = "warning";
// No logs are printed.
const char kErrorVerbosity[] = "error";

// Prints the given message to stdout iff 'severity' >= the level
// specified by the --gmock_verbose flag.  If stack_frames_to_skip >=
// 0, also prints the stack trace excluding the top
// stack_frames_to_skip frames.  In opt mode, any positive
// stack_frames_to_skip is treated as 0, since we don't know which
// function calls will be inlined by the compiler and need to be
// conservative.
void Log(LogSeverity severity, const string& message, int stack_frames_to_skip);

// The universal value printer (public/gmock-printers.h) needs this
// to declare an unused << operator in the global namespace.
struct Unused {};

// Type traits.

// is_reference<T>::value is non-zero iff T is a reference type.
template <typename T> struct is_reference : public false_type {};
template <typename T> struct is_reference<T&> : public true_type {};

// type_equals<T1, T2>::value is non-zero iff T1 and T2 are the same type.
template <typename T1, typename T2> struct type_equals : public false_type {};
template <typename T> struct type_equals<T, T> : public true_type {};

// remove_reference<T>::type removes the reference from type T, if any.
template <typename T> struct remove_reference { typedef T type; };
template <typename T> struct remove_reference<T&> { typedef T type; };

// Invalid<T>() returns an invalid value of type T.  This is useful
// when a value of type T is needed for compilation, but the statement
// will not really be executed (or we don't care if the statement
// crashes).
template <typename T>
inline T Invalid() {
  return *static_cast<typename remove_reference<T>::type*>(NULL);
}
template <>
inline void Invalid<void>() {}

}  // namespace internal
}  // namespace testing

#endif  // GMOCK_INCLUDE_GMOCK_INTERNAL_GMOCK_INTERNAL_UTILS_H_

#endif