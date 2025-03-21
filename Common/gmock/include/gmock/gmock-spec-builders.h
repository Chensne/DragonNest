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
// This file implements the ON_CALL() and EXPECT_CALL() macros.
//
// A user can use the ON_CALL() macro to specify the default action of
// a mock method.  The syntax is:
//
//   ON_CALL(mock_object, Method(argument-matchers))
//       .WithArguments(multi-argument-matcher)
//       .WillByDefault(action);
//
//  where the .WithArguments() clause is optional.
//
// A user can use the EXPECT_CALL() macro to specify an expectation on
// a mock method.  The syntax is:
//
//   EXPECT_CALL(mock_object, Method(argument-matchers))
//       .WithArguments(multi-argument-matchers)
//       .Times(cardinality)
//       .InSequence(sequences)
//       .WillOnce(action)
//       .WillRepeatedly(action)
//       .RetiresOnSaturation();
//
// where all clauses are optional, .InSequence() and .WillOnce() can
// appear any number of times, and .Times() can be omitted only if
// .WillOnce() or .WillRepeatedly() is present.

#ifndef GMOCK_INCLUDE_GMOCK_GMOCK_SPEC_BUILDERS_H_
#define GMOCK_INCLUDE_GMOCK_GMOCK_SPEC_BUILDERS_H_

#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <gmock/gmock-actions.h>
#include <gmock/gmock-cardinalities.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-printers.h>
#include <gmock/internal/gmock-internal-utils.h>
#include <gmock/internal/gmock-port.h>
#include <gtest/gtest.h>

namespace testing {

// Anything inside the 'internal' namespace IS INTERNAL IMPLEMENTATION
// and MUST NOT BE USED IN USER CODE!!!
namespace internal {

template <typename F>
class FunctionMocker;

// Base class for expectations.
class ExpectationBase;

// Helper class for testing the Expectation class template.
class ExpectationTester;

// Base class for function mockers.
template <typename F>
class FunctionMockerBase;

// Helper class for implementing FunctionMockerBase<F>::InvokeWith().
template <typename Result, typename F>
class InvokeWithHelper;

// Protects the mock object registry (in class Mock), all function
// mockers, and all expectations.
//
// The reason we don't use more fine-grained protection is: when a
// mock function Foo() is called, it needs to consult its expectations
// to see which one should be picked.  If another thread is allowed to
// call a mock function (either Foo() or a different one) at the same
// time, it could affect the "retired" attributes of Foo()'s
// expectations when InSequence() is used, and thus affect which
// expectation gets picked.  Therefore, we sequence all mock function
// calls to ensure the integrity of the mock objects' states.
extern Mutex g_gmock_mutex;

// Abstract base class of FunctionMockerBase.  This is the
// type-agnostic part of the function mocker interface.  Its pure
// virtual methods are implemented by FunctionMockerBase.
class UntypedFunctionMockerBase {
 public:
  virtual ~UntypedFunctionMockerBase() {}

  // Verifies that all expectations on this mock function have been
  // satisfied.  Reports one or more Google Test non-fatal failures
  // and returns false if not.
  // L >= g_gmock_mutex
  virtual bool VerifyAndClearExpectationsLocked() = 0;

  // Clears the ON_CALL()s set on this mock function.
  // L >= g_gmock_mutex
  virtual void ClearDefaultActionsLocked() = 0;
};  // class UntypedFunctionMockerBase

// This template class implements a default action spec (i.e. an
// ON_CALL() statement).
template <typename F>
class DefaultActionSpec {
 public:
  typedef typename Function<F>::ArgumentTuple ArgumentTuple;
  typedef typename Function<F>::ArgumentMatcherTuple ArgumentMatcherTuple;

  // Constructs a DefaultActionSpec object from the information inside
  // the parenthesis of an ON_CALL() statement.
  DefaultActionSpec(const char* file, int line,
                    const ArgumentMatcherTuple& matchers)
      : file_(file),
        line_(line),
        matchers_(matchers),
        extra_matcher_(_),
        last_clause_(NONE) {
  }

  // Where in the source file was the default action spec defined?
  const char* file() const { return file_; }
  int line() const { return line_; }

  // Implements the .WithArguments() clause.
  DefaultActionSpec& WithArguments(const Matcher<const ArgumentTuple&>& m) {
    // Makes sure this is called at most once.
    ExpectSpecProperty(last_clause_ < WITH_ARGUMENTS,
                       ".WithArguments() cannot appear "
                       "more than once in an ON_CALL().");
    last_clause_ = WITH_ARGUMENTS;

    extra_matcher_ = m;
    return *this;
  }

  // Implements the .WillByDefault() clause.
  DefaultActionSpec& WillByDefault(const Action<F>& action) {
    ExpectSpecProperty(last_clause_ < WILL_BY_DEFAULT,
                       ".WillByDefault() must appear "
                       "exactly once in an ON_CALL().");
    last_clause_ = WILL_BY_DEFAULT;

    ExpectSpecProperty(!action.IsDoDefault(),
                       "DoDefault() cannot be used in ON_CALL().");
    action_ = action;
    return *this;
  }

  // Returns true iff the given arguments match the matchers.
  bool Matches(const ArgumentTuple& args) const {
    return TupleMatches(matchers_, args) && extra_matcher_.Matches(args);
  }

  // Returns the action specified by the user.
  const Action<F>& GetAction() const {
    AssertSpecProperty(last_clause_ == WILL_BY_DEFAULT,
                       ".WillByDefault() must appear exactly "
                       "once in an ON_CALL().");
    return action_;
  }
 private:
  // Gives each clause in the ON_CALL() statement a name.
  enum Clause {
    // Do not change the order of the enum members!  The run-time
    // syntax checking relies on it.
    NONE,
    WITH_ARGUMENTS,
    WILL_BY_DEFAULT,
  };

  // Asserts that the ON_CALL() statement has a certain property.
  void AssertSpecProperty(bool property, const string& failure_message) const {
    Assert(property, file_, line_, failure_message);
  }

  // Expects that the ON_CALL() statement has a certain property.
  void ExpectSpecProperty(bool property, const string& failure_message) const {
    Expect(property, file_, line_, failure_message);
  }

  // The information in statement
  //
  //   ON_CALL(mock_object, Method(matchers))
  //       .WithArguments(multi-argument-matcher)
  //       .WillByDefault(action);
  //
  // is recorded in the data members like this:
  //
  //   source file that contains the statement => file_
  //   line number of the statement            => line_
  //   matchers                                => matchers_
  //   multi-argument-matcher                  => extra_matcher_
  //   action                                  => action_
  const char* file_;
  int line_;
  ArgumentMatcherTuple matchers_;
  Matcher<const ArgumentTuple&> extra_matcher_;
  Action<F> action_;

