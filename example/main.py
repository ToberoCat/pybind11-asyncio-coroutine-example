import asyncio

from asyncio_example import myFunction


class AsyncIterable:
    def __init__(self, iterable):
        self._iterable = iterable

    def __await__(self):
        return self._run().__await__()

    async def _run(self):
        for _ in self._iterable:
            await asyncio.sleep(0)


def as_task(iterable):
    return AsyncIterable(iterable)


async def main():
    it1 = as_task(myFunction())
    it2 = as_task(myFunction())

    await asyncio.gather(it1, it2)
    print("Done")


asyncio.run(main())
