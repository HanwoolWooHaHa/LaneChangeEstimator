#pragma once

#include <qthread.h>

class CThread : public QThread
{
    Q_OBJECT

public:
    CThread( QObject* parent = 0 )
    {}
    ~CThread()
    {}
};