  // The last clause in the ON_CALL() statement as seen so far.
  // Initially NONE and changes as the statement is parsed.
  Clause last_clause_;
};  // class DefaultActionSpec

// Possible reactions on uninteresting calls.
enum CallReaction {
  ALLOW,
  WARN,
  FAIL,
};

}  // namespace internal

// Utilities for manipulating mock objects.
class Mock {
 public:
  // The following public methods can be called concurrently.

  // Verifies and clears all expectations on the given mock object.
  // If the expectations aren't satisfied, generates one or more
  // Google Test non-fatal failures and returns false.
  static bool VerifyAndClearExpectations(void* mock_obj);

  // Verifies all expectations on the given mock object and clears its
  // default actions and expectations.  Returns true iff the
  // verification was successful.
  static bool VerifyAndClear(void* mock_obj);
 private:
  // Needed for a function mocker to register itself (so that we know
  // how to clear a mock object).
  template <typename F>
  friend class internal::FunctionMockerBase;

  template <typename R, typename Args>
  friend class internal::InvokeWithHelper;

  template <typename M>
  friend class NiceMock;

  template <typename M>
  friend class StrictMock;

  // Tells Google Mock to allow uninteresting calls on the given mock
  // object.
  // L < g_gmock_mutex
  static void AllowUninterestingCalls(const void* mock_obj);

  // Tells Google Mock to warn the user about uninteresting calls on
  // the given mock object.
  // L < g_gmock_mutex
  static void WarnUninterestingCalls(const void* mock_obj);

  // Tells Google Mock to fail uninteresting calls on the given mock
  // object.
  // L < g_gmock_mutex
  static void FailUninterestingCalls(const void* mock_obj);

  // Tells Google Mock the given mock object is being destroyed and
  // its entry in the call-reaction table should be removed.
  // L < g_gmock_mutex
  static void UnregisterCallReaction(const void* mock_obj);

  // Returns the reaction Google Mock will have on uninteresting calls
  // made on the given mock object.
  // L < g_gmock_mutex
  static internal::CallReaction GetReactionOnUninterestingCalls(
      const void* mock_obj);

  // Verifies that all expectations on the given mock object have been
  // satisfied.  Reports one or more Google Test non-fatal failures
  // and returns false if not.
  // L >= g_gmock_mutex
  static bool VerifyAndClearExpectationsLocked(void* mock_obj);

  // Clears all ON_CALL()s set on the given mock object.
  // L >= g_gmock_mutex
  static void ClearDefaultActionsLocked(void* mock_obj);

  // Registers a mock object and a mock method it owns.
  // L < g_gmock_mutex
  static void Register(const void* mock_obj,
                       internal::UntypedFunctionMockerBase* mocker);

  // Unregisters a mock method; removes the owning mock object from
  // the registry when the last mock method associated with it has
  // been unregistered.  This is called only in the destructor of
  // FunctionMockerBase.
  // L >= g_gmock_mutex
  static void UnregisterLocked(internal::UntypedFunctionMockerBase* mocker);
};  // class Mock

// Sequence objects are used by a user to specify the relative order
// in which the expectations should match.  They are copyable (we rely
// on the compiler-defined copy constructor and assignment operator).
class Sequence {
 public:
  // Constructs an empty sequence.
  Sequence()
      : last_expectation_(
          new internal::linked_ptr<internal::ExpectationBase>(NULL)) {}

  // Adds an expectation to this sequence.  The caller must ensure
  // that no other thread is accessing this Sequence object.
  void AddExpectation(
      const internal::linked_ptr<internal::ExpectationBase>& expectation) const;
 private:
  // The last expectation in this sequence.  We use a nested
  // linked_ptr here because:
  //   - Sequence objects are copyable, and we want the copies to act
  //     as aliases.  The outer linked_ptr allows the copies to co-own
  //     and share the same state.
  //   - An Expectation object is co-owned (via linked_ptr) by its
  //     FunctionMocker and its successors (other Expectation objects).
  //     Hence the inner linked_ptr.
  internal::linked_ptr<internal::linked_ptr<internal::ExpectationBase> >
      last_expectation_;
};  // class Sequence

// An object of this type causes all EXPECT_CALL() statements
// encountered in its scope to be put in an anonymous sequence.  The
// work is done in the constructor and destructor.  You should only
// create an InSequence object on the stack.
//
// The sole purpose for this class is to support easy definition of
// sequential expectations, e.g.
//
//   {
//     InSequence dummy;  // The name of the object doesn't matter.
//
//     // The following expectations must match in the order they appear.
//     EXPECT_CALL(a, Bar())...;
//     EXPECT_CALL(a, Baz())...;
//     ...
//     EXPECT_CALL(b, Xyz())...;
//   }
//
// You can create InSequence objects in multiple threads, as long as
// they are used to affect different mock objects.  The idea is that
// each thread can create and set up its own mocks as if it's the only
// thread.  However, for clarity of your tests we recommend you to set
// up mocks in the main thread unless you have a good reason not to do
// so.
class InSequence {
 public:
  InSequence();
  ~InSequence();
 private:
  bool sequence_created_;

  GTEST_DISALLOW_COPY_AND_ASSIGN_(InSequence);  // NOLINT
} GMOCK_ATTRIBUTE_UNUSED_;

namespace internal {

// Points to the implicit sequence introduced by a living InSequence
// object (if any) in the current thread or NULL.
extern ThreadLocal<Sequence*> g_gmock_implicit_sequence;

// Base class for implementing expectations.
//
// There are two reasons for having a type-agnostic base class for
// Expectation:
//
//   1. We need to store collections of expectations of different
//   types (e.g. all pre-requisites of a particular expectation, all
//   expectations in a sequence).  Therefore these expectation objects
//   must share a common base class.
//
//   2. We can avoid binary code bloat by moving methods not depending
//   on the template argument of Expectation to the base class.
//
// This class is internal and mustn't be used by user code directly.
class ExpectationBase {
 public:
  ExpectationBase(const char* file, int line);

  virtual ~ExpectationBase();

  // Where in the source file was the expectation spec defined?
  const char* file() const { return file_; }
  int line() const { return line_; }

  // Returns the cardinality specified in the expectation spec.
  const Cardinality& cardinality() const { return cardinality_; }

  // Describes the source file location of this expectation.
  void DescribeLocationTo(::std::ostream* os) const {
    *os << file() << ":" << line() << ": ";
  }

  // Describes how many times a function call matching this
  // expectation has occurred.
  // L >= g_gmock_mutex
  virtual void DescribeCallCountTo(::std::ostream* os) const = 0;
 protected:
  typedef std::set<linked_ptr<ExpectationBase>,
                   LinkedPtrLessThan<ExpectationBase> >
      ExpectationBaseSet;

  enum Clause {
    // Don't change the order of the enum members!
    NONE,
    WITH_ARGUMENTS,
    TIMES,
    IN_SEQUENCE,
    WILL_ONCE,
    WILL_REPEATEDLY,
    RETIRES_ON_SATURATION,
  };

  // Asserts that the EXPECT_CALL() statement has the given property.
  void AssertSpecProperty(bool property, const string& failure_message) const {
    Assert(property, file_, line_, failure_message);
  }

  // Expects that the EXPECT_CALL() statement has the given property.
  void ExpectSpecProperty(bool property, const string& failure_message) const {
    Expect(property, file_, line_, failure_message);
  }

  // Explicitly specifies the cardinality of this expectation.  Used
  // by the subclasses to implement the .Times() clause.
  void SpecifyCardinality(const Cardinality& cardinality);

  // Returns true iff the user specified the cardinality explicitly
  // using a .Times().
  bool cardinality_specified() const { return cardinality_specified_; }

  // Sets the cardinality of this expectation spec.
  void set_cardinality(const Cardinality& cardinality) {
    cardinality_ = cardinality;
  }

  // The following group of methods should only be called after the
  // EXPECT_CALL() statement, and only when g_gmock_mutex is held by
  // the current thread.

  // Retires all pre-requisites of this expectation.
  // L >= g_gmock_mutex
  void RetireAllPreRequisites();

  // Returns true iff this expectation is retired.
  // L >= g_gmock_mutex
  bool is_retired() const {
    g_gmock_mutex.AssertHeld();
    return retired_;
  }

  // Retires this expectation.
  // L >= g_gmock_mutex
  void Retire() {
    g_gmock_mutex.AssertHeld();
    retired_ = true;
  }

  // Returns true iff this expectation is satisfied.
  // L >= g_gmock_mutex
  bool IsSatisfied() const {
    g_gmock_mutex.AssertHeld();
    return cardinality().IsSatisfiedByCallCount(call_count_);
  }

  // Returns true iff this expectation is saturated.
  // L >= g_gmock_mutex
  bool IsSaturated() const {
    g_gmock_mutex.AssertHeld();
    return cardinality().IsSaturatedByCallCount(call_count_);
  }

  // Returns true iff this expectation is over-saturated.
  // L >= g_gmock_mutex
  bool IsOverSaturated() const {
    g_gmock_mutex.AssertHeld();
    return cardinality().IsOverSaturatedByCallCount(call_count_);
  }

  // Returns true iff all pre-requisites of this expectation are satisfied.
  // L >= g_gmock_mutex
  bool AllPrerequisitesAreSatisfied() const;

  // Adds unsatisfied pre-requisites of this expectation to 'result'.
  // L >= g_gmock_mutex
  void FindUnsatisfiedPrerequisites(ExpectationBaseSet* result) const;

  // Returns the number this expectation has been invoked.
  // L >= g_gmock_mutex
  int call_count() const {
    g_gmock_mutex.AssertHeld();
    return call_count_;
  }

  // Increments the number this expectation has been invoked.
  // L >= g_gmock_mutex
  void IncrementCallCount() {
    g_gmock_mutex.AssertHeld();
    call_count_++;
  }

 private:
  friend class ::testing::Sequence;
  friend class ::testing::internal::ExpectationTester;

  template <typename Function>
  friend class Expectation;

  // This group of fields are part of the spec and won't change after
  // an EXPECT_CALL() statement finishes.
  const char* file_;  // The file that contains the expectation.
  int line_;          // The line number of the expectation.
  // True iff the cardinality is specified explicitly.
  bool cardinality_specified_;
  Cardinality cardinality_;            // The cardinality of the expectation.
  // The immediate pre-requisites of this expectation.  We use
  // linked_ptr in the set because we want an Expectation object to be
  // co-owned by its FunctionMocker and its successors.  This allows
  // multiple mock objects to be deleted at different times.
  ExpectationBaseSet immediate_prerequisites_;

  // This group of fields are the current state of the expectation,
  // and can change as the mock function is called.
  int call_count_;  // How many times this expectation has been invoked.
  bool retired_;    // True iff this expectation has retired.
};  // class ExpectationBase

// Impements an expectation for the given function type.
template <typename F>
class Expectation : public ExpectationBase {
 public:
  typedef typename Function<F>::ArgumentTuple ArgumentTuple;
  typedef typename Function<F>::ArgumentMatcherTuple ArgumentMatcherTuple;
  typedef typename Function<F>::Result Result;

  Expectation(FunctionMockerBase<F>* owner, const char* file, int line,
              const ArgumentMatcherTuple& m)
      : ExpectationBase(file, line),
        owner_(owner),
        matchers_(m),
        extra_matcher_(_),
        repeated_action_specified_(false),
        repeated_action_(DoDefault()),
        retires_on_saturation_(false),
        last_clause_(NONE),
        action_count_checked_(false) {}

  virtual ~Expectation() {
    // Check the validity of the action count if it hasn't been done
    // yet (for example, if the expectation was never used).
    CheckActionCountIfNotDone();
  }

  // Implements the .WithArguments() clause.
  Expectation& WithArguments(const Matcher<const ArgumentTuple&>& m) {
    if (last_clause_ == WITH_ARGUMENTS) {
      ExpectSpecProperty(false,
                         ".WithArguments() cannot appear "
                         "more than once in an EXPECT_CALL().");
    } else {
      ExpectSpecProperty(last_clause_ < WITH_ARGUMENTS,
                         ".WithArguments() must be the first "
                         "clause in an EXPECT_CALL().");
    }
    last_clause_ = WITH_ARGUMENTS;

    extra_matcher_ = m;
    return *this;
  }

  // Implements the .Times() clause.
  Expectation& Times(const Cardinality& cardinality) {
    if (last_clause_ ==TIMES) {
      ExpectSpecProperty(false,
                         ".Times() cannot appear "
                         "more than once in an EXPECT_CALL().");
    } else {
      ExpectSpecProperty(last_clause_ < TIMES,
                         ".Times() cannot appear after "
                         ".InSequence(), .WillOnce(), .WillRepeatedly(), "
                         "or .RetiresOnSaturation().");
    }
    last_clause_ = TIMES;

    ExpectationBase::SpecifyCardinality(cardinality);
    return *this;
  }

  // Implements the .Times() clause.
  Expectation& Times(int n) {
    return Times(Exactly(n));
  }

  // Implements the .InSequence() clause.
  Expectation& InSequence(const Sequence& s) {
    ExpectSpecProperty(last_clause_ <= IN_SEQUENCE,
                       ".InSequence() cannot appear after .WillOnce(),"
                       " .WillRepeatedly(), or "
                       ".RetiresOnSaturation().");
    last_clause_ = IN_SEQUENCE;

    s.AddExpectation(owner_->GetLinkedExpectationBase(this));
    return *this;
  }
  Expectation& InSequence(const Sequence& s1, const Sequence& s2) {
    return InSequence(s1).InSequence(s2);
  }
  Expectation& InSequence(const Sequence& s1, const Sequence& s2,
                              const Sequence& s3) {
    return InSequence(s1, s2).InSequence(s3);
  }
  Expectation& InSequence(const Sequence& s1, const Sequence& s2,
                              const Sequence& s3, const Sequence& s4) {
    return InSequence(s1, s2, s3).InSequence(s4);
  }
  Expectation& InSequence(const Sequence& s1, const Sequence& s2,
                              const Sequence& s3, const Sequence& s4,
                              const Sequence& s5) {
    return InSequence(s1, s2, s3, s4).InSequence(s5);
  }

  // Implements the .WillOnce() clause.
  Expectation& WillOnce(const Action<F>& action) {
    ExpectSpecProperty(last_clause_ <= WILL_ONCE,
                       ".WillOnce() cannot appear after "
                       ".WillRepeatedly() or .RetiresOnSaturation().");
    last_clause_ = WILL_ONCE;

    actions_.push_back(action);
    if (!cardinality_specified()) {
      set_cardinality(Exactly(static_cast<int>(actions_.size())));
    }
    return *this;
  }

  // Implements the .WillRepeatedly() clause.
  Expectation& WillRepeatedly(const Action<F>& action) {
    if (last_clause_ == WILL_REPEATEDLY) {
      ExpectSpecProperty(false,
                         ".WillRepeatedly() cannot appear "
                         "more than once in an EXPECT_CALL().");
    } else {
      ExpectSpecProperty(last_clause_ < WILL_REPEATEDLY,
                         ".WillRepeatedly() cannot appear "
                         "after .RetiresOnSaturation().");
    }
    last_clause_ = WILL_REPEATEDLY;
    repeated_action_specified_ = true;

    repeated_action_ = action;
    if (!cardinality_specified()) {
      set_cardinality(AtLeast(static_cast<int>(actions_.size())));
    }

    // Now that no more action clauses can be specified, we check
    // whether their count makes sense.
    CheckActionCountIfNotDone();
    return *this;
  }

  // Implements the .RetiresOnSaturation() clause.
  Expectation& RetiresOnSaturation() {
    ExpectSpecProperty(last_clause_ < RETIRES_ON_SATURATION,
                       ".RetiresOnSaturation() cannot appear "
                       "more than once.");
    last_clause_ = RETIRES_ON_SATURATION;
    retires_on_saturation_ = true;

    // Now that no more action clauses can be specified, we check
    // whether their count makes sense.
    CheckActionCountIfNotDone();
    return *this;
  }

  // Returns the matchers for the arguments as specified inside the
  // EXPECT_CALL() macro.
  const ArgumentMatcherTuple& matchers() const {
    return matchers_;
  }

  // Returns the matcher specified by the .WithArguments() clause.
  const Matcher<const ArgumentTuple&>& extra_matcher() const {
    return extra_matcher_;
  }

  // Returns the sequence of actions specified by the .WillOnce() clause.
  const std::vector<Action<F> >& actions() const { return actions_; }

  // Returns the action specified by the .WillRepeatedly() clause.
  const Action<F>& repeated_action() const { return repeated_action_; }

  // Returns true iff the .RetiresOnSaturation() clause was specified.
  bool retires_on_saturation() const { return retires_on_saturation_; }

  // Describes how many times a function call matching this
  // expectation has occurred (implements
  // ExpectationBase::DescribeCallCountTo()).
  // L >= g_gmock_mutex
  virtual void DescribeCallCountTo(::std::ostream* os) const {
    g_gmock_mutex.AssertHeld();

    // Describes how many times the function is expected to be called.
    *os << "         Expected: to be ";
    cardinality().DescribeTo(os);
    *os << "\n           Actual: ";
    Cardinality::DescribeActualCallCountTo(call_count(), os);

    // Describes the state of the expectation (e.g. is it satisfied?
    // is it active?).
    *os << " - " << (IsOverSaturated() ? "over-saturated" :
                     IsSaturated() ? "saturated" :
                     IsSatisfied() ? "satisfied" : "unsatisfied")
        << " and "
        << (is_retired() ? "retired" : "active");
  }
 private:
  template <typename Function>
  friend class FunctionMockerBase;

  template <typename R, typename Function>
  friend class InvokeWithHelper;

  // The following methods will be called only after the EXPECT_CALL()
  // statement finishes and when the current thread holds
  // g_gmock_mutex.

  // Returns true iff this expectation matches the given arguments.
  // L >= g_gmock_mutex
  bool Matches(const ArgumentTuple& args) const {
    g_gmock_mutex.AssertHeld();
    return TupleMatches(matchers_, args) && extra_matcher_.Matches(args);
  }

  // Returns true iff this expectation should handle the given arguments.
  // L >= g_gmock_mutex
  bool ShouldHandleArguments(const ArgumentTuple& args) const {
    g_gmock_mutex.AssertHeld();

    // In case the action count wasn't checked when the expectation
    // was defined (e.g. if this expectation has no WillRepeatedly()
    // or RetiresOnSaturation() clause), we check it when the
    // expectation is used for the first time.
    CheckActionCountIfNotDone();
    return !is_retired() && AllPrerequisitesAreSatisfied() && Matches(args);
  }

  // Describes the result of matching the arguments against this
  // expectation to the given ostream.
  // L >= g_gmock_mutex
  void DescribeMatchResultTo(const ArgumentTuple& args,
                             ::std::ostream* os) const {
    g_gmock_mutex.AssertHeld();

    if (is_retired()) {
      *os << "         Expected: the expectation is active\n"
          << "           Actual: it is retired\n";
    } else if (!Matches(args)) {
      if (!TupleMatches(matchers_, args)) {
        DescribeMatchFailureTupleTo(matchers_, args, os);
      }
      if (!extra_matcher_.Matches(args)) {
        *os << "         Expected: ";
        extra_matcher_.DescribeTo(os);
        *os << "\n           Actual: false";

        internal::ExplainMatchResultAsNeededTo<const ArgumentTuple&>(
            extra_matcher_, args, os);
        *os << "\n";
      }
    } else if (!AllPrerequisitesAreSatisfied()) {
      *os << "         Expected: all pre-requisites are satisfied\n"
          << "           Actual: the following immediate pre-requisites "
          << "are not satisfied:\n";
      ExpectationBaseSet unsatisfied_prereqs;
      FindUnsatisfiedPrerequisites(&unsatisfied_prereqs);
      int i = 0;
      for (ExpectationBaseSet::const_iterator it = unsatisfied_prereqs.begin();
           it != unsatisfied_prereqs.end(); ++it) {
        (*it)->DescribeLocationTo(os);
        *os << "pre-requisite #" << i++ << "\n";
      }
      *os << "                   (end of pre-requisites)\n";
    } else {
      // This line is here just for completeness' sake.  It will never
      // be executed as currently the DescribeMatchResultTo() function
      // is called only when the mock function call does NOT match the
      // expectation.
      *os << "The call matches the expectation.\n";
    }
  }

  // Returns the action that should be taken for the current invocation.
  // L >= g_gmock_mutex
  const Action<F>& GetCurrentAction(const FunctionMockerBase<F>* mocker,
                                    const ArgumentTuple& args) const {
    g_gmock_mutex.AssertHeld();
    const int count = call_count();
    Assert(count >= 1, __FILE__, __LINE__,
           "call_count() is <= 0 when GetCurrentAction() is "
           "called - this should never happen.");

    const int action_count = static_cast<int>(actions().size());
    if (action_count > 0 && !repeated_action_specified_ &&
        count > action_count) {
      // If there is at least one WillOnce() and no WillRepeatedly(),
      // we warn the user when the WillOnce() clauses ran out.
      ::std::stringstream ss;
      DescribeLocationTo(&ss);
      ss << "Actions ran out.\n"
         << "Called " << count << " times, but only "
         << action_count << " WillOnce()"
         << (action_count == 1 ? " is" : "s are") << " specified - ";
      mocker->DescribeDefaultActionTo(args, &ss);
      Log(WARNING, ss.str(), 1);
    }

    return count <= action_count ? actions()[count - 1] : repeated_action();
  }

  // Given the arguments of a mock function call, if the call will
  // over-saturate this expectation, returns the default action;
  // otherwise, returns the next action in this expectation.  Also
  // describes *what* happened to 'what', and explains *why* Google
  // Mock does it to 'why'.  This method is not const as it calls
  // IncrementCallCount().
  // L >= g_gmock_mutex
  Action<F> GetActionForArguments(const FunctionMockerBase<F>* mocker,
                                  const ArgumentTuple& args,
                                  ::std::ostream* what,
                                  ::std::ostream* why) {
    g_gmock_mutex.AssertHeld();
    if (IsSaturated()) {
      // We have an excessive call.
      IncrementCallCount();
      *what << "Mock function called more times than expected - ";
      mocker->DescribeDefaultActionTo(args, what);
      DescribeCallCountTo(why);

      // TODO(wan): allow the user to control whether unexpected calls
      // should fail immediately or continue using a flag
      // --gmock_unexpected_calls_are_fatal.
      return DoDefault();
    }

    IncrementCallCount();
    RetireAllPreRequisites();

    if (retires_on_saturation() && IsSaturated()) {
      Retire();
    }

    // Must be done after IncrementCount()!
    *what << "Expected mock function call.\n";
    return GetCurrentAction(mocker, args);
  }

  // Checks the action count (i.e. the number of WillOnce() and
  // WillRepeatedly() clauses) against the cardinality if this hasn't
  // been done before.  Prints a warning if there are too many or too
  // few actions.
  // L < mutex_
  void CheckActionCountIfNotDone() const {
    bool should_check = false;
    {
      MutexLock l(&mutex_);
      if (!action_count_checked_) {
        action_count_checked_ = true;
        should_check = true;
      }
    }

    if (should_check) {
      if (!cardinality_specified_) {
        // The cardinality was inferred - no need to check the action
        // count against it.
        return;
      }

      // The cardinality was explicitly specified.
      const int action_count = static_cast<int>(actions_.size());
      const int upper_bound = cardinality().ConservativeUpperBound();
      const int lower_bound = cardinality().ConservativeLowerBound();
      bool too_many;  // True if there are too many actions, or false
                      // if there are too few.
      if (action_count > upper_bound ||
          (action_count == upper_bound && repeated_action_specified_)) {
        too_many = true;
      } else if (0 < action_count && action_count < lower_bound &&
                 !repeated_action_specified_) {
        too_many = false;
      } else {
        return;
      }

      ::std::stringstream ss;
      DescribeLocationTo(&ss);
      ss << "Too " << (too_many ? "many" : "few")
         << " actions specified.\n"
         << "Expected to be ";
      cardinality().DescribeTo(&ss);
      ss << ", but has " << (too_many ? "" : "only ")
         << action_count << " WillOnce()"
         << (action_count == 1 ? "" : "s");
      if (repeated_action_specified_) {
        ss << " and a WillRepeatedly()";
      }
      ss << ".";
      Log(WARNING, ss.str(), -1);  // -1 means "don't print stack trace".
    }
  }

  // All the fields below won't change once the EXPECT_CALL()
  // statement finishes.
  FunctionMockerBase<F>* const owner_;
  ArgumentMatcherTuple matchers_;
  Matcher<const ArgumentTuple&> extra_matcher_;
  std::vector<Action<F> > actions_;
  bool repeated_action_specified_;  // True if a WillRepeatedly() was specified.
  Action<F> repeated_action_;
  bool retires_on_saturation_;
  Clause last_clause_;
  mutable bool action_count_checked_;  // Under mutex_.
  mutable Mutex mutex_;  // Protects action_count_checked_.
};  // class Expectation

// A MockSpec object is used by ON_CALL() or EXPECT_CALL() for
// specifying the default behavior of, or expectation on, a mock
// function.

// Note: class MockSpec really belongs to the ::testing namespace.
// However if we define it in ::testing, MSVC will complain when
// classes in ::testing::internal declare it as a friend class
// template.  To workaround this compiler bug, we define MockSpec in
// ::testing::internal and import it into ::testing.

template <typename F>
class MockSpec {
 public:
  typedef typename internal::Function<F>::ArgumentTuple ArgumentTuple;
  typedef typename internal::Function<F>::ArgumentMatcherTuple
      ArgumentMatcherTuple;

  // Constructs a MockSpec object, given the function mocker object
  // that the spec is associated with.
  explicit MockSpec(internal::FunctionMockerBase<F>* function_mocker)
      : function_mocker_(function_mocker) {}

  // Adds a new default action spec to the function mocker and returns
  // the newly created spec.
  internal::DefaultActionSpec<F>& InternalDefaultActionSetAt(
      const char* file, int line, const char* obj, const char* call) {
    LogWithLocation(internal::INFO, file, line,
        string("ON_CALL(") + obj + ", " + call + ") invoked");
    return function_mocker_->AddNewDefaultActionSpec(file, line, matchers_);
  }

  // Adds a new expectation spec to the function mocker and returns
  // the newly created spec.
  internal::Expectation<F>& InternalExpectedAt(
      const char* file, int line, const char* obj, const char* call) {
    LogWithLocation(internal::INFO, file, line,
        string("EXPECT_CALL(") + obj + ", " + call + ") invoked");
    return function_mocker_->AddNewExpectation(file, line, matchers_);
  }

 private:
  template <typename Function>
  friend class internal::FunctionMocker;

  void SetMatchers(const ArgumentMatcherTuple& matchers) {
    matchers_ = matchers;
  }

  // Logs a message including file and line number information.
  void LogWithLocation(testing::internal::LogSeverity severity,
                       const char* file, int line,
                       const string& message) {
    ::std::ostringstream s;
    s << file << ":" << line << ": " << message << ::std::endl;
    Log(severity, s.str(), 0);
  }

