# Windows-System-Programming

A practice project to simulate player fights with monsters. 

Reference: [windows system programming](https://www.books.com.tw/products/0010507046).

# Features

1. Execute a program with argvs, `--start`, `--player`, `--monster`, `--fight`.

2. For `--start`, program will act as server named pipe to receive other commands.

3. For `--player`, `--monster`, programs will act as client named pipe to send characters information.

4. Ather `--fight`, threads will be started, and player will battle with monster.

5. During fighting, there will be some lock to prevent player from attacking or getting hurt all the time.

6. For Each attack, logs will be generated through file system APIs.

7. Additionally, functions will be linked by static link or dynamic link.

# Summary

In this project, there will be file system, loading, threads, critical sections, and pipe APIs inside. 
