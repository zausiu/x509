////////////////////////////////////////////
// http://blog.ykyi.net/ presents ...
// created on Apr. 22 2018
//////////////////////////////////////////////
#include <ZooVisitor.h>
#include "x509/X509Crt.h"
#include "x509/X509Verifier.h"
#include "Strategy.h"

class CuttleImpl
{
public:
    CuttleImpl();
    ~CuttleImpl();

    void init(unsigned short port, const char* ca_crt_path);
    void run();

private:
    void run_once();

private:
    int server_fd_;
    ZooVisitor zoo_visitor_;

    X509Verifier* crt_verifier_;

    Strategy strategy_;
};
