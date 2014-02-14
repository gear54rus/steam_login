#include "core.h"
#include "logger.h"
#include "util.h"

core::core(int argc, char **argv) :
    QCoreApplication(argc,argv)
{
    util::init_resources();
    QString uname(app::query("Enter the username: "));
    QString pwd(app::query("Enter the password: "));
    w = new network::worker(uname,pwd,this);
    QTimer::singleShot(0,w,SLOT(start_login()));
}
void core::enter_sg() {

}
