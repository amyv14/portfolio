# Mini-Shell

A Unix-style command-line shell written in C that supports process creation, job control, pipelines, and interactive command execution. This project demonstrates low-level control over processes, signals, and the UNIX API.

---

## Brief Video Demo


https://github.com/user-attachments/assets/913c40d8-aee5-4990-95d9-de94504468ef


## Overview

Mini-Shell is a lightweight custom shell that provides an interactive interface to the operating system. It can parse commands, spawn child processes, execute programs, manage background jobs, and handle complex features like multi-stage pipelines and chained execution.

The shell supports:

- Executing Linux commands via `fork()` and `exec()`
- Job control (`jobs`, `fg`, `bg`)
- Foreground and background processes (`&`)
- Arbitrary-length pipelines (`ls -l | grep .c | wc -l`)
- Built-in commands (`cd`, `exit`, `help`)
- Custom built-in command (`history`)
- Command chaining (`&&`, `||`, `;`)
- Signal handling for `Ctrl+C` and `Ctrl+Z`
- Persistent interactive prompt: `mini-shell>`

---

## Features

### Built-in Commands

| Command | Description |
|--------|-------------|
| `cd <path>` | Change directory inside the shell process. |
| `exit` | Safely terminates the shell and reaps background jobs. |
| `help` | Displays all built-in commands. |
| `jobs` | Lists active or suspended background jobs. |
| `fg <job#>` | Bring a background job into the foreground. |
| `bg <job#>` | Resume a suspended job in the background. |
| `history`   | Prints a numbered list of previously entered commands. |

---

### Process Management

- Foreground commands block the shell until they finish.
- Background commands (`cmd &`) run concurrently and are tracked in a job table.
- All jobs are cleaned up when the shell exits.

---

### Signal Handling

- **Ctrl+C (SIGINT):**  
  Prints `mini-shell terminated` and kills all running jobs.

- **Ctrl+Z (SIGTSTP):**  
  Suspends the foreground job and places it into the job list.

Both signals use safe handlers to avoid race conditions.

---

### Implementation Notes

Mini-Shell is written entirely in C and uses:
- `fork()` for process creation
- `execvp()` for command execution
- `pipe()` + `dup2()` for pipelines
- `wait()` / `waitpid()` for synchronization
- `signal()` / `sigaction()` for safe signal handling
- A job management struct for background/suspended tasks
- A dynamic string helper for parsing commands

### To run locally

1. First, `cd systems/my_shell`
2. run `make all`
3. This creates the shell executable. Next, run `./bin/shell` to enter the shell.
4. Use `exit` to exit the shell. 

## Summary 

### What This Project Demonstrates

- Understanding of Unix process life cycles
- Designing a REPL-style interactive shell
- Job control and signaling
- Low-level use of fork, exec, dup2, and descriptors
- Building pipelines manually
- Clean modular C design across multiple components

This project offers a deep understanding of how real shells work internally and how user programs interact with the OS.
