# 🔗 pipex - Mastering Unix Pipes and Process Management

<div align="center">

[![42 School](https://img.shields.io/badge/42-School-000000?style=for-the-badge&logo=42&logoColor=white)](https://42.fr/)
[![C](https://img.shields.io/badge/C-A8B9CC?style=for-the-badge&logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Unix](https://img.shields.io/badge/Unix-System-FCC624?style=for-the-badge&logo=linux&logoColor=black)](https://en.wikipedia.org/wiki/Unix)
[![Norminette](https://img.shields.io/badge/Norminette-✅_Passing-success?style=for-the-badge)](https://github.com/42School/norminette)
[![Grade](https://img.shields.io/badge/Grade-125%2F100-brightgreen?style=for-the-badge)]()

*"Pipes are the arteries of Unix - connecting processes in an endless flow of data"*

</div>

---

## 🎯 Project Overview

**pipex** is where 42 students dive deep into Unix system programming, mastering the art of process creation, inter-process communication, and file descriptor manipulation. This project recreates the behavior of shell pipes, teaching you to think like the shell itself - orchestrating multiple processes to work together seamlessly.

> *"In Unix, everything is a file, and pipes are the highways that connect them."*

## 🌟 The Pipe Philosophy

Pipes embody the Unix philosophy: **"Do one thing and do it well"**. By connecting simple programs together, you can create powerful, complex operations. pipex teaches you to implement this fundamental concept from scratch.

### 🔗 What is a Pipe?

A pipe (`|`) connects the output of one command to the input of another:

```bash
< infile cmd1 | cmd2 > outfile
```

This creates a **data pipeline** where:
- `cmd1` reads from `infile`
- `cmd1`'s output flows directly to `cmd2`'s input
- `cmd2`'s output is written to `outfile`

---

## 🏗️ Technical Architecture

### 🧠 Core Data Flow

```
┌─────────┐    ┌──────┐    ┌─────────┐    ┌──────────┐
│ infile  │───▶│ cmd1 │───▶│  pipe   │───▶│   cmd2   │───▶│ outfile │
└─────────┘    └──────┘    └─────────┘    └──────────┘
     ▲             ▲           ▲               ▲             ▲
  file I/O     process 1   IPC mechanism   process 2     file I/O
```

### 🔧 System Call Arsenal

| System Call | Purpose | Usage in pipex |
|-------------|---------|----------------|
| `pipe()` | Create pipe | Establish communication channel |
| `fork()` | Create process | Spawn child processes for commands |
| `execve()` | Execute program | Replace process with command |
| `dup2()` | Duplicate FD | Redirect stdin/stdout to pipe |
| `wait()` | Wait for child | Synchronize parent with children |
| `access()` | Check file | Validate input/output files |

---

## 🚀 Quick Start

```bash
# Clone this Unix masterpiece
git clone git@github.com:danielgessner/42_Core_pipex.git
cd 42_Core_pipex

# Build the project
make

# Test basic pipe functionality
echo "hello world" > input.txt
./pipex input.txt "cat" "wc -l" output.txt
cat output.txt  # Should show: 1

# Compare with shell behavior
< input.txt cat | wc -l > shell_output.txt
diff output.txt shell_output.txt  # Should be identical
```

## 💡 Usage Examples

### 🎨 Basic Piping
```bash
# Equivalent to: < file1 cat | grep hello > file2
./pipex file1 "cat" "grep hello" file2

# Equivalent to: < data.txt sort | uniq > clean.txt
./pipex data.txt "sort" "uniq" clean.txt

# Equivalent to: < numbers.txt head -5 | tail -1 > result.txt
./pipex numbers.txt "head -5" "tail -1" result.txt
```

### 🔥 Advanced Processing
```bash
# Text processing pipeline
echo -e "apple\nbanana\napple\ncherry" > fruits.txt
./pipex fruits.txt "sort" "uniq -c" counts.txt

# Log analysis simulation
./pipex access.log "grep ERROR" "wc -l" error_count.txt

# Data transformation
./pipex data.csv "cut -d, -f1" "sort" names.txt
```

### 🧪 Error Handling Tests
```bash
# Test with non-existent input file
./pipex nonexistent.txt "cat" "wc" output.txt
# Should show: pipex: nonexistent.txt: No such file or directory

# Test with invalid command
./pipex input.txt "invalidcmd" "cat" output.txt
# Should show: pipex: invalidcmd: command not found

# Test with permission issues
chmod 000 readonly.txt
./pipex readonly.txt "cat" "wc" output.txt
# Should show: pipex: readonly.txt: Permission denied
```

---

## 🛠️ Implementation Deep Dive

### 🧩 Program Structure

```c
// Core function prototypes
int     main(int argc, char **argv, char **envp);
void    pipex(char **argv, char **envp);
void    child_process(char **argv, char **envp, int *pipefd);
void    parent_process(char **argv, char **envp, int *pipefd);
char    **find_path(char **envp);
char    *get_cmd_path(char **paths, char *cmd);
void    execute_cmd(char *cmd, char **envp);
void    error_exit(char *msg);
```

### 🔧 Process Flow Architecture

#### 1. **Pipe Creation**
```c
int pipefd[2];
if (pipe(pipefd) == -1)
    error_exit("pipe failed");

// pipefd[0] = read end
// pipefd[1] = write end
```

#### 2. **Process Forking**
```c
pid_t pid = fork();
if (pid == -1)
    error_exit("fork failed");
else if (pid == 0)
    child_process(argv, envp, pipefd);   // Execute cmd1
else
    parent_process(argv, envp, pipefd);  // Execute cmd2
```

#### 3. **File Descriptor Redirection**
```c
// Child process (cmd1)
dup2(infile_fd, STDIN_FILENO);   // Redirect input
dup2(pipefd[1], STDOUT_FILENO);  // Redirect output to pipe
close(pipefd[0]);                // Close unused read end

// Parent process (cmd2)  
dup2(pipefd[0], STDIN_FILENO);   // Redirect input from pipe
dup2(outfile_fd, STDOUT_FILENO); // Redirect output to file
close(pipefd[1]);                // Close unused write end
```

### 🎯 Command Execution Strategy

#### Path Resolution Algorithm
```c
char *get_cmd_path(char **paths, char *cmd) {
    char *full_path;
    int i = 0;
    
    // If command contains '/', it's already a path
    if (ft_strchr(cmd, '/'))
        return (cmd);
    
    // Search through PATH directories
    while (paths[i]) {
        full_path = ft_strjoin(paths[i], "/");
        full_path = ft_strjoin(full_path, cmd);
        
        if (access(full_path, F_OK | X_OK) == 0)
            return (full_path);
            
        free(full_path);
        i++;
    }
    return (NULL);
}
```

#### Command Parsing and Execution
```c
void execute_cmd(char *cmd, char **envp) {
    char **cmd_args = ft_split(cmd, ' ');
    char **paths = find_path(envp);
    char *cmd_path = get_cmd_path(paths, cmd_args[0]);
    
    if (!cmd_path) {
        ft_putstr_fd("pipex: command not found: ", 2);
        ft_putendl_fd(cmd_args[0], 2);
        exit(127);
    }
    
    if (execve(cmd_path, cmd_args, envp) == -1) {
        perror("execve failed");
        exit(126);
    }
}
```

---

## 🧪 Testing Framework

### 🎯 Functional Tests
```bash
#!/bin/bash
# test_pipex.sh - Comprehensive testing suite

test_basic() {
    echo "Testing basic functionality..."
    echo "hello world" > test_input.txt
    
    # Test our pipex
    ./pipex test_input.txt "cat" "wc -w" our_output.txt
    
    # Test shell equivalent
    < test_input.txt cat | wc -w > shell_output.txt
    
    # Compare results
    if diff our_output.txt shell_output.txt > /dev/null; then
        echo "✅ Basic test PASSED"
    else
        echo "❌ Basic test FAILED"
    fi
}

test_error_handling() {
    echo "Testing error handling..."
    
    # Test non-existent file
    ./pipex nonexistent.txt "cat" "wc" output.txt 2>&1 | grep -q "No such file"
    [ $? -eq 0 ] && echo "✅ Non-existent file test PASSED"
    
    # Test invalid command
    ./pipex test_input.txt "invalidcmd" "cat" output.txt 2>&1 | grep -q "command not found"
    [ $? -eq 0 ] && echo "✅ Invalid command test PASSED"
}
```

### 🔬 Performance Testing
```bash
# Generate large test file
seq 1 100000 > large_file.txt

# Time comparison
time (< large_file.txt sort | uniq > shell_result.txt)
time (./pipex large_file.txt "sort" "uniq" pipex_result.txt)

# Verify identical results
diff shell_result.txt pipex_result.txt
```

### 🏁 Memory Testing
```bash
# Check for memory leaks
valgrind --leak-check=full --show-leak-kinds=all \
  ./pipex input.txt "cat" "wc" output.txt

# Should show: "All heap blocks were freed -- no leaks are possible"
```

---

## 📋 42 Standards Compliance

### ✅ Norminette Perfect
- Maximum 25 lines per function
- Maximum 5 functions per file  
- No forbidden constructs (`for`, `switch`, `case`, `goto`)
- Proper 42 header format
- Clean memory management

### 🎯 Project Requirements
- **Input**: `./pipex infile cmd1 cmd2 outfile`
- **Behavior**: Equivalent to `< infile cmd1 | cmd2 > outfile`
- **Error handling**: Proper exit codes and error messages
- **Memory**: Zero leaks, proper cleanup
- **System calls**: Efficient use of Unix primitives

---

## 🌟 Advanced Features & Edge Cases

### ⚡ Robust Error Handling
```c
// Comprehensive error checking
void check_file_access(char *filename, int mode) {
    if (access(filename, mode) == -1) {
        ft_putstr_fd("pipex: ", 2);
        perror(filename);
        exit(1);
    }
}

// Proper exit codes matching shell behavior
void handle_execve_error(char *cmd) {
    if (errno == ENOENT) {
        ft_putstr_fd("pipex: command not found: ", 2);
        ft_putendl_fd(cmd, 2);
        exit(127);  // Command not found
    } else {
        perror("execve");
        exit(126);  // Command cannot execute
    }
}
```

### 🔧 PATH Resolution Mastery
```c
// Handle complex PATH scenarios
char **find_path(char **envp) {
    int i = 0;
    char *path_str;
    
    // Find PATH in environment
    while (envp[i]) {
        if (ft_strncmp(envp[i], "PATH=", 5) == 0) {
            path_str = envp[i] + 5;
            return (ft_split(path_str, ':'));
        }
        i++;
    }
    
    // Fallback if PATH not found
    return (ft_split("/usr/bin:/bin", ':'));
}
```

### 🛡️ Signal Handling
```c
// Proper cleanup on signals
void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        // Clean up resources
        cleanup_processes();
        exit(130);  // Shell convention for signal termination
    }
}
```

---

## 🧮 System Programming Concepts

### 🔄 Process Lifecycle Management

#### Fork-Exec Pattern
```c
pid_t child_pid = fork();
switch (child_pid) {
    case -1:
        perror("fork failed");
        exit(1);
    case 0:
        // Child process
        setup_child_environment();
        execve(program, args, envp);
        // Never reached if execve succeeds
        perror("execve failed");
        exit(127);
    default:
        // Parent process
        wait_for_child(child_pid);
        cleanup_parent_resources();
}
```

#### File Descriptor Lifecycle
```c
// Pipe creation and management
int pipefd[2];
pipe(pipefd);

// Child: write end
close(pipefd[0]);           // Close read end
dup2(pipefd[1], STDOUT);    // Redirect stdout
close(pipefd[1]);           // Close original write end

// Parent: read end  
close(pipefd[1]);           // Close write end
dup2(pipefd[0], STDIN);     // Redirect stdin
close(pipefd[0]);           // Close original read end
```

### 🎯 Inter-Process Communication

#### Pipe Mechanics
- **Unidirectional**: Data flows one way
- **Buffered**: OS buffers data between processes
- **Blocking**: Reads block until data available
- **Atomic**: Small writes are atomic

#### Synchronization Strategy
```c
// Parent waits for child completion
int status;
waitpid(child_pid, &status, 0);

// Check exit status
if (WIFEXITED(status)) {
    int exit_code = WEXITSTATUS(status);
    // Handle based on exit code
}
```

---

## 🎮 Interactive Examples

### 🔧 Building Complex Pipelines
```bash
# Create test data
echo -e "apple\nbanana\napple\ncherry\nbanana" > fruits.txt

# Count unique fruits
./pipex fruits.txt "sort" "uniq -c" fruit_counts.txt
cat fruit_counts.txt
# Output:
#    2 apple  
#    2 banana
#    1 cherry

# Find longest line
echo -e "short\nmuch longer line\nmedium" > lines.txt
./pipex lines.txt "wc -L" "cat" max_length.txt

# Process CSV data
echo -e "name,age,city\nAlice,25,NYC\nBob,30,LA" > data.csv
./pipex data.csv "cut -d, -f1" "tail -n +2" names.txt
```

### 🧪 System Integration Tests
```bash
# Test with system utilities
./pipex /etc/passwd "grep root" "cut -d: -f1" root_users.txt

# Network-style processing
./pipex server.log "grep ERROR" "wc -l" error_count.txt

# File system operations
./pipex file_list.txt "sort" "uniq" unique_files.txt
```

---

## 📖 The Philosophy of Pipes

> *"Pipes teach us that complexity emerges not from monolithic programs, but from the elegant composition of simple tools. Each program does one thing well, and pipes are the syntax that lets us speak fluently in the language of Unix."*

### 🎭 Design Principles
- **Modularity**: Each command has a single responsibility
- **Composability**: Commands combine to create new behaviors  
- **Simplicity**: Simple interface, powerful combinations
- **Reusability**: Same commands work in different contexts

### 🌊 The Unix Way
- **Text streams**: Universal interface between programs
- **Filter pattern**: Transform input to output
- **Tool combination**: Build complex operations from simple parts
- **Lazy evaluation**: Process data as it flows

---

## 🚀 Real-World Applications

This pipe mastery applies to:

- 🔧 **DevOps**: Building deployment pipelines and automation
- 📊 **Data Processing**: ETL operations and stream processing  
- 🐧 **System Administration**: Log analysis and monitoring
- 🔍 **Security**: Log parsing and threat detection
- 📈 **Analytics**: Real-time data transformation
- 🤖 **Process Automation**: Workflow orchestration

---

## 🎓 Learning Outcomes

After mastering pipex, you understand:

- **Process management**: Creating and controlling child processes
- **Inter-process communication**: Pipes and file descriptors
- **System call mastery**: Low-level Unix programming
- **Error handling**: Robust system programming practices
- **Resource management**: Proper cleanup and resource allocation
- **Unix philosophy**: Building complex systems from simple components

---

## 🌟 Advanced Concepts Explored

### 🔄 Process Synchronization
- Fork-exec pattern implementation
- Parent-child process coordination
- Exit status handling and propagation
- Signal handling in multi-process environments

### 🗂️ File Descriptor Management
- Descriptor duplication and redirection
- Pipe creation and management
- Resource cleanup and leak prevention
- Standard stream manipulation

### 🧠 System Programming Patterns
- Error handling best practices
- PATH resolution algorithms
- Environment variable processing
- Memory management in system calls

---

<div align="center">

**Made with ❤️ at 42 Heilbronn**

*Where system programming becomes second nature*

---

*"In the symphony of Unix, pipes are the conductor's baton, orchestrating the flow of data through the eternal dance of processes"*

</div>