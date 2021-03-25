//
//  main.cpp
//  StateMachine
//
//  Created by Michele Gruppioni on 21/03/21.
//

#include <chrono>
#include <iostream>
#include <sys/time.h>
#include <ctime>
#include "src/StateMachine.h"

unsigned long millis() {
  return (unsigned long)time(nullptr) * 1000 ;
}

struct StateContext {
  bool isCompleted = false;
};

class ErrorState: public State<StateContext*> {
  void handleState(StateMachine<StateContext*> *stateMachine) {
    if (!stateMachine->context->isCompleted) {
      std::cout << "Error\n";
      stateMachine->context->isCompleted = true;
    }
  }
};

class FinalState: public State<StateContext*> {
  void handleState(StateMachine<StateContext*> *stateMachine) {
    if (!stateMachine->context->isCompleted) {
      std::cout << "Perform FinalState work\n";
      stateMachine->context->isCompleted = true;
    }
  }
};

class SecondState: public State<StateContext*> {
  void handleState(StateMachine<StateContext*> *stateMachine) {
    std::cout << "Perform SecondState work\n";
    stateMachine->setState(new FinalState());
  }
};

class InitialState: public State<StateContext*> {
  void handleState(StateMachine<StateContext*> *stateMachine) {
    std::cout << "Perform BeginState work\n";
    stateMachine->setState(
                           (new SecondState())
                           ->recoveryAfter(3000, new ErrorState(), new ErrorState())
                           ->throttle(100)
                           ->delay(1000)
                           );
  }
};

int main(int argc, const char * argv[]) {
  StateContext *stateContext = new StateContext();
  StateMachine<StateContext*> *stateMachine = new StateMachine<StateContext*>(stateContext, new InitialState());
  
//  State<StateContext*> *s = (new InitialState())->delay(1000)->throttle(1000)->recoveryAfter(100, new ErrorState(), new ErrorState());
//  delete s;
  
  while (!stateContext->isCompleted) {
    stateMachine->handleState();
  }
  
  return 0;
}
