//
//  StateMachine.h
//  StateMachine
//
//  Created by Michele Gruppioni on 21/03/21.
//

#ifndef StateMachine_h
#define StateMachine_h

#include <memory>

#include <iostream>
int n = 0;

extern unsigned long millis();

template <class Context>
class State;

template <class Context>
class ThrottledState;

template <class Context>
class DelayedState;

template <class Context>
class RecoveryState;

// StateMachine

template <class Context>
class StateMachine
{
public:
  Context context;
  
private:
  std::shared_ptr<State<Context>> state;
  
public:
  ~StateMachine();
  StateMachine(Context context, std::shared_ptr<State<Context>> initialState);
  void setState(std::shared_ptr<State<Context>> newState);
  void handleState();
};

template <class Context>
StateMachine<Context>::~StateMachine() {
}

template <class Context>
StateMachine<Context>::StateMachine(Context context, std::shared_ptr<State<Context>> initialState) : context(context),
state(initialState)
{
}

template <class Context>
void StateMachine<Context>::setState(std::shared_ptr<State<Context>> newState)
{
  state = newState;
  //std::cout << "Prev: " << prevState << ", State:" << state << "\n";
}

template <class Context>
void StateMachine<Context>::handleState()
{
  state->handleState(std::shared_ptr<State<Context>>(this));
}

// State

template <class Context>
class State
{
public:
  State();
  virtual ~State();
  virtual void handleState(std::shared_ptr<StateMachine<Context>> stateMachine) = 0;
  
  std::shared_ptr<State<Context>> throttle(unsigned long interval);
  std::shared_ptr<State<Context>> delay(unsigned long interval);
  std::shared_ptr<State<Context>> recoveryAfter(unsigned long maxDuration, std::shared_ptr<State<Context>> recoveryState);
  std::shared_ptr<State<Context>> recoveryAfter(unsigned long maxDuration, std::shared_ptr<State<Context>> recoveryState, std::shared_ptr<State<Context>> recoverRecoveryState);
};

template <class Context>
State<Context>::State()
{
  n++;
  std::cout << n << "\n";
}

template <class Context>
State<Context>::~State()
{
  std::cout << "~Deleted State:" << this << "\n";
  n--;
  std::cout << n << "\n";
}

template <class Context>
std::shared_ptr<State<Context>> State<Context>::throttle(unsigned long interval)
{
  return std::shared_ptr<ThrottledState<Context>>(interval, this);
}

template <class Context>
std::shared_ptr<State<Context>> State<Context>::delay(unsigned long interval)
{
  return std::shared_ptr<DelayedState<Context>>(interval, this);
}

template <class Context>
std::shared_ptr<State<Context>> State<Context>::recoveryAfter
 (unsigned long maxDuration,
  std::shared_ptr<State<Context>> recoveryState)
{
  return std::shared_ptr<RecoveryState<Context>>(maxDuration, recoveryState, this);
}

template <class Context>
std::shared_ptr<State<Context>> State<Context>::recoveryAfter
(unsigned long maxDuration,
 std::shared_ptr<State<Context>> recoveryState,
 std::shared_ptr<State<Context>> recoverRecoveryState)
{
  return std::shared_ptr<RecoveryState<Context>>
  (
   maxDuration,
   std::shared_ptr<RecoveryState<Context>>(maxDuration, recoverRecoveryState, recoveryState),
   this
   );
}

// ThrottledState

template <typename Context>
class ThrottledState : public State<Context>
{
private:
  unsigned long interval = 0;
  std::shared_ptr<State<Context>> state = 0;
  unsigned long lastTimeRunned = 0;
  
public:
  ~ThrottledState();
  ThrottledState(unsigned long interval, std::shared_ptr<State<Context>> state);
  void handleState(std::shared_ptr<StateMachine<Context>> stateMachine);
};

template <class Context>
ThrottledState<Context>::~ThrottledState()
{
//  delete state;
//  state = 0;
}

template <class Context>
ThrottledState<Context>::ThrottledState(unsigned long interval, std::shared_ptr<State<Context>> state) :
interval(interval),
state(state)
{
}

template <class Context>
void ThrottledState<Context>::handleState(std::shared_ptr<StateMachine<Context>> stateMachine)
{
  if (millis() >= lastTimeRunned + interval)
  {
    lastTimeRunned = millis();
    state->handleState(stateMachine);
  }
}

// DelayedState

template <class Context>
class DelayedState : public State<Context>
{
private:
  unsigned long delayValue = 0;
  std::shared_ptr<State<Context>> state;
  unsigned long startTime = millis();
  
public:
  ~DelayedState();
  DelayedState(unsigned long delayValue, std::shared_ptr<State<Context>> state);
  void handleState(std::shared_ptr<StateMachine<Context>> stateMachine);
};

template <class Context>
DelayedState<Context>::~DelayedState() {
//  delete state;
//  state = 0;
}

template <class Context>
DelayedState<Context>::DelayedState(unsigned long delayValue, std::shared_ptr<State<Context>> state):
delayValue(delayValue),
state(state) {
}

template <class Context>
void DelayedState<Context>::handleState(std::shared_ptr<StateMachine<Context>> stateMachine) {
  if(millis() > startTime + delayValue) {
    stateMachine->setState(state);
  }
}

// RecoveryState

template <class Context>
class RecoveryState : public State<Context>
{
private:
  unsigned long maxDuration;
  std::shared_ptr<State<Context>> recoveryState;
  std::shared_ptr<State<Context>> state;
  unsigned long startTime = 0;
  
public:
  ~RecoveryState();
  RecoveryState(unsigned long maxDuration, std::shared_ptr<State<Context>> recoveryState, std::shared_ptr<State<Context>> state);
  void handleState(std::shared_ptr<StateMachine<Context>> stateMachine);
};

template <class Context>
RecoveryState<Context>::~RecoveryState() {
}

template <class Context>
RecoveryState<Context>::RecoveryState(unsigned long maxDuration, std::shared_ptr<State<Context>> recoveryState, std::shared_ptr<State<Context>> state):
maxDuration(maxDuration),
recoveryState(recoveryState),
state(state) {
}

template <class Context>
void RecoveryState<Context>::handleState(std::shared_ptr<StateMachine<Context>> stateMachine) {
  if (startTime == 0)
    startTime = millis();
  
  if (millis() > startTime + maxDuration) {
    stateMachine->setState(recoveryState);
  } else {
    state->handleState(stateMachine);
  }
}

#endif /* StateMachine_h */
