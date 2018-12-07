#include <stdio.h>

#include <uv.h>

#ifdef _WIN32
#include <Windows.h>
#elif
#include <unistd.h>
#endif // _WIN32


//void signal_handler(uv_signal_t *handle, int signum)
//{
//	printf("Signal received: %d\n", signum);
//	uv_signal_stop(handle);
//}
//
//// two signal handlers in one loop
//void thread1_worker(void *userp)
//{
//	uv_loop_t *loop1 = uv_loop_new();
//
//	uv_signal_t sig1a, sig1b;
//	uv_signal_init(loop1, &sig1a);
//	uv_signal_start(&sig1a, signal_handler, SIGUSR1);
//
//	uv_signal_init(loop1, &sig1b);
//	uv_signal_start(&sig1b, signal_handler, SIGUSR1);
//
//	uv_run(loop1, UV_RUN_DEFAULT);
//	uv_barrier_wait();
//}
//
//// two signal handlers, each in its own loop
//void thread2_worker(void *userp)
//{
//	uv_loop_t *loop2 = uv_loop_new();
//	uv_loop_t *loop3 = uv_loop_new();
//
//	uv_signal_t sig2;
//	uv_signal_init(loop2, &sig2);
//	uv_signal_start(&sig2, signal_handler, SIGUSR1);
//
//	uv_signal_t sig3;
//	uv_signal_init(loop3, &sig3);
//	uv_signal_start(&sig3, signal_handler, SIGUSR1);
//
//	while (uv_run(loop2, UV_RUN_NOWAIT) || uv_run(loop3, UV_RUN_NOWAIT)) {
//	}
//}

//int main()
//{
//	printf("PID %d\n", getpid());
//
//	uv_thread_t thread1, thread2;
//
//	uv_thread_create(&thread1, thread1_worker, 0);
//	uv_thread_create(&thread2, thread2_worker, 0);
//
//	uv_thread_join(&thread1);
//	uv_thread_join(&thread2);
//	return 0;
//}

#include <iostream>
#include <future>
#include <thread>

int main()
{
	// future from a packaged_task
	std::packaged_task<int()> task([]() {
		std::cout << "packaged_task started" << std::endl;
		std::cout << "packaged_task thread pid=" << _getpid() << std::endl;
		return 7; }); // wrap the function
	std::future<int> f1 = task.get_future();  // get a future
	std::thread(std::move(task)).detach(); // launch on a thread

										   // future from an async()
	std::future<int> f2 = std::async(std::launch::deferred, []() {
		std::cout << "Async task started" << std::endl;
		std::cout << "Async task thread pid=" << _getpid() << std::endl;
		return 8; });

	// future from a promise
	std::promise<int> p;
	std::future<int> f3 = p.get_future();
	std::thread([&p] { p.set_value_at_thread_exit(9); std::cout << "promise thread pid=" << _getpid() << std::endl; }).detach();

	f1.wait();
	f2.wait();
	f3.wait();
	std::cout << "main thread pid=" << _getpid() << std::endl;
	std::cout << "Done!\nResults are: "
		<< f1.get() << ' ' << f2.get() << ' ' << f3.get() << '\n';

	return 0;
}