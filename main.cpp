//
//  main.cpp
//  StateMachine
//
//  Created by Michele Gruppioni on 21/03/21.
//

#include <iostream>
#include "src/StateMachine.h"

unsigned long millis() {
  return 0;
}

struct StateContext {
  bool isCompleted = false;
};

class FinalState: public State<StateContext*> {
  void handleState(StateMachine<StateContext*> *stateMachine) {
    std::cout << "Perform FinalState work\n";
    stateMachine->context->isCompleted = true;
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
    stateMachine->setState(new SecondState());
  }
};

int main(int argc, const char * argv[]) {
  StateContext *stateContext = new StateContext();
  StateMachine<StateContext*> *stateMachine = new StateMachine<StateContext*>(stateContext, new InitialState());
  
  while (!stateContext->isCompleted) {
    stateMachine->handleState();
  }

  return 0;
}
