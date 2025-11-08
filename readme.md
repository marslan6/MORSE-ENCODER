# Debugging and building (XMC4500) — VS Code

## Prerequisites
- Visual Studio Code
- Install the Cortex-Debug extension: https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug
- SEGGER J-Link tools (JLinkGDBServer) and arm-none-eabi toolchain installed and in PATH as needed.

## student.mk
To ensure correct stepping during debug mode, add the following line.  
`SCFLAGS += -Og -fno-omit-frame-pointer -fno-inline`

## .vscode/tasks.json
All settings are parametrized. Copy and paste file directly to use.

## .vscode/settings.json
Set the parameter `"PROJECT.NAME": "MORSE_TIME"` to your folder name where .c and makefiles are listed.

## .vscode/launch.json
Put .c, Makefile, student.mk files under new folder named as `"PROJECT.NAME"`

|------- `"NAME"` 
|   
|------- /.vscode  
|  
|------- /`"PROJECT.NAME"`  
|    
|    
|    

Create `.vscode/launch.json` and adjust `runToEntryPoint` to match your project layout:

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "J-Link (XMC4500)",
            "type": "cortex-debug",
            "request": "launch",
            "servertype": "jlink",
            "device": "XMC4500",
            "interface": "swd",
            "runToEntryPoint": "main",
            "cwd": "${workspaceFolder}",
            "executable": "${workspaceFolder}/${config:PROJECT.NAME}/build/main.elf",
            "gdbPath": "/usr/bin/gdb-multiarch",
            "serverpath": "JLinkGDBServer"
        }
    ]
}
```

## .vscode/c_cpp_properties.json
Example `c_cpp_properties.json` for IntelliSense. Update include paths and defines to match your installation:

```json
{
    "configurations": [
        {
            "name": "ARM-GCC",
            "includePath": [
                "${workspaceFolder}/**",
                "/opt/XMClib/XMC_Peripheral_Library_v2.1.16/XMCLib/inc",
                "/opt/XMClib/XMC_Peripheral_Library_v2.1.16/CMSIS/Include",
                "/opt/XMClib/XMC_Peripheral_Library_v2.1.16/CMSIS/Infineon/XMC4500_series/Include",
                "/opt/XMClib/XMC_Peripheral_Library_v2.1.16/ThirdPartyLibraries/**"
            ],
            "defines": [
                "XMC4500_F100x1024"
            ],
            "compilerPath": "/usr/bin/arm-none-eabi-gcc",
            "cStandard": "gnu99",
            "cppStandard": "gnu++17",
            "intelliSenseMode": "linux-gcc-arm"
        }
    ],
    "version": 4
}
```

## Build and Flash
Run these from your workspace root (adjust path if workspace is the `MORSE_TIME` folder):

```bash
cd ${workspaceFolder}/MORSE_TIME  
make clean
make
make program    # flash to Cortex-M4 (non-debug)
```

## Start a debug session
1. Open `main.c` (or your entry source file) in VS Code.  
2. Press F5 to start the Cortex-Debug session.

Notes:
- In the `settings.json` set parameter `${config:PROJECT.NAME}` to your project name.
- Ensure `executable` in `launch.json` points to the built ELF.
- If debugging does not start, verify `gdbPath` and `serverpath` are correct and that J-Link is connected.
- Adjust `runToEntryPoint` if your project uses a different entry function.
- Copy `.vscode`, `Makefile`, and `student.mk` to your project.
- Set `LD_NAME` in your `student.mk` to your project name.
- Adjust `SRCS` `HDRS` `LIBSRCS` according to project structure.
- To disable copilot auto-pilot code suggestions: `File > Preferences > Settings > Github Copilot Enable > (Adjust) Item * = False`