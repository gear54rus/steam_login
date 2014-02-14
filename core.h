#ifndef CORE_H
#define CORE_H

#include <QCoreApplication>
#include <QTimer>
#include "worker.h"

class core : public QCoreApplication
{
    Q_OBJECT
public:
    explicit core(int argc, char** argv);
private:
   network::worker* w;
signals:

public slots:
   void enter_sg();
};

#endif // CORE_H
