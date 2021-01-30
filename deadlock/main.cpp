#include <exception>
#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <chrono>
#include <signal.h>

class application
{
public:
    typedef std::list<std::thread> thread_pool_t;

    static void signal_handler(int sig)
    {
        std::cout << "signal: " << sig << "\n";
    }
public:
    application()
        : guard()
        , thread_pool()
        , thread_pool_size(5)
    {
        signal(SIGTERM, &application::signal_handler);
    }

    ~application()
    {
        for (thread_pool_t::iterator ithread = this->thread_pool.begin(); ithread != this->thread_pool.end(); ++ithread)
            ithread->join();
    }

    void run()
    {
        for (int i = 0; i < this->thread_pool_size; ++i)
            this->thread_pool.emplace_back(&application::thread_run, this);
    }

private:
   void thread_run()
   {
       using namespace std::chrono_literals;

       std::cout << "thread_run, thread: " << std::this_thread::get_id() << "\n";
       std::lock_guard<std::mutex> guard(this->guard);
       while (true) {
        std::cout << "thread_run, loop thread: " << std::this_thread::get_id() << "\n";
        std::this_thread::sleep_for(2s);
        std::lock_guard<std::mutex> guard2(this->guard);
       }
   }

private:
    std::mutex guard;
    thread_pool_t thread_pool;
    int thread_pool_size;
};

int main()
{
    try 
    {
        auto app = application();
        app.run();
    }
    catch (std::exception& err)
    {
        std::cout << "exception: " << err.what() << "\n";
    }
    catch (...)
    {
        std::cout << "unknown error\n";
    }
    return 0;
}
