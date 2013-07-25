/* ============================================================
 *
 * This file is a part of WHC IDE Project
 * http://http://whcomputing.wikispaces.com/
 *
 * Date        : 23 JUL 2013
 * Description : Implementation for the monitor class, responsible for logging.
 *
 * Copyright (C) 2013 by Preda Andrei <andreipreda07 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "monitor.h"

#include <QStringList>
#include <QString>
#include <QDir>
#include <QDebug>

/**
 * The total number of runs
 */
#define PROJ_RUNS "projRuns"

/**
 * Information stored about one exectuion of the project
 */
#define PROJ_RUN_INFO "projRun%1_"

/**
 * The total number of processes that ran;
 */
#define PROCS_RAN "procsRan"

/**
 * The device on which the process ran
 */
#define PROC_DEVID "procDevId_Id%1"

/**
 * The time it took to finish the process
 */
#define PROC_TIME "procTime_Id%1"

/**
 * The input file/s for the process
 **/
#define PROC_INPUT "procInput_Id%1"

/**
 * The id of the process in the workflow diagram
 */
#define PROC_DIAG_ID "procDiagId_Id%1"

/**
 * The status of the process (Succress, IOError, Crash exit or Process error)
 */
#define PROC_EXIT_STATUS "procExitStatus_Id%1"

/**
 * Additional info regarding the process exit status
 */
#define PROC_EXIT_INFO "procExitInfo_Id%1"

/**
 * Total execution time
 */
#define EXEC_TIME "execTime"

Monitor::Monitor()
{
    aggregateStats = new QSettings("WHC", "WHC IDE Monitor");
}

Monitor::~Monitor()
{
    delete aggregateStats;
}

void Monitor::slotStartProcess(int devId)
{
    procTimer[devId] = new QTime();
    procTimer[devId]->start();
}

void Monitor::slotStartExecute(QString whcFile)
{
    QString logPath = whcFile.remove(whcFile.split("/").last());
    QDir projectDir(logPath);
    projectDir.mkdir("log");

    projectStats = new QSettings(logPath + "log/stats" , QSettings::IniFormat);
    runStats = new QSettings(logPath + "log/run", QSettings::IniFormat);

    runStats->clear();
    procsRan = 0;

    if(!projectStats->contains(PROJ_RUNS))
        runId = 1;
    else
        runId = projectStats->value(PROJ_RUNS).toInt() + 1;

    execTimer.start();
}

void Monitor::slotFinishedProcess(int devId, int taskId, QString *inFiles,
                                  int taskStatus, int moreInfo)
{
    runStats->setValue(QString(PROC_DEVID).arg(procsRan), devId);
    runStats->setValue(QString(PROC_DIAG_ID).arg(procsRan), taskId);
    runStats->setValue(QString(PROC_TIME).arg(procsRan),
                       procTimer[devId]->elapsed());
    runStats->setValue(QString(PROC_INPUT).arg(procsRan), *inFiles);
    runStats->setValue(QString(PROC_EXIT_STATUS).arg(procsRan), taskStatus);
    runStats->setValue(QString(PROC_EXIT_INFO).arg(procsRan), moreInfo);
    procsRan++;

    delete inFiles;
    delete procTimer[devId];
}

void Monitor::slotFinishedExecute()
{
    runStats->setValue(EXEC_TIME, execTimer.elapsed());
    runStats->setValue(PROCS_RAN, procsRan);

    projectStats->setValue(PROJ_RUNS, runId);

    projectStats->sync();
    runStats->sync();

    delete projectStats;
    delete runStats;
}
