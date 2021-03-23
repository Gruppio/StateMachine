//
//  StateMachine.h
//  StateMachine
//
//  Created by Michele Gruppioni on 21/03/21.
//

#ifndef StateMachine_h
#define StateMachine_h

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
  State<Context> *state;
  
public:
  StateMachine(Context context, State<Context> *initialState);
  void setState(State<Context> *newState);
  void handleState();
};

template <class Context>
StateMachine<Context>::StateMachine(Context context, State<Context> *initialState) : context(context),
state(initialState)
{
}

template <class Context>
void StateMachine<Context>::setState(State<Context> *newState)
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
  virtual void handleState(StateMachine<Context> *stateContext) = 0;
  
  State<Context>* throttle(unsigned long interval);
  State<Context>* delay(unsigned long interval);
  State<Context>* recoveryAfter(unsigned long maxDuration, State<Context> *recoveryState);
};

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

// ThrottledState

template <typename Context>
class ThrottledState : public State<Context>
{
private:
  unsigned long interval;
  State<Context> *nextState;
  unsigned long lastTimeRunned = 0;
  
public:
  ThrottledState(unsigned long interval, State<Context> *nextState);
  void handleState(StateMachine<Context> *stateMachine);
};

template <class Context>
ThrottledState<Context>::ThrottledState(unsigned long interval, State<Context> *nextState) :
interval(interval),
nextState(nextState)
{
}

template <class Context>
void ThrottledState<Context>::handleState(StateMachine<Context> *stateMachine)
{
  if (millis() >= lastTimeRunned + interval)
  {
    lastTimeRunned = millis();
    nextState->handleState(stateMachine);
  }
}

// DelayedState

template <class Context>
class DelayedState : public State<Context>
{
private:
  unsigned long delayValue;
  State<Context> *nextState;
  unsigned long startTime = millis();
  
public:
  DelayedState(unsigned long delayValue, State<Context> *nextState);
  void handleState(StateMachine<Context> *stateMachine);
};

template <class Context>
DelayedState<Context>::DelayedState(unsigned long delayValue, State<Context> *nextState):
delayValue(delayValue),
nextState(nextState) {
}

template <class Context>
void DelayedState<Context>::handleState(StateMachine<Context> *stateMachine) {
  if(millis() > startTime + delayValue) {
    stateMachine->setState(nextState);
  }
}

// RecoveryState

template <class Context>
class RecoveryState : public State<Context>
{
private:
  unsigned long maxDuration;
  State<Context> *recoveryState;
  State<Context> *state;
  unsigned long startTime = 0;
  
public:
  RecoveryState(unsigned long maxDuration, State<Context> *recoveryState, State<Context> *state);
  void handleState(StateMachine<Context> *stateMachine);
};

template <class Context>
RecoveryState<Context>::RecoveryState(unsigned long maxDuration, State<Context> *recoveryState, State<Context> *state):
maxDuration(maxDuration),
recoveryState(recoveryState),
state(state) {
}

template <class Context>
void RecoveryState<Context>::handleState(StateMachine<Context> *stateMachine) {
  if (startTime == 0)
    startTime = millis();
  
  if (millis() > startTime + maxDuration) {
    stateMachine->setState(recoveryState);
  } else {
    stateMachine->handleState();
  }
}

#endif /* StateMachine_h */
