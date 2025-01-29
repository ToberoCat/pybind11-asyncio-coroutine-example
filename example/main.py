import asyncio

from asyncio_example import start_counter


async def main():
    task1 = start_counter(10)
    task2 = start_counter(15)
    
    await start_counter(2)
    
    await task1
    await task2
    
    await asyncio.sleep(1)
    await start_counter(5)

asyncio.run(main())
