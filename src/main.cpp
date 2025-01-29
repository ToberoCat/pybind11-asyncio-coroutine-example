#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/iostream.h>
#include <pybind11/chrono.h>
#include <string>
#include <memory>
#include <coroutine>

namespace py = pybind11;

template <typename T>
class AsyncAlgorithm
{
public:
    struct promise_type
    {
        bool initialized = false;

        T current_value{};

        AsyncAlgorithm get_return_object()
        {
            return AsyncAlgorithm(std::coroutine_handle<promise_type>::from_promise(*this));
        }

        static std::suspend_always initial_suspend() noexcept
        {
            return {};
        }

        static std::suspend_always final_suspend() noexcept
        {
            return {};
        }

        static void return_void()
        {
        }

        static void unhandled_exception() { std::terminate(); }

        std::suspend_always yield_value(const T& value)
        {
            current_value = value;
            return {};
        }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    explicit AsyncAlgorithm(handle_type h) : coro(h)
    {
    }

    AsyncAlgorithm(const AsyncAlgorithm&) = delete;

    AsyncAlgorithm(AsyncAlgorithm&& other) noexcept
        : coro(other.coro)
    {
        other.coro = nullptr;
    }

    ~AsyncAlgorithm()
    {
        if (coro)
        {
            coro.destroy();
        }
    }

    bool advance()
    {
        if (!coro || coro.done())
            return false;

        auto& p = coro.promise();

        if (!p.initialized)
        {
            p.initialized = true;
        }

        coro.resume();

        if (coro.done())
            return false;

        return true;
    }

    T current() const
    {
        return coro.promise().current_value;
    }

private:
    handle_type coro;
};

template <typename T>
py::object start_async_algorithm(const py::module_& m,
                                 AsyncAlgorithm<T> algo)
{
    auto sharedAlgo = std::make_shared<AsyncAlgorithm<T>>(std::move(algo));
    const py::object asyncio = py::module_::import("asyncio");
    py::object runner = m.attr("_run_algorithm");
    return asyncio.attr("create_task")(runner(sharedAlgo));
}

//*********************************************************
// Example 1: A simple integer counter from 0..N
//*********************************************************
AsyncAlgorithm<int> create_counter(int N)
{
    for (int i = 0; i <= N; i++)
    {
        std::cout << "Yielding " << i << std::endl;
        co_yield i;
    }
}

//*********************************************************
// Example 2: A "fake" bubble sort demonstration
//            Yields the number of swaps done so far.
//*********************************************************
AsyncAlgorithm<int> bubble_sort_example(std::vector<int> data)
{
    int n = (int)data.size();
    int swaps = 0;

    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (data[j] > data[j + 1])
            {
                std::swap(data[j], data[j + 1]);
                swaps++;

                std::cout << "Swapped " << data[j] << " with " << data[j + 1] << std::endl;
                co_yield swaps;
            }
        }
    }
}


PYBIND11_MODULE(asyncio_example, m)
{
    m.doc() = "A generic example bridging C++20 coroutines with Python asyncio.";

    py::exec(R"(
import asyncio

async def run_algorithm(algo):
    '''Repeatedly calls algo.advance() until completion.'''
    while True:
        await asyncio.sleep(0)  # let other tasks run
        more = algo.advance()
        if not more:
            break
)");

    m.attr("_run_algorithm") =py::globals()["run_algorithm"];

    py::class_<AsyncAlgorithm<int>, std::shared_ptr<AsyncAlgorithm<int>>>(m, "AsyncAlgorithmInt")
        .def("advance", &AsyncAlgorithm<int>::advance)
        .def("current", &AsyncAlgorithm<int>::current);

    m.def(
        "start_counter", [m](const int N)
        {
            auto counter = create_counter(N);
            return start_async_algorithm(m, std::move(counter));
        },
        py::arg("N")
    );

    m.def(
        "start_bubble_sort",
        [m](const std::vector<int>& data)
        {
            auto sorter = bubble_sort_example(data);
            return start_async_algorithm(m, std::move(sorter));
        },
        py::arg("data")
    );
}
