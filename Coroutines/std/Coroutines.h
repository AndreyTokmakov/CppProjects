/**============================================================================
Name        : Coroutines.h
Created on  : 27.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Coroutines.h
============================================================================**/

#ifndef CPPPROJECTS_COROUTINES_H
#define CPPPROJECTS_COROUTINES_H

namespace Coroutines {
    void TestAll();
};

/** =====================   Simple    ======================== **/

namespace Coroutines::Simple::Coroutine_Lifecycle_CoAwait {
    void TestAll();
}

namespace Coroutines::Simple::Coroutine_Lifecycle_CoReturn {
    void TestAll();
}

namespace Coroutines::Simple::Returning_Coroutine {
    void TestAll();
}

namespace Coroutines::Simple::Returning_Coroutine_2 {
    void TestAll();
}

namespace Coroutines::Simple::Resuming_Coroutine_1 {
    void TestAll();
}

namespace Coroutines::Simple::Waitable_Coroutine {
    void TestAll();
}

namespace Coroutines::Simple::Waitable_Coroutine_2 {
    void TestAll();
}

namespace Coroutines::Simple::Waitable_Coroutine_Update_Promise_State {
    void TestAll();
}

namespace Coroutines::Simple::Multiple_Awaiters_Resolution {
    void TestAll();
}

namespace Coroutines::Simple::Multiple_Awaiters_Resolution_2 {
    void TestAll();
}

namespace Coroutines::Simple::Yield_Coroutine {
    void TestAll();
}

namespace Coroutines::Simple::Yield_Coroutine_Values_from_List {
    void TestAll();
}


/** ===================== Experiments ======================== **/

namespace Coroutines::Experiments {
    void TestAll();
};

namespace Coroutines::Experiments::PinBall_Game {
    void TestAll();
};

namespace Coroutines::Experiments::Calculating_Average {
    void TestAll();
}

namespace Coroutines::Experiments::Waitable_Coroutine_With_Mutex {
    void TestAll();
}

namespace Coroutines::Experiments::Generic_TaskBased_Coroutine {
    void TestAll();
}

/** ===================== Generators  ======================== **/

namespace Coroutines::Generators {
    void TestAll();
}

/** =====================  UseCases   ======================== **/

namespace Coroutines::Experiments::FileReader {
    void TestAll();
}

namespace Coroutines::Experiments::TaskCoordination {
    void TestAll();
}

/** =====================             ======================== **/


namespace Coroutines::Waiting_Coroutine {
    void TestAll();
};


#endif //CPPPROJECTS_COROUTINES_H
