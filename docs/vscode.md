# Developing workerd with Visual Studio Code

A few helpful tips for developing workerd with Visual Studio Code.

## clangd

The workerd project uses [clangd](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd)
for code completion and diagnostics.

clangd relies on a `compile_commands.json` file to support cross referencing of symbols in source files (`Find all references`, `Go to implementation`, etc).
There are some C++ template constructs from capnproto code in workerd that do not work well with code navigation using clangd. In this case, it is worth
using `Find in files...` to find the classes in the template.

## Generate a compile_commands.json file

To support code navigation in VSCode with clangd, generate a `compile_commands.json` with:

```sh
$ bazel run //:refresh_compile_commands
```

If this command fails, try again after running:

```sh
$ bazel clean --expunge
$ bazel test //...
```

There is an issue between workerd's bazel setup and Hedron's compile_commands.json generator (tracked in
https://github.com/cloudflare/workerd/issues/506).

## VSCode Tasks

The [.vscode/tasks.json](../.vscode/tasks.json) file included in the project that is seeded with a few useful
tasks for building and testing:
* Bazel Build (dbg, fastbuild, opt).
* Bazel Test
* Generate compile_commands.json.
* Generate rust-project.json.

The [Task Explorer](https://marketplace.visualstudio.com/items?itemName=spmeesseman.vscode-taskexplorer) extension adds an
explorer for tasks to ease discovery. Tasks can also be run from the VSCode Command Palette (`Tasks: Run Task`).

## Debugging

Debugging workerd inside VSCode is supported on Linux (x64) and OS X (arm64).

### Pre-requisites:
* Install the [Microsoft C/C++ Extension Pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) as this provides the `cppdbg` extension for debugging.
* The Microsoft C/C++ Intellisense plugin is not compatible with the clangd extension. We recommend disabling the Microsoft Intellisense extension for this project (`Settings` -> `C_Cpp: Intelli Sense Engine` -> `disabled`).
* Create a VSCode workspace for your `workerd` source directory (`File` -> `Save Workspace As...`) if one does not exist already.

### Debugging workerd

To launch the debugger click, `Run -> Start Debugging (F5)`. This will perform a debug build of workerd
and then prompt for a workerd configuration to serve (the default is
`${workspaceFolder}/samples/helloworld/config.capnp`). `workerd` will then be launched with the debugger attached from within
VSCode.

### Debug configurations

The [.vscode/launch.json](../.vscode/launch.json) file contains the configurations for debugging `workerd`. On Linux, the debug engine uses `gdb`, and on
OS X, the debug engine uses `lldb`.
