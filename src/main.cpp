#include <pybind11/pybind11.h>
#include <pybind11/iostream.h>
#include <pybind11/pytypes.h>
#include <pybind11/embed.h>
#include <pybind11/chrono.h>
#include <iostream>

namespace py = pybind11;

class AsyncIterator
{
public:
    virtual ~AsyncIterator() = default;

    virtual void init()
    {
    }

    virtual bool step() = 0;

    virtual void dispose()
    {
    }
};

py::object create_async_runner(std::shared_ptr<AsyncIterator> async_iterator)
{
    py::exec(R"(
import asyncio

async def async_runner(iterator):
    iterator.init()
    try:
        while True:
            await asyncio.sleep(0)  # Yield to the event loop
            done = iterator.step()
            if done:
                break
    except Exception as e:
        iterator.dispose()
        raise e
    iterator.dispose()
)");

    const py::object async_runner = py::globals()["async_runner"];

    const py::module asyncio = py::module::import("asyncio");
    return asyncio.attr("create_task")(async_runner(async_iterator));
}

std::shared_ptr<AsyncIterator> myFunction(std::string prefix)
{
    class MyIterator : public AsyncIterator
    {
        int i;
        std::string prefix;

    public:
        explicit MyIterator(const std::string& prefix)
            : i(0), prefix(prefix)
        {
        }

        void init() override
        {
            std::cout << prefix << " init" << std::endl;
        }

        bool step() override
        {
            std::cout << prefix << " step " << i << std::endl;
            return i++ >= 10; // Return `true` to indicate completion
        }

        void dispose() override
        {
            std::cout << prefix << " dispose" << std::endl;
        }
    };

    return std::make_shared<MyIterator>(prefix);
}

PYBIND11_MODULE(asyncio_example, m)
{
    m.doc() = "Cheap way to create asyncio compatible C++ iterator (Very dirty solution)";

    py::class_<AsyncIterator, std::shared_ptr<AsyncIterator>>(m, "AsyncIterator")
        .def("init", &AsyncIterator::init)
        .def("step", &AsyncIterator::step)
        .def("dispose", &AsyncIterator::dispose);


    m.def("myFunction", [](const std::string& prefix) { return create_async_runner(myFunction(prefix)); },
          "Create an async iterator", py::arg("prefix"));
}
