//
//  StateMachine.h
//  StateMachine
//
//  Created by Michele Gruppioni on 21/03/21.
//

#ifndef StateMachine_h
#define StateMachine_h

#include <memory>

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
  StateMachine(Context context, State<Context> *initialState);
  void setState(State<Context> *newState);
  void setState(std::shared_ptr<State<Context>> newState);
  void handleState();
};

template <class Context>
StateMachine<Context>::StateMachine(Context context, State<Context> *initialState) : context(context),
state(std::shared_ptr<State<Context>>(initialState))
{
}

template <class Context>
void StateMachine<Context>::setState(State<Context> *newState)
{
  state = std::shared_ptr<State<Context>>(newState);
}

template <class Context>
void StateMachine<Context>::setState(std::shared_ptr<State<Context>> newState)
{
  state = newState;
}

template <class Context>
void StateMachine<Context>::handleState()
{
  state->handleState(this);
}

// State

template <class Context>
class State
{
public:
  virtual ~State();
  virtual void handleState(StateMachine<Context> *stateContext) = 0;
  
  State<Context>* throttle(unsigned long interval);
  State<Context>* delay(unsigned long interval);
  State<Context>* recoveryAfter(unsigned long maxDuration, State<Context> *recoveryState);
  State<Context>* recoveryAfter(unsigned long maxDuration, State<Context> *recoveryState, State<Context> *recoverRecoveryState);
};

template <class Context>
State<Context>::~State()
{
}

template <class Context>
State<Context>* State<Context>::throttle(unsigned long interval)
{
  return new ThrottledState<Context>(interval, this);
}

template <class Context>
State<Context>* State<Context>::delay(unsigned long interval)
{
  return new DelayedState<Context>(interval, this);
}

template <class Context>
State<Context>* State<Context>::recoveryAfter(unsigned long maxDuration, State<Context> *recoveryState)
{
  return new RecoveryState<Context>(maxDuration, recoveryState, this);
}

template <class Context>
State<Context>* State<Context>::recoveryAfter(unsigned long maxDuration, State<Context> *recoveryState, State<Context> *recoverRecoveryState)
{
  return new RecoveryState<Context>
  (
   maxDuration,
   new RecoveryState<Context>(maxDuration, recoverRecoveryState, recoveryState),
   this
   );
}

// ThrottledState

template <typename Context>
class ThrottledState : public State<Context>
{
private:
  unsigned long interval;
  std::shared_ptr<State<Context>> state;
  unsigned long lastTimeRunned = 0;
  
public:
  ThrottledState(unsigned long interval, State<Context> *state);
  void handleState(StateMachine<Context> *stateMachine);
};

template <class Context>
ThrottledState<Context>::ThrottledState(unsigned long interval, State<Context> *state) :
interval(interval),
state(std::shared_ptr<State<Context>>(state))
{
}

template <class Context>
void ThrottledState<Context>::handleState(StateMachine<Context> *stateMachine)
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
  unsigned long delayValue;
  std::shared_ptr<State<Context>> state;
  unsigned long startTime = millis();
  
public:
  DelayedState(unsigned long delayValue, State<Context> *state);
  void handleState(StateMachine<Context> *stateMachine);
};

template <class Context>
DelayedState<Context>::DelayedState(unsigned long delayValue, State<Context> *state):
delayValue(delayValue),
state(std::shared_ptr<State<Context>>(state)) {
}

template <class Context>
void DelayedState<Context>::handleState(StateMachine<Context> *stateMachine) {
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
  RecoveryState(unsigned long maxDuration, State<Context> *recoveryState, State<Context> *state);
  void handleState(StateMachine<Context> *stateMachine);
};

template <class Context>
RecoveryState<Context>::RecoveryState(unsigned long maxDuration, State<Context> *recoveryState, State<Context> *state):
maxDuration(maxDuration),
recoveryState(std::shared_ptr<State<Context>>(recoveryState)),
state(std::shared_ptr<State<Context>>(state)) {
}

template <class Context>
void RecoveryState<Context>::handleState(StateMachine<Context> *stateMachine) {
  if (startTime == 0)
    startTime = millis();
  
  if (millis() > startTime + maxDuration) {
    stateMachine->setState(recoveryState);
  } else {
    state->handleState(stateMachine);
  }
}

#endif /* StateMachine_h */
