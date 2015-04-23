/*
 * sample application to show the usage of work queues along with async http
 *server
 *
 * (C) Copyright Dino Korah 2013.
 * Modifications (C) Vince Coccia 2015
 * Distributed under the Boost Software License, Version 1.0. (See copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

//#include <boost/network/include/http/server.hpp>
//#include <boost/network/uri.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp> // cpp-netlib and asio want boost::shared_ptrs
#include <boost/thread.hpp>
#include <iostream>
#include <signal.h>

#include "rest_processor.hh"
#include "liveness_resource.hh"

#define Log(line)                   \
  do {                              \
    std::cout << line << std::endl; \
  } while (false)

typedef boost::network::http::async_server<rest::WorkQueue> server;

/**
 * Clean shutdown signal handler
 *
 * @param error
 * @param signal
 * @param p_server_instance
 */
void shut_me_down(const boost::system::error_code& error, int signal,
                  boost::shared_ptr<server> p_server_instance) 
{
  if (!error) p_server_instance->stop();
}

/**
 * Process request; worker (thread)
 *
 * @param queue
 */
void process_request(rest::WorkQueue& queue) 
{
    while (!boost::this_thread::interruption_requested()) 
    {
        rest::void_function work = [](){return;};
        if (queue.get_work(work))
        {
            work();
        }
        boost::this_thread::sleep(boost::posix_time::microseconds(1000));
    }
}

int main(void) try 
{
    // the thread group
    boost::shared_ptr<boost::thread_group> 
        p_threads(boost::make_shared<boost::thread_group>());

    // setup asio::io_service
    boost::shared_ptr<boost::asio::io_service> 
        p_io_service(boost::make_shared<boost::asio::io_service>());
    boost::shared_ptr<boost::asio::io_service::work> 
        p_work(boost::make_shared<boost::asio::io_service::work>(boost::ref(*p_io_service)) );

    // io_service threads
    /*{
        int n_threads = 5;
        while (0 < n_threads--) 
        {
            p_threads->create_thread(
                boost::bind(&boost::asio::io_service::run, p_io_service));
        }
    }*/
  
    rest::WorkQueue queue;
    rest::AsyncLivenessResource liveness;
    queue.add_resource(liveness.destination, liveness);

    // worker threads that will process the request; off the queue
    {
        int n_threads = 5;
        while (0 < n_threads--) 
        {
            p_threads->create_thread(std::bind(process_request, std::ref(queue)));
        }
    }

    // setup the async server
    boost::shared_ptr<rest::async_server> 
        p_server_instance(boost::make_shared<rest::async_server>(
            server::options(queue)
                .address("127.0.0.1")
                .port("8080")
                .io_service(p_io_service)
                .reuse_address(true)
                .thread_pool(boost::make_shared<boost::network::utils::thread_pool>
                                (2, p_io_service, p_threads)))
        );

    // setup clean shutdown
    boost::asio::signal_set signals(*p_io_service, SIGINT, SIGTERM);
    signals.async_wait(boost::bind(shut_me_down, _1, _2, p_server_instance));

    // run the async server
    p_server_instance->run();

    // we are stopped - shutting down

    p_threads->interrupt_all();

    p_work.reset();
    p_io_service->stop();

    p_threads->join_all();

    Log("Terminated normally");
    exit(EXIT_SUCCESS);
}
catch (const std::exception& e) 
{
    Log("Abnormal termination - exception:" << e.what());
    exit(EXIT_FAILURE);
}
