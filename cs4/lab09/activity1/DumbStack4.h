// DumbStack4.h
// Dysfunctional Stack
// Author: Andreas Borchert
// Contributors: {James Heliotis}

// Description: Faulty stack implementation # 4.

// To use this, include it in your test program, and link your code
// with DumbStack4.o.

#ifndef RITCS_DUMBSTACK4
#define RITCS_DUMBSTACK4

#include "Stack.h"

namespace RITCS {

class DumbStack4: public Stack {
   public:
      // constructor
      DumbStack4( int size );

      // destructor
      ~DumbStack4();

      // accessors

      // top
      //
      // Returns:     the item most recently pushed on the stack
      // Pre:         stack is not empty
      // Post:        stack is unchanged
      //
      virtual char top() const;

      // full
      //
      // Returns:     true iff the stack cannot accept furter elements
      // Post:        stack is unchanged
      //
      virtual bool full() const;

      // size
      //
      // Returns:     the number of elements in the stack
      // Post:        stack is unchanged
      //
      virtual unsigned int size() const;

      // mutators

      // push
      //
      // Description: adds a new element to "the top of" the stack
      // Arguments:   the element to be added
      // Pre:         stack is not full
      // Post:        size has increased by one
      // Post:        top is equal to the argument newElement
      //
      virtual void push( char newElement );

      // pop
      //
      // Description: removes an element from "the top of" the stack,
      //              i.e., the one most recently added but not yet popped
      // Pre:         stack is not empty
      // Post:        size has decreased by one
      //
      virtual void pop();

   private:
      struct Hidden* hidden; // sorry, no spoilers here :-)
}; // class DumbStack4

} // RITCS namespace

#endif