  // The function mocker that owns this spec.
  internal::FunctionMockerBase<F>* const function_mocker_;
  // The argument matchers specified in the spec.
  ArgumentMatcherTuple matchers_;
};  // class MockSpec

// MSVC warns about using 'this' in base member initializer list, so
// we need to temporarily disable the warning.  We have to do it for
// the entire class to suppress the warning, even though it's about
// the constructor only.

#ifdef _MSC_VER
#pragma warning(push)          // Saves the current warning state.
#pragma warning(disable:4355)  // Temporarily disables warning 4355.
#endif  // _MSV_VER

// The base of the function mocker class for the given function type.
// We put the methods in this class instead of its child to avoid code
// bloat.
template <typename F>
class FunctionMockerBase : public UntypedFunctionMockerBase {
 public:
  typedef typename Function<F>::Result Result;
  typedef typename Function<F>::ArgumentTuple ArgumentTuple;
  typedef typename Function<F>::ArgumentMatcherTuple ArgumentMatcherTuple;

  FunctionMockerBase() : mock_obj_(NULL), name_(""), current_spec_(this) {}

  // The destructor verifies that all expectations on this mock
  // function have been satisfied.  If not, it will report Google Test
  // non-fatal failures for the violations.
  // L < g_gmock_mutex
  virtual ~FunctionMockerBase() {
    MutexLock l(&g_gmock_mutex);
    VerifyAndClearExpectationsLocked();
    Mock::UnregisterLocked(this);
  }

  // Returns the ON_CALL spec that matches this mock function with the
  // given arguments; returns NULL if no matching ON_CALL is found.
  // L = *
  const DefaultActionSpec<F>* FindDefaultActionSpec(
      const ArgumentTuple& args) const {
    for (typename std::vector<DefaultActionSpec<F> >::const_reverse_iterator it
             = default_actions_.rbegin();
         it != default_actions_.rend(); ++it) {
      const DefaultActionSpec<F>& spec = *it;
      if (spec.Matches(args))
        return &spec;
    }

    return NULL;
  }

  // Performs the default action of this mock function on the given arguments
  // and returns the result. Asserts with a helpful call descrption if there is
  // no valid return value. This method doesn't depend on the mutable state of
  // this object, and thus can be called concurrently without locking.
  // L = *
  Result PerformDefaultAction(const ArgumentTuple& args,
                              const string& call_description) const {
    const DefaultActionSpec<F>* const spec = FindDefaultActionSpec(args);
    if (spec != NULL) {
      return spec->GetAction().Perform(args);
    }
    Assert(DefaultValue<Result>::Exists(), "", -1,
           call_description + "\n    The mock function has no default action "
           "set, and its return type has no default value set.");
    return DefaultValue<Result>::Get();
  }

  // Registers this function mocker and the mock object owning it;
  // returns a reference to the function mocker object.  This is only
  // called by the ON_CALL() and EXPECT_CALL() macros.
  FunctionMocker<F>& RegisterOwner(const void* mock_obj) {
    Mock::Register(mock_obj, this);
    return *down_cast<FunctionMocker<F>*>(this);
  }

  // The following two functions are from UntypedFunctionMockerBase.

  // Verifies that all expectations on this mock function have been
  // satisfied.  Reports one or more Google Test non-fatal failures
  // and returns false if not.
  // L >= g_gmock_mutex
  virtual bool VerifyAndClearExpectationsLocked();

  // Clears the ON_CALL()s set on this mock function.
  // L >= g_gmock_mutex
  virtual void ClearDefaultActionsLocked() {
    g_gmock_mutex.AssertHeld();
    default_actions_.clear();
  }

  // Sets the name of the function being mocked.  Will be called upon
  // each invocation of this mock function.
  // L < g_gmock_mutex
  void SetOwnerAndName(const void* mock_obj, const char* name) {
    // We protect name_ under g_gmock_mutex in case this mock function
    // is called from two threads concurrently.
    MutexLock l(&g_gmock_mutex);
    mock_obj_ = mock_obj;
    name_ = name;
  }

  // Returns the address of the mock object this method belongs to.
  // Must be called after SetOwnerAndName() has been called.
  // L < g_gmock_mutex
  const void* MockObject() const {
    const void* mock_obj;
    {
      // We protect mock_obj_ under g_gmock_mutex in case this mock
      // function is called from two threads concurrently.
      MutexLock l(&g_gmock_mutex);
      mock_obj = mock_obj_;
    }
    return mock_obj;
  }

  // Returns the name of the function being mocked.  Must be called
  // after SetOwnerAndName() has been called.
  // L < g_gmock_mutex
  const char* Name() const {
    const char* name;
    {
      // We protect name_ under g_gmock_mutex in case this mock
      // function is called from two threads concurrently.
      MutexLock l(&g_gmock_mutex);
      name = name_;
    }
    return name;
  }
 protected:
  template <typename Function>
  friend class MockSpec;

  template <typename R, typename Function>
  friend class InvokeWithHelper;

  // Returns the result of invoking this mock function with the given
  // arguments.  This function can be safely called from multiple
  // threads concurrently.
  // L < g_gmock_mutex
  Result InvokeWith(const ArgumentTuple& args) {
    return InvokeWithHelper<Result, F>::InvokeAndPrintResult(this, args);
  }

  // Adds and returns a default action spec for this mock function.
  DefaultActionSpec<F>& AddNewDefaultActionSpec(
      const char* file, int line,
      const ArgumentMatcherTuple& m) {
    default_actions_.push_back(DefaultActionSpec<F>(file, line, m));
    return default_actions_.back();
  }

  // Adds and returns an expectation spec for this mock function.
  Expectation<F>& AddNewExpectation(
      const char* file, int line,
      const ArgumentMatcherTuple& m) {
    const linked_ptr<Expectation<F> > expectation(
        new Expectation<F>(this, file, line, m));
    expectations_.push_back(expectation);

    // Adds this expectation into the implicit sequence if there is one.
    Sequence* const implicit_sequence = g_gmock_implicit_sequence.get();
    if (implicit_sequence != NULL) {
      implicit_sequence->AddExpectation(expectation);
    }

    return *expectation;
  }

  // The current spec (either default action spec or expectation spec)
  // being described on this function mocker.
  MockSpec<F>& current_spec() { return current_spec_; }
 private:
  template <typename Func> friend class Expectation;

  typedef std::vector<internal::linked_ptr<Expectation<F> > > Expectations;

  // Gets the internal::linked_ptr<ExpectationBase> object that co-owns 'exp'.
  internal::linked_ptr<ExpectationBase> GetLinkedExpectationBase(
      Expectation<F>* exp) {
    for (typename Expectations::const_iterator it = expectations_.begin();
         it != expectations_.end(); ++it) {
      if (it->get() == exp) {
        return *it;
      }
    }

    Assert(false, __FILE__, __LINE__, "Cannot find expectation.");
    return internal::linked_ptr<ExpectationBase>(NULL);
    // The above statement is just to make the code compile, and will
    // never be executed.
  }

