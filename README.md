# Asyncio Coroutine Integration with Pybind11

This repository demonstrates a basic, albeit rough, implementation of integrating asyncio with C++ using Pybind11. It is
a minimal example, not a comprehensive solution, but it provides a starting point for those looking to bridge C++
coroutines with Python's asyncio.

I made this because after a few weeks of researching and trying out, I still haven't found a good and clean way of doing
it, which would allow me to directly call my cpp function and use it with await.

## Overview

The example showcases how to create an iterator in C++ and expose it to Python as an asynchronous iterator. This allows
the iterator to be awaited in Python, enabling integration with asyncio workflows.

### Key Notes:

- The example avoids threading, relying solely on coroutine-based execution.
- The implementation is intentionally minimal and not optimized for production use.
- The primary goal is to illustrate the concept, leaving further refinements to the user.

## Structure

The repository includes:

- A `main.cpp` file defining the C++ iterator and an example function that uses it.
- A `main.py` file demonstrating how to use the wrapped C++ iterator as an asynchronous iterator in Python.
- An `install.sh` script for setting up the example environment.

## Prerequisites

To run this example, you will need:

- Python with a virtual environment (venv) set up.
- [CMake](https://cmake.org/) or [Ninja](https://ninja-build.org/) for building the C++ extension.
- A compiler compatible with C++17 or later.

## Installation and Usage

1. **Set up a virtual environment:**
   ```bash
   python -m venv venv
   source venv/bin/activate  # On Windows: venv\Scripts\activate
   ```

2. **Install the example:**
   Run the provided `install.sh` script to build and install the extension into your virtual environment:
   ```bash
   ./install.sh
   ```

3. **Run the Python example:**
   With the virtual environment activated, execute the `main.py` script:
   ```bash
   python main.py
   ```

## Expected Output

The output will demonstrate the asynchronous behavior of the iterators, showing that both iterators run concurrently.

## Limitations

- This is a minimal proof-of-concept and is not production-ready.
- Proper error handling, resource management, and advanced features are not implemented.
- Users are encouraged to expand upon and refine this example based on their specific needs.

## Contributing

Contributions are welcome! If you have ideas for improvements or new features, feel free to submit a pull request or
open an issue. Together, we can enhance this example and make it more robust for various use cases.

---

This example provides a foundation for those looking to explore asyncio integration with C++ using Pybind11. While it is
not comprehensive, it offers a starting point for further development.