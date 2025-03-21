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
// This file implements the spec builder syntax (ON_CALL and
// EXPECT_CALL).

#include <gmock/gmock-spec-builders.h>

#include <set>
#include <gtest/gtest.h>

namespace testing {
namespace internal {

// Protects the mock object registry (in class Mock), all function
// mockers, and all expectations.
Mutex g_gmock_mutex(Mutex::NO_CONSTRUCTOR_NEEDED_FOR_STATIC_MUTEX);

// Constructs an ExpectationBase object.
ExpectationBase::ExpectationBase(const char* file, int line)
    : file_(file),
      line_(line),
      cardinality_specified_(false),
      cardinality_(Exactly(1)),
      call_count_(0),
      retired_(false) {
}

// Destructs an ExpectationBase object.
ExpectationBase::~ExpectationBase() {}

// Explicitly specifies the cardinality of this expectation.  Used by
// the subclasses to implement the .Times() clause.
void ExpectationBase::SpecifyCardinality(const Cardinality& cardinality) {
  cardinality_specified_ = true;
  cardinality_ = cardinality;
}

// Retires all pre-requisites of this expectation.
void ExpectationBase::RetireAllPreRequisites() {
  if (is_retired()) {
    // We can take this short-cut as we never retire an expectation
    // until we have retired all its pre-requisites.
    return;
  }

  for (ExpectationBaseSet::const_iterator it =
           immediate_prerequisites_.begin();
       it != immediate_prerequisites_.end();
       ++it) {
    ExpectationBase* const prerequisite = (*it).get();
    if (!prerequisite->is_retired()) {
      prerequisite->RetireAllPreRequisites();
      prerequisite->Retire();
    }
  }
}

// Returns true iff all pre-requisites of this expectation have been
// satisfied.
// L >= g_gmock_mutex
bool ExpectationBase::AllPrerequisitesAreSatisfied() const {
  g_gmock_mutex.AssertHeld();
  for (ExpectationBaseSet::const_iterator it = immediate_prerequisites_.begin();
       it != immediate_prerequisites_.end(); ++it) {
    if (!(*it)->IsSatisfied() ||
        !(*it)->AllPrerequisitesAreSatisfied())
      return false;
  }
  return true;
}

// Adds unsatisfied pre-requisites of this expectation to 'result'.
// L >= g_gmock_mutex
void ExpectationBase::FindUnsatisfiedPrerequisites(
    ExpectationBaseSet* result) const {
  g_gmock_mutex.AssertHeld();
  for (ExpectationBaseSet::const_iterator it = immediate_prerequisites_.begin();
       it != immediate_prerequisites_.end(); ++it) {
    if ((*it)->IsSatisfied()) {
      // If *it is satisfied and has a call count of 0, some of its
      // pre-requisites may not be satisfied yet.
      if ((*it)->call_count_ == 0) {
        (*it)->FindUnsatisfiedPrerequisites(result);
      }
    } else {
      // Now that we know *it is unsatisfied, we are not so interested
      // in whether its pre-requisites are satisfied.  Therefore we
      // don't recursively call FindUnsatisfiedPrerequisites() here.
      result->insert(*it);
    }
  }
}

// Points to the implicit sequence introduced by a living InSequence
// object (if any) in the current thread or NULL.
ThreadLocal<Sequence*> g_gmock_implicit_sequence;

// Reports an uninteresting call (whose description is in msg) in the
// manner specified by 'reaction'.
void ReportUninterestingCall(CallReaction reaction, const string& msg) {
  switch (reaction) {
    case ALLOW:
      Log(INFO, msg, 4);
      break;
    case WARN:
      Log(WARNING, msg, 4);
      break;
    default:  // FAIL
      Expect(false, NULL, -1, msg);
  }
}

}  // namespace internal

// Class Mock.

namespace {

typedef std::set<internal::UntypedFunctionMockerBase*> FunctionMockers;
typedef std::map<const void*, FunctionMockers> MockObjectRegistry;

// Maps a mock object to the set of mock methods it owns.  Protected
// by g_gmock_mutex.
MockObjectRegistry g_mock_object_registry;

// Maps a mock object to the reaction Google Mock should have when an
// uninteresting method is called.  Protected by g_gmock_mutex.
std::map<const void*, internal::CallReaction> g_uninteresting_call_reaction;

// Sets the reaction Google Mock should have when an uninteresting
// method of the given mock object is called.
// L < g_gmock_mutex
void SetReactionOnUninterestingCalls(const void* mock_obj,
                                     internal::CallReaction reaction) {
  internal::MutexLock l(&internal::g_gmock_mutex);
  g_uninteresting_call_reaction[mock_obj] = reaction;
}

}  // namespace

// Tells Google Mock to allow uninteresting calls on the given mock
// object.
// L < g_gmock_mutex
void Mock::AllowUninterestingCalls(const void* mock_obj) {
  SetReactionOnUninterestingCalls(mock_obj, internal::ALLOW);
}

// Tells Google Mock to warn the user about uninteresting calls on the
// given mock object.
// L < g_gmock_mutex
void Mock::WarnUninterestingCalls(const void* mock_obj) {
  SetReactionOnUninterestingCalls(mock_obj, internal::WARN);
}

// Tells Google Mock to fail uninteresting calls on the given mock
// object.
// L < g_gmock_mutex
void Mock::FailUninterestingCalls(const void* mock_obj) {
  SetReactionOnUninterestingCalls(mock_obj, internal::FAIL);
}

// Tells Google Mock the given mock object is being destroyed and its
// entry in the call-reaction table should be removed.
// L < g_gmock_mutex
void Mock::UnregisterCallReaction(const void* mock_obj) {
  internal::MutexLock l(&internal::g_gmock_mutex);
  g_uninteresting_call_reaction.erase(mock_obj);
}

// Returns the reaction Google Mock will have on uninteresting calls
// made on the given mock object.
// L < g_gmock_mutex
internal::CallReaction Mock::GetReactionOnUninterestingCalls(
    const void* mock_obj) {
  internal::MutexLock l(&internal::g_gmock_mutex);
  return (g_uninteresting_call_reaction.count(mock_obj) == 0) ?
      internal::WARN : g_uninteresting_call_reaction[mock_obj];
}

// Verifies and clears all expectations on the given mock object.  If
// the expectations aren't satisfied, generates one or more Google
// Test non-fatal failures and returns false.
// L < g_gmock_mutex
bool Mock::VerifyAndClearExpectations(void* mock_obj) {
  internal::MutexLock l(&internal::g_gmock_mutex);
  return VerifyAndClearExpectationsLocked(mock_obj);
}

// Verifies all expectations on the given mock object and clears its
// default actions and expectations.  Returns true iff the
// verification was successful.
// L < g_gmock_mutex
bool Mock::VerifyAndClear(void* mock_obj) {
  internal::MutexLock l(&internal::g_gmock_mutex);
  ClearDefaultActionsLocked(mock_obj);
  return VerifyAndClearExpectationsLocked(mock_obj);
}

// Verifies and clears all expectations on the given mock object.  If
// the expectations aren't satisfied, generates one or more Google
// Test non-fatal failures and returns false.
// L >= g_gmock_mutex
bool Mock::VerifyAndClearExpectationsLocked(void* mock_obj) {
  internal::g_gmock_mutex.AssertHeld();
  if (g_mock_object_registry.count(mock_obj) == 0) {
    // No EXPECT_CALL() was set on the given mock object.
    return true;
  }

  // Verifies and clears the expectations on each mock method in the
  // given mock object.
  bool expectations_met = true;
  FunctionMockers& mockers = g_mock_object_registry[mock_obj];
  for (FunctionMockers::const_iterator it = mockers.begin();
       it != mockers.end(); ++it) {
    if (!(*it)->VerifyAndClearExpectationsLocked()) {
      expectations_met = false;
    }
  }

  // We don't clear the content of mockers, as they may still be
  // needed by ClearDefaultActionsLocked().
  return expectations_met;
}

// Registers a mock object and a mock method it owns.
// L < g_gmock_mutex
void Mock::Register(const void* mock_obj,
                    internal::UntypedFunctionMockerBase* mocker) {
  internal::MutexLock l(&internal::g_gmock_mutex);
  g_mock_object_registry[mock_obj].insert(mocker);
}

// Unregisters a mock method; removes the owning mock object from the
// registry when the last mock method associated with it has been
// unregistered.  This is called only in the destructor of
// FunctionMockerBase.
// L >= g_gmock_mutex
void Mock::UnregisterLocked(internal::UntypedFunctionMockerBase* mocker) {
  internal::g_gmock_mutex.AssertHeld();
  for (MockObjectRegistry::iterator it = g_mock_object_registry.begin();
       it != g_mock_object_registry.end(); ++it) {
    FunctionMockers& mockers = it->second;
    if (mockers.erase(mocker) > 0) {
      // mocker was in mockers and has been just removed.
      if (mockers.empty()) {
        g_mock_object_registry.erase(it);
      }
      return;
    }
  }
}

// Clears all ON_CALL()s set on the given mock object.
// L >= g_gmock_mutex
void Mock::ClearDefaultActionsLocked(void* mock_obj) {
  internal::g_gmock_mutex.AssertHeld();

  if (g_mock_object_registry.count(mock_obj) == 0) {
    // No ON_CALL() was set on the given mock object.
    return;
  }

  // Clears the default actions for each mock method in the given mock
  // object.
  FunctionMockers& mockers = g_mock_object_registry[mock_obj];
  for (FunctionMockers::const_iterator it = mockers.begin();
       it != mockers.end(); ++it) {
    (*it)->ClearDefaultActionsLocked();
  }

  // We don't clear the content of mockers, as they may still be
  // needed by VerifyAndClearExpectationsLocked().
}

// Adds an expectation to a sequence.
void Sequence::AddExpectation(
    const internal::linked_ptr<internal::ExpectationBase>& expectation) const {
  if (*last_expectation_ != expectation) {
    if (*last_expectation_ != NULL) {
      expectation->immediate_prerequisites_.insert(*last_expectation_);
    }
    *last_expectation_ = expectation;
  }
}

// Creates the implicit sequence if there isn't one.
InSequence::InSequence() {
  if (internal::g_gmock_implicit_sequence.get() == NULL) {
    internal::g_gmock_implicit_sequence.set(new Sequence);
    sequence_created_ = true;
  } else {
    sequence_created_ = false;
  }
}

// Deletes the implicit sequence if it was created by the constructor
// of this object.
InSequence::~InSequence() {
  if (sequence_created_) {
    delete internal::g_gmock_implicit_sequence.get();
    internal::g_gmock_implicit_sequence.set(NULL);
  }
}

}  // namespace testing

#endif