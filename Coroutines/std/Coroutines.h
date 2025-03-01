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

namespace Coroutines::CoroutineLifecycle {
    void TestAll();
}

namespace Coroutines::Simple_Coroutine_ReturningValue {
    void TestAll();
}

namespace Coroutines::Simple_Coroutine_Waitable {
    void TestAll();
}

namespace Coroutines::Simple_Coroutine_Waitable_2 {
    void TestAll();
}

namespace Coroutines::Waitable_Coroutine_With_Mutex {
    void TestAll();
}

namespace Coroutines::UseCases_FileReader_Coroutine {
    void TestAll();
}

namespace Coroutines::UseCases_TaskCoordination {
    void TestAll();
}

namespace Coroutines::Yield_Coroutine {
    void TestAll();
}

namespace Coroutines::Return_Value_Coroutine {
    void TestAll();
};

namespace Coroutines::Simple_Coroutine {
    void TestAll();
};

namespace Coroutines::Waiting_Coroutine {
    void TestAll();
};

namespace Coroutines::Generators {
    void TestAll();
};

namespace Coroutines::Experiments {
    void TestAll();
};

namespace Coroutines::Examples::PinBall_Game {
    void TestAll();
};

namespace Coroutines::Examples::Calculating_Average {
    void TestAll();
}

#endif //CPPPROJECTS_COROUTINES_H
