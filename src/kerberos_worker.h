#ifndef ASYNC_WORKER_H
#define ASYNC_WORKER_H

#include <functional>
#include <nan.h>

class KerberosWorker : public Nan::AsyncWorker {
 public:
    typedef std::function<void(KerberosWorker*)>  OnFinishedHandler;
    typedef std::function<void(OnFinishedHandler)> SetOnFinishedHandler;
    typedef std::function<void(SetOnFinishedHandler)> ExecuteHandler;

    explicit KerberosWorker(Nan::Callback *callback, const char* resource_name, ExecuteHandler handler)
        : Nan::AsyncWorker(callback, resource_name), execute_handler(handler) {}

    template <class... T>
    void Call(T... t) {
      callback->Call(t..., async_resource);
    }

    virtual void Execute() {
        execute_handler([=] (OnFinishedHandler handler) {
            on_finished_handler = handler;
        });
    }

    static void Run(Nan::Callback *callback, const char* resource_name, ExecuteHandler handler) {
        Nan::TryCatch tryCatch;
        if (tryCatch.HasCaught()) {
            tryCatch.ReThrow();
            return; // don't proceed in case there were any previous errors
        }

        auto worker = new KerberosWorker(callback, resource_name, handler);
        Nan::AsyncQueueWorker(worker);
    }

 protected:
    void HandleOKCallback() {
        on_finished_handler(this);
    }

 private:
    ExecuteHandler execute_handler;
    OnFinishedHandler on_finished_handler;
};

#endif // ASYNC_WORKER_H
