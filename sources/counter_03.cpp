#include <coroutine>
#include <iostream>

template <typename PromiseType>
struct GetPromise {
	PromiseType *p_;
	bool await_ready() { return false; }
	bool await_suspend(std::coroutine_handle<PromiseType> h) {
		p_ = &h.promise();
		return false;
	}
	PromiseType *await_resume() { return p_; }
};

struct ReturnObject {
	struct promise_type {
		unsigned value_;

		ReturnObject get_return_object() {
			return {
				.h_ = std::coroutine_handle<promise_type>::from_promise(*this)
			};
		}
		std::suspend_never initial_suspend() { return {}; }
		std::suspend_never final_suspend() noexcept { return {}; }
		void unhandled_exception() {}
	};

	std::coroutine_handle<promise_type> h_;
	operator std::coroutine_handle<promise_type>() const { return h_; }
};

ReturnObject counter()
{
	auto promise = co_await GetPromise<ReturnObject::promise_type>{};

	for (unsigned i = 0; ; ++i) {
		promise->value_ = i;
		co_await std::suspend_always{};
	}
}

int main()
{
	std::coroutine_handle<ReturnObject::promise_type> h = counter();
	ReturnObject::promise_type& promise = h.promise();
	for (int i = 0; i != 3; ++i) {
		std::cout << "counter: " << promise.value_ << std::endl;
		h();
	}
	h.destroy();
}