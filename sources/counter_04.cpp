#include <coroutine>
#include <iostream>

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
		std::suspend_always yield_value(unsigned value) {
			value_ = value;
			return {};
		}
	};

	std::coroutine_handle<promise_type> h_;
};

ReturnObject counter()
{
	for (unsigned i = 0; ; ++i) {
		co_yield i;
	}
}

int main()
{
	auto h = counter().h_;
	auto &promise = h.promise();
	for (int i = 0; i != 3; ++i) {
		std::cout << "counter: " << promise.value_ << std::endl;
		h();
	}
	h.destroy();
}