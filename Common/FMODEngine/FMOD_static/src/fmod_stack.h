#ifndef _FMOD_STACK_H
#define _FMOD_STACK_H

namespace FMOD
{
    class Stack
    {
      private:

        Stack       *mTop;
        Stack       *mNext;
        Stack       *mPrevious;

      public:

        Stack();

        void         push(Stack *object);
        Stack       *pop();
        bool         stackEmpty();	

    };
}


#endif