  // Some utilities needed for implementing InvokeWith().

  // Describes what default action will be performed for the given
  // arguments.
  // L = *
  void DescribeDefaultActionTo(const ArgumentTuple& args,
                               ::std::ostream* os) const {
    const DefaultActionSpec<F>* const spec = FindDefaultActionSpec(args);

    if (spec == NULL) {
      *os << (internal::type_equals<Result, void>::value ?
              "returning directly.\n" :
              "returning default value.\n");
    } else {
      *os << "taking default action specified at:\n"
          << spec->file() << ":" << spec->line() << ":\n";
    }
  }

  // Writes a message that the call is uninteresting (i.e. neither
  // explicitly expected nor explicitly unexpected) to the given
  // ostream.
  // L < g_gmock_mutex
  void DescribeUninterestingCall(const ArgumentTuple& args,
                                 ::std::ostream* os) const {
    *os << "Uninteresting mock function call - ";
    DescribeDefaultActionTo(args, os);
    *os << "    Function call: " << Name();
    UniversalPrinter<ArgumentTuple>::Print(args, os);
  }

  // Critical section: We must find the matching expectation and the
  // corresponding action that needs to be taken in an ATOMIC
  // transaction.  Otherwise another thread may call this mock
  // method in the middle and mess up the state.
  //
  // However, performing the action has to be left out of the critical
  // section.  The reason is that we have no control on what the
  // action does (it can invoke an arbitrary user function or even a
  // mock function) and excessive locking could cause a dead lock.
  // L < g_gmock_mutex
  bool FindMatchingExpectationAndAction(
      const ArgumentTuple& args, Expectation<F>** exp, Action<F>* action,
      bool* is_excessive, ::std::ostream* what, ::std::ostream* why) {
    MutexLock l(&g_gmock_mutex);
    *exp = this->FindMatchingExpectationLocked(args);
    if (*exp == NULL) {  // A match wasn't found.
      *action = DoDefault();
      this->FormatUnexpectedCallMessageLocked(args, what, why);
      return false;
    }

    // This line must be done before calling GetActionForArguments(),
    // which will increment the call count for *exp and thus affect
    // its saturation status.
    *is_excessive = (*exp)->IsSaturated();
    *action = (*exp)->GetActionForArguments(this, args, what, why);
    return true;
  }

  // Returns the expectation that matches the arguments, or NULL if no
  // expectation matches them.
  // L >= g_gmock_mutex
  Expectation<F>* FindMatchingExpectationLocked(
      const ArgumentTuple& args) const {
    g_gmock_mutex.AssertHeld();
    for (typename Expectations::const_reverse_iterator it =
             expectations_.rbegin();
         it != expectations_.rend(); ++it) {
      Expectation<F>* const exp = it->get();
      if (exp->ShouldHandleArguments(args)) {
        return exp;
      }
    }
    return NULL;
  }

  // Returns a message that the arguments don't match any expectation.
  // L >= g_gmock_mutex
  void FormatUnexpectedCallMessageLocked(const ArgumentTuple& args,
                                         ::std::ostream* os,
                                         ::std::ostream* why) const {
    g_gmock_mutex.AssertHeld();
    *os << "\nUnexpected mock function call - ";
    DescribeDefaultActionTo(args, os);
    PrintTriedExpectationsLocked(args, why);
  }

  // Prints a list of expectations that have been tried against the
  // current mock function call.
  // L >= g_gmock_mutex
  void PrintTriedExpectationsLocked(const ArgumentTuple& args,
                                    ::std::ostream* why) const {
    g_gmock_mutex.AssertHeld();
    const int count = static_cast<int>(expectations_.size());
    *why << "Google Mock tried the following " << count << " "
         << (count == 1 ? "expectation, but it didn't match" :
             "expectations, but none matched")
         << ":\n";
    for (int i = 0; i < count; i++) {
      *why << "\n";
      expectations_[i]->DescribeLocationTo(why);
      if (count > 1) {
        *why << "tried expectation #" << i;
      }
      *why << "\n";
      expectations_[i]->DescribeMatchResultTo(args, why);
      expectations_[i]->DescribeCallCountTo(why);
    }
  }

  // Address of the mock object this mock method belongs to.
  const void* mock_obj_;  // Protected by g_gmock_mutex.

  // Name of the function being mocked.
  const char* name_;  // Protected by g_gmock_mutex.

  // The current spec (either default action spec or expectation spec)
  // being described on this function mocker.
  MockSpec<F> current_spec_;

  // All default action specs for this function mocker.
  std::vector<DefaultActionSpec<F> > default_actions_;
  // All expectations for this function mocker.
  Expectations expectations_;
};  // class FunctionMockerBase

#ifdef _MSC_VER
#pragma warning(pop)  // Restores the warning state.
#endif  // _MSV_VER

// Implements methods of FunctionMockerBase.

// Verifies that all expectations on this mock function have been
// satisfied.  Reports one or more Google Test non-fatal failures and
// returns false if not.
// L >= g_gmock_mutex
template <typename F>
bool FunctionMockerBase<F>::VerifyAndClearExpectationsLocked() {
  g_gmock_mutex.AssertHeld();
  bool expectations_met = true;
  for (typename Expectations::const_iterator it = expectations_.begin();
       it != expectations_.end(); ++it) {
    Expectation<F>* const exp = it->get();

    if (exp->IsOverSaturated()) {
      // There was an upper-bound violation.  Since the error was
      // already reported when it occurred, there is no need to do
      // anything here.
      expectations_met = false;
    } else if (!exp->IsSatisfied()) {
      expectations_met = false;
      ::std::stringstream ss;
      ss << "Actual function call count doesn't match this expectation.\n";
      // No need to show the source file location of the expectation
      // in the description, as the Expect() call that follows already
      // takes care of it.
      exp->DescribeCallCountTo(&ss);
      Expect(false, exp->file(), exp->line(), ss.str());
    }
  }
  expectations_.clear();
  return expectations_met;
}

// Reports an uninteresting call (whose description is in msg) in the
// manner specified by 'reaction'.
void ReportUninterestingCall(CallReaction reaction, const string& msg);

// When an uninteresting or unexpected mock function is called, we
// want to print its return value to assist the user debugging.  Since
// there's nothing to print when the function returns void, we need to
// specialize the logic of FunctionMockerBase<F>::InvokeWith() for
// void return values.
//
// C++ doesn't allow us to specialize a member function template
// unless we also specialize its enclosing class, so we had to let
// InvokeWith() delegate its work to a helper class InvokeWithHelper,
// which can then be specialized.
//
// Note that InvokeWithHelper must be a class template (as opposed to
// a function template), as only class templates can be partially
// specialized.
template <typename Result, typename F>
class InvokeWithHelper {
 public:
  typedef typename Function<F>::ArgumentTuple ArgumentTuple;

