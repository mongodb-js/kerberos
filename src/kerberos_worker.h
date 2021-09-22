#ifndef KERBEROS_ASYNC_WORKER_H
#define KERBEROS_ASYNC_WORKER_H

#include <functional>
#include "kerberos.h"

namespace node_kerberos {

class KerberosWorker final : public Napi::AsyncWorker {
 public:
    ~KerberosWorker() {}

    typedef std::function<void(KerberosWorker*)>  OnFinishedHandler;
    typedef std::function<void(OnFinishedHandler)> SetOnFinishedHandler;
    typedef std::function<void(SetOnFinishedHandler)> ExecuteHandler;

    template <class... T>
    void Call(T... t) {
      Callback().Call(t...);
    }

    static void Run(Napi::Function callback, const char* resource_name, ExecuteHandler handler) {
        auto worker = new KerberosWorker(callback, resource_name, handler);
        worker->Queue();
    }

 protected:
    void Execute() final {
        execute_handler([=] (OnFinishedHandler handler) {
            on_finished_handler = handler;
        });
    }

    void OnOK() final {
        on_finished_handler(this);
    }

 private:
    explicit KerberosWorker(Napi::Function callback, const char* resource_name, ExecuteHandler handler)
        : Napi::AsyncWorker(callback, resource_name), execute_handler(handler) {}

    ExecuteHandler execute_handler;
    OnFinishedHandler on_finished_handler;
};

}

#endif // ASYNC_WORKER_H
