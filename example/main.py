import asyncio

from asyncio_example import myFunction


async def main():
    task1 = myFunction("Coro1")
    task2 = myFunction("Coro2")
    
    await myFunction("Coro3")
    
    await task1
    await task2
    
    await asyncio.sleep(5)
    await myFunction("Coro4")

asyncio.run(main())
