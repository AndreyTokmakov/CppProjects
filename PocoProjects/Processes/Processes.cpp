//============================================================================
// Name        : Processes.h
// Created on  : 25.04.2022
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Processes
//============================================================================

#include "Processes.h"

#include <iostream>
#include <string_view>
#include <vector>

#include "Poco/Process.h"
#include "Poco/PipeStream.h"
#include "Poco/StreamCopier.h"

namespace Processes {

    void RunSimpleProcess()
    {
        std::string cmd("/bin/ps");
        std::vector<std::string> params;
        params.emplace_back("-ax");


        Poco::Pipe outPipe;
        Poco::ProcessHandle ph = Poco::Process::launch(cmd, params, 0, &outPipe, 0);
        Poco::PipeInputStream istr(outPipe);
        // std::ofstream ostr("processes.txt");
        Poco::StreamCopier::copyStream(istr, std::cout);

    }
}

void Processes::TestAll() {
    RunSimpleProcess();
}