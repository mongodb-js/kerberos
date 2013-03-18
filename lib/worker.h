#ifndef WORKER_H_
#define WORKER_H_

#include <v8.h>
#include <node.h>

using namespace node;

class Worker {
  public:
    Worker();
    virtual ~Worker();

    // libuv's request struct.
    uv_work_t request;
    // Callback
    v8::Persistent<v8::Function> callback;
    // Arguments
    v8::Persistent<v8::Array> arguments;
    // Results
    v8::Persistent<v8::Value> return_value;
    // Did we raise an error
    bool error;
    // The error message
    char *error_message;
    // Error code if not message
    int error_code;
    // Method we are going to fire
    void virtual execute();
};

#endif  // WORKER_H_