  // Calculates the result of invoking the function mocked by mocker
  // with the given arguments, prints it, and returns it.
  // L < g_gmock_mutex
  static Result InvokeAndPrintResult(
      FunctionMockerBase<F>* mocker,
      const ArgumentTuple& args) {
    if (mocker->expectations_.size() == 0) {
      // No expectation is set on this mock method - we have an
      // uninteresting call.

      // Warns about the uninteresting call.
      ::std::stringstream ss;
      mocker->DescribeUninterestingCall(args, &ss);

      // We must get Google Mock's reaction on uninteresting calls
      // made on this mock object BEFORE performing the action,
      // because the action may DELETE the mock object and make the
      // following expression meaningless.
      const CallReaction reaction =
          Mock::GetReactionOnUninterestingCalls(mocker->MockObject());

      // Calculates the function result.
      Result result = mocker->PerformDefaultAction(args, ss.str());

      // Prints the function result.
      ss << "\n          Returns: ";
      UniversalPrinter<Result>::Print(result, &ss);
      ReportUninterestingCall(reaction, ss.str());

      return result;
    }

    bool is_excessive = false;
    ::std::stringstream ss;
    ::std::stringstream why;
    ::std::stringstream loc;
    Action<F> action;
    Expectation<F>* exp;

    // The FindMatchingExpectationAndAction() function acquires and
    // releases g_gmock_mutex.
    const bool found = mocker->FindMatchingExpectationAndAction(
        args, &exp, &action, &is_excessive, &ss, &why);
    ss << "    Function call: " << mocker->Name();
    UniversalPrinter<ArgumentTuple>::Print(args, &ss);
    // In case the action deletes a piece of the expectation, we
    // generate the message beforehand.
    if (found && !is_excessive) {
      exp->DescribeLocationTo(&loc);
    }
    Result result = action.IsDoDefault() ?
        mocker->PerformDefaultAction(args, ss.str())
        : action.Perform(args);
    ss << "\n          Returns: ";
    UniversalPrinter<Result>::Print(result, &ss);
    ss << "\n" << why.str();

    if (found) {
      if (is_excessive) {
        // We had an upper-bound violation and the failure message is in ss.
        Expect(false, exp->file(), exp->line(), ss.str());
      } else {
        // We had an expected call and the matching expectation is
        // described in ss.
        Log(INFO, loc.str() + ss.str(), 3);
      }
    } else {
      // No expectation matches this call - reports a failure.
      Expect(false, NULL, -1, ss.str());
    }
    return result;
  }
};  // class InvokeWithHelper

// This specialization helps to implement
// FunctionMockerBase<F>::InvokeWith() for void-returning functions.
template <typename F>
class InvokeWithHelper<void, F> {
 public:
  typedef typename Function<F>::ArgumentTuple ArgumentTuple;

  // Invokes the function mocked by mocker with the given arguments.
  // L < g_gmock_mutex
  static void InvokeAndPrintResult(FunctionMockerBase<F>* mocker,
                                   const ArgumentTuple& args) {
    const int count = static_cast<int>(mocker->expectations_.size());
    if (count == 0) {
      // No expectation is set on this mock method - we have an
      // uninteresting call.
      ::std::stringstream ss;
      mocker->DescribeUninterestingCall(args, &ss);

      // We must get Google Mock's reaction on uninteresting calls
      // made on this mock object BEFORE performing the action,
      // because the action may DELETE the mock object and make the
      // following expression meaningless.
      const CallReaction reaction =
          Mock::GetReactionOnUninterestingCalls(mocker->MockObject());

      mocker->PerformDefaultAction(args, ss.str());
      ReportUninterestingCall(reaction, ss.str());
      return;
    }

    bool is_excessive = false;
    ::std::stringstream ss;
    ::std::stringstream why;
    ::std::stringstream loc;
    Action<F> action;
    Expectation<F>* exp;

    // The FindMatchingExpectationAndAction() function acquires and
    // releases g_gmock_mutex.
    const bool found = mocker->FindMatchingExpectationAndAction(
        args, &exp, &action, &is_excessive, &ss, &why);
    ss << "    Function call: " << mocker->Name();
    UniversalPrinter<ArgumentTuple>::Print(args, &ss);
    ss << "\n" << why.str();
    // In case the action deletes a piece of the expectation, we
    // generate the message beforehand.
    if (found && !is_excessive) {
      exp->DescribeLocationTo(&loc);
    }
    if (action.IsDoDefault()) {
      mocker->PerformDefaultAction(args, ss.str());
    } else {
      action.Perform(args);
    }

    if (found) {
      // A matching expectation and corresponding action were found.
      if (is_excessive) {
        // We had an upper-bound violation and the failure message is in ss.
        Expect(false, exp->file(), exp->line(), ss.str());
      } else {
        // We had an expected call and the matching expectation is
        // described in ss.
        Log(INFO, loc.str() + ss.str(), 3);
      }
    } else {
      // No matching expectation was found - reports an error.
      Expect(false, NULL, -1, ss.str());
    }
  }
};  // class InvokeWithHelper<void, F>

}  // namespace internal

// The style guide prohibits "using" statements in a namespace scope
// inside a header file.  However, the MockSpec class template is
// meant to be defined in the ::testing namespace.  The following line
// is just a trick for working around a bug in MSVC 8.0, which cannot
// handle it if we define MockSpec in ::testing.
using internal::MockSpec;

// Const(x) is a convenient function for obtaining a const reference
// to x.  This is useful for setting expectations on an overloaded
// const mock method, e.g.
//
//   class MockFoo : public FooInterface {
//    public:
//     MOCK_METHOD0(Bar, int());
//     MOCK_CONST_METHOD0(Bar, int&());
//   };
//
//   MockFoo foo;
//   // Expects a call to non-const MockFoo::Bar().
//   EXPECT_CALL(foo, Bar());
//   // Expects a call to const MockFoo::Bar().
//   EXPECT_CALL(Const(foo), Bar());
template <typename T>
inline const T& Const(const T& x) { return x; }

}  // namespace testing

// A separate macro is required to avoid compile errors when the name
// of the method used in call is a result of macro expansion.
// See CompilesWithMethodNameExpandedFromMacro tests in
// internal/gmock-spec-builders_test.cc for more details.
#define GMOCK_ON_CALL_IMPL_(obj, call) \
    ((obj).gmock_##call).InternalDefaultActionSetAt(__FILE__, __LINE__, \
                                                    #obj, #call)
#define ON_CALL(obj, call) GMOCK_ON_CALL_IMPL_(obj, call)

#define GMOCK_EXPECT_CALL_IMPL_(obj, call) \
    ((obj).gmock_##call).InternalExpectedAt(__FILE__, __LINE__, #obj, #call)
#define EXPECT_CALL(obj, call) GMOCK_EXPECT_CALL_IMPL_(obj, call)

#endif  // GMOCK_INCLUDE_GMOCK_GMOCK_SPEC_BUILDERS_H_

#endif