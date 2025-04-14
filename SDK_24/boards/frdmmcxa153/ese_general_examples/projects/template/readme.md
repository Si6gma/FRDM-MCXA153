Overview
========
This is a template project. Use this template to start your own project.

Details
====================
Make a copy of the folder *template*. Give this folder the name of your project, such as *my_project*.

*Important note.* For all IDEs, relative paths are used in the settings files. This means that the *my_project* folder should be in the same folder as the *template* folder.

Take the following steps for your IDE of choice for renaming the project and adding/removing source files.

MCUXpresso-IDE
--------------

1. Open the file *.\mcuxpresso\.project* in a text editor.
2. To change the project name
    - Search-and-replace *template* with *my_project*.
3. To add a source file
    - In Windows explorer, copy the file to the root folder of *my_project* (the same location as the other source files).
    - In the file *.\mcuxpresso\.project*, search for *main.c*.
    - Copy-and-paste everything from \<link\> to \</link\> (there is one occurrence).
    - Within the pasted section, replace *main.c* with the new filename.
4. To remove a source file
    - In Windows explorer, delete the file from the root folder of *my_project*
    - In the file *.\mcuxpresso\.project*, search for the filename.
    - Remove everything from \<link\> to \</link\> (there is one occurrences, one for each target).
    
Keil MDK-ARM
------------

1. Rename the file *.\mdkarm\template.uvoptx* to *.\mdkarm\my_project.uvoptx*
2. Rename the file *.\mdkarm\template.uvprojx* to *.\mdkarm\my_project.uvprojx*
3. Open the file *.\mdkarm\my_project.uvprojx* in a text editor.
4. To change the project name
    - Search-and-replace *template* with *my_project*.
5. To add a source file
    - In Windows explorer, copy the file to the root folder of *my_project* (the same location as the other source files).
    - In the file *.\mdkarm\my_project.uvprojx*, search for *main.c*.
    - Copy-and-paste everything from \<File\> to \</File\> (there are two occurrences, one for each target).
    - Within the pasted section, replace *main.c* with the new filename.
6. To remove a source file
    - In Windows explorer, delete the file from the root folder of *my_project*
    - In the file *.\mdkarm\my_project.uvprojx*, search for the filename.
    - Remove everything from \<File\> to \</File\> (there are two occurrences, one for each target).
    
ARM GCC
-------

1. Open the file *.\armgcc\CMakeLists.txt* in a text editor.
2. To change the project name
    - Search-and-replace *template* with *my_project*.
3. To add a source file
    - In Windows explorer, copy the file to the root folder of *my_project* (the same location as the other source files).
    - In the file *.\armgcc\CMakeLists.txt*, search for *add_executable(${MCUX_SDK_PROJECT_NAME}*.
    - Add an entry with the corresponding filename to the list.
3. To remove a source file
    - In Windows explorer, delete the file from the root folder of *my_project*
    - In the file *.\armgcc\CMakeLists.txt*, search for *add_executable(${MCUX_SDK_PROJECT_NAME}*.
    - Remove the corresponding entry from the list.

Hardware requirements
=====================
- FRDM-MCXA153 board
- Type-C USB cable

Board settings
==============
- Default

Preparation
===========
- Connect the type-C USB cable between the host PC and the MCU-Link port (J15) on the target board.
- Build the application.
- Run or debug the application.

Result
======
- Open the VCOM (115200-8n1) in a terminal application of your choice. A timeout message is printed every second.
- Press SW2. The green LED will switch on.
- Press SW3. The green LED will switch off.
