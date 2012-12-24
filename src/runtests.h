#ifndef RUNTESTS_H
#define RUNTESTS_H

#include "projBuild/test/testCommandLine.h"
#include "tests/testIde.h"
#include <QtTest/QtTest>

void runtests()
{
    int argc = 0;
    char** argv = 0;

    TestCommandLine test;
    QTest::qExec(&test,argc,argv);

    TestIde test1;
    QTest::qExec(&test1,argc,argv);

}
#endif // RUNTESTS_H
