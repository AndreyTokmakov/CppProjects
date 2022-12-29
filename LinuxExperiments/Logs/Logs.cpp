//============================================================================
// Name        : Logs.h
// Created on  : 04.06.2022.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Logs
//============================================================================

#include "Logs.h"

#include "../common.h"

namespace Logs
{
    void SysLogMessage()
    {
        // INFO: 'tail /var/log/syslog' will be added:
        // INFO: Jun  4 11:12:15 AndTokmUbuntu LinuxExperiments: syslog test 1: Success
        syslog(LOG_ERR | LOG_USER, "syslog test 1: %m\n");

    }
};

void Logs::TestAll() {
    SysLogMessage();
};
