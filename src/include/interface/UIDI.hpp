#include <coroutine>

template<typename Pro>
struct _task;

template<typename P, typename return_>
struct Promise {
    _task<P> get_return_object(){
        return {std::coroutine_handle<P>::from_promise(static_cast<P&>(*this))};
    };
    std::suspend_never initial_suspend(){
        return static_cast<P *>(this)->initial_suspend();
    }
    std::suspend_always final_suspend()noexcept{
        return static_cast<P *>(this)->final_suspend();
    }
    void unhandled_exception(){}
};

template<typename Pro>
class _task {
    std::coroutine_handle<Pro> handle;
public:
    using promise_type = Pro;

    task(std::coroutine_handle<Pro> h):handle(h){};
    void resume(){handle.resume();}
};

template<typename _Aw>
struct _awaitable{
    std::coroutine_handle<> handle;
    bool await_ready()noexcept{
        return static_cast<_Aw *>(this)->await_ready();
    }
    void await_suspend(std::coroutine_handle<> h){
        handle = h;
        static_cast<_Aw *>(this)->await_suspend(h);
    }
    void await_resume()noexcept{
        static_cast<_Aw *>(this)->await_resume();
    }
};
