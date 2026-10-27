#include <stdexec/execution.hpp>

#include <coroutine>
#include <exception>

struct inline_coro
{
  struct promise_type
  {
    inline_coro get_return_object() noexcept
    {
      return {};
    }

    std::suspend_never initial_suspend() noexcept
    {
      return {};
    }

    std::suspend_never final_suspend() noexcept
    {
      return {};
    }

    void return_void() noexcept {}

    void unhandled_exception() noexcept
    {
      std::terminate();
    }

    std::coroutine_handle<> unhandled_stopped() noexcept
    {
      return std::noop_coroutine();
    }

    template <class awaitable>
    auto await_transform(awaitable&& value)
    {
      return stdexec::as_awaitable(static_cast<awaitable&&>(value), *this);
    }
  };
};

struct receiver
{
  using receiver_concept = stdexec::receiver_tag;

  void set_value(int value) noexcept
  {
    result = value;
  }

  void set_error(std::exception_ptr) noexcept
  {
    result = -1;
  }

  void set_stopped() noexcept
  {
    result = -2;
  }

  int& result;
};

inline_coro await_inline_sender(int& result)
{
  result = co_await stdexec::just(42);
}

int main()
{
  int coroutine_result = 0;
  await_inline_sender(coroutine_result);

  int result = 0;
  auto operation = stdexec::connect(stdexec::just(42), receiver{result});
  stdexec::start(operation);
  return coroutine_result == 42 && result == 42 ? 0 : 1;
}
