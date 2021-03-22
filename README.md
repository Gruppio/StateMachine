# StateMachine
Create State Machines with class

## Advantages
Create a State Machine has never been easier!
There are just 2 components `StateMachine` and `State`.
There are some state's decorators that will help develop your applications logic.

## StateMachine
The StateMachine is the orchestrator of the states.
It has a generic `Context` parameter that allows to provide some informations or dependencies to the states.

## State
Subclass the `State` class and override the `handleState` method.

## State Decorators

### Delay
The `State`'s `delay` method will return a new `DelayedState` that will switch to the new state after a period of time

### Throttle
The `State`'s `throttle` method will return a new `ThrottledState` call the `handleState` max once every period of time

### Recovery
The `State`'s `recoveryAfter` method will switch to a recoveryState if the state is still running after a period of time

## Example
```c
// This example creates a StateMachine with 3 states

// Include the StateMachine library found here :
// https://github.com/Gruppio/StateMachine

#include <Arduino.h>
#include <StateMachine.h>

struct StateContext {
  bool isCompleted = false;
};

class FinalState: public State<StateContext*> {
  void handleState(StateMachine<StateContext*> *stateMachine) {
    Serial.println("Perform FinalState work");
    stateMachine->context->isCompleted = true;
  }
};

class SecondState: public State<StateContext*> {
  void handleState(StateMachine<StateContext*> *stateMachine) {
    Serial.println("Perform SecondState work");
    // Will call the `handleState` of `FinalState` max once every `100ms`
    stateMachine->setState(
        (new FinalState())->throttle(100);
    );
  }
};

class InitialState: public State<StateContext*> {
  void handleState(StateMachine<StateContext*> *stateMachine) {
    Serial.println("Perform BeginState work");
    // Will transition to `SecondState` after 1000ms
    stateMachine->setState(
        (new SecondState())->delay(1000);
    );
  }
};

StateContext *stateContext;
StateMachine<StateContext*> *stateMachine;

void setup() {
  stateContext = new StateContext();
  stateMachine = new StateMachine<StateContext*>(stateContext, new InitialState());
}

void loop() {
   stateMachine->handleState();
}
```

## If you like this project please:

<a href="https://www.buymeacoffee.com/gruppio" target="_blank"><img src="https://raw.githubusercontent.com/Gruppio/Sonoff-Homekit/images/images/buymeacoffee.png" alt="Buy Me A Coffee" width="300" ></a>

## Thank you!
