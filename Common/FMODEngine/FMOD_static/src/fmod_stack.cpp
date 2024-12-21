#include "fmod_settings.h"

#include "fmod_stack.h"

namespace FMOD
{


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    PS3

	[SEE_ALSO]
]
*/
Stack::Stack()
{
    mTop      = 0;
    mNext     = 0;
    mPrevious = 0;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    PS3

	[SEE_ALSO]
]
*/
void Stack::push(Stack *object)
{
    if (!mTop)
    {
        /*
            We have just pushed on the first object
        */

        mTop = object;

        return;
    }

    mTop->mNext       = object;
    object->mPrevious = mTop;
    mTop              = object;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    PS3

	[SEE_ALSO]
]
*/
Stack *Stack::pop()
{
    Stack *popped;

    if (!mTop)
    {
        return 0;
    }

    popped = mTop;

    mTop = mTop->mPrevious;

    return popped;
}


/*
[
	[DESCRIPTION]

	[PARAMETERS]
 
	[RETURN_VALUE]

	[REMARKS]

    [PLATFORMS]
    PS3

	[SEE_ALSO]
]
*/
bool Stack::stackEmpty()
{
    if (mTop)
    {
        return false;
    }

    return true;
}

}
