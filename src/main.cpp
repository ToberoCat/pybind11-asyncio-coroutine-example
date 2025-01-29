#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/iostream.h>
#include <pybind11/chrono.h>
#include <iostream>
#include <string>
#include <memory>
#include <coroutine>

namespace py = pybind11;

/**
 * A simple generator-like coroutine in C++20 that:
 *   1. Prints "prefix init" at start
 *   2. Yields values from 0 to 10
 *   3. Prints "prefix dispose" at the end
 */
class MyAsyncGenerator
{
public:
    //******************************
    //  Promise Type for Coroutines
    //******************************
    struct promise_type
    {
        // -- State you want to manage per coroutine --
        std::string prefix;
        int current = 0;
        bool initialized = false;

        // Return object for the coroutine
        MyAsyncGenerator get_return_object()
        {
            return MyAsyncGenerator(std::coroutine_handle<promise_type>::from_promise(*this));
        }

        // Called when the coroutine first starts
        std::suspend_always initial_suspend() noexcept
        {
            return {};
        }

        // Called right before the coroutine ends
        std::suspend_always final_suspend() noexcept
        {
            // We can do final clean-up here if needed
            std::cout << prefix << " dispose" << std::endl;
            return {};
        }

        // If an unhandled exception occurs in the coroutine
        void unhandled_exception()
        {
            std::exit(1); // or store the exception in std::exception_ptr
        }

        // We have no final return value for a generator-style coroutine
        void return_void()
        {
        }

        // Called each time we do `co_yield someValue;`
        std::suspend_always yield_value(int /* value */)
        {
            return {};
        }

        // Optional: if you want a custom constructor or to pass in prefix
        promise_type() = default;
    };

    using handle_type = std::coroutine_handle<promise_type>;

    // Constructors / destructor
    MyAsyncGenerator(handle_type h) : coro(h)
    {
    }

    MyAsyncGenerator(const MyAsyncGenerator&) = delete;

    MyAsyncGenerator(MyAsyncGenerator&& other) noexcept : coro(other.coro)
    {
        other.coro = nullptr;
    }

    ~MyAsyncGenerator()
    {
        if (coro)
        {
            coro.destroy(); // Clean up the coroutine if not already done
        }
    }

    /**
     * move_next():
     *   - Resume the coroutine.
     *   - If this is the first resume, prints "<prefix> init".
     *   - On each resume, the coroutine runs until the next co_yield or completion.
     *   - Returns false if the coroutine is done, true otherwise.
     */
    bool move_next()
    {
        if (!coro || coro.done())
        {
            return false; // Already finished
        }

        auto& p = coro.promise();

        // If not yet initialized, do the "init" part
        if (!p.initialized)
        {
            p.initialized = true;
            std::cout << p.prefix << " init" << std::endl;
        }

        // Resume the coroutine
        coro.resume();

        // If the coroutine is now done, we won't continue
        if (coro.done())
        {
            return false;
        }

        // At each step, we print the current iteration
        std::cout << p.prefix << " step " << p.current << std::endl;
        p.current++;
        return true;
    }

    // Helper to set the prefix from the outside (before running)
    void set_prefix(const std::string& pref)
    {
        coro.promise().prefix = pref;
    }

private:
    handle_type coro;
};

/**
 * The actual coroutine function that "co_yield"s from 0 to 10.
 * Notice it returns MyAsyncGenerator, whose promise_type is defined above.
 */
MyAsyncGenerator createGenerator()
{
    // From 0..10
    for (int i = 0; i <= 10; i++)
    {
        co_yield i;
    }
}


/**
 * This function:
 *   1. Creates a new C++20 coroutine generator (MyAsyncGenerator).
 *   2. Sets the prefix to control logging.
 *   3. Returns a Python asyncio.Task that runs our `run_iterator(...)` async loop.
 *
 * Usage in Python:
 *   import asyncio_example
 *   import asyncio
 *
 *   async def main():
 *       task = asyncio_example.myFunction("TEST")
 *       await task
 *
 *   asyncio.run(main())
 */
py::object myFunction(const py::module_& module, const std::string& prefix)
{
    // Create a new generator
    MyAsyncGenerator gen = createGenerator();
    gen.set_prefix(prefix);

    // Wrap in shared_ptr so Python can manage lifetime
    auto sharedGen = std::make_shared<MyAsyncGenerator>(std::move(gen));

    // Import asyncio
    const py::object asyncio = py::module_::import("asyncio");

    // Call:  asyncio.create_task(run_iterator(sharedGen, prefix))
    const auto run_iterator = module.attr("_run_iterator");
    return asyncio.attr("create_task")(run_iterator(sharedGen, prefix));
}


PYBIND11_MODULE(asyncio_example, m)
{
    m.doc() = "Example showcasing a C++20 coroutine exposed as a Python async function.";

    // 1. Define the MyAsyncGenerator type in Python
    py::class_<MyAsyncGenerator, std::shared_ptr<MyAsyncGenerator>>(m, "MyAsyncGenerator")
        .def("move_next", &MyAsyncGenerator::move_next)
        .def("set_prefix", &MyAsyncGenerator::set_prefix);

    // 2. Embed a one-time definition of the Python async function: run_iterator(...)
    py::exec(R"(
import asyncio

async def run_iterator(gen, prefix):
    """
    Polls the C++ generator in a loop, yielding control to the event loop each time.
    """
    while True:
        # let other tasks run
        await asyncio.sleep(0)
        # step the C++ generator
        cont = gen.move_next()
        if not cont:
            break
)");

    // 3. Retrieve the run_iterator function so we can reuse it
    m.attr("_run_iterator") = py::globals()["run_iterator"];;

    // 4. Expose myFunction(...) to create & schedule the async iteration
    m.def(
        "myFunction",
        [m](const std::string& prefix) { return myFunction(m, prefix); },
        py::arg("prefix"),
        R"pbdoc(
            Create a new C++20 coroutine generator, wrap it in a Python async task, and return the task.
            You can 'await' the returned object or schedule it via asyncio.
        )pbdoc"
    );
}
