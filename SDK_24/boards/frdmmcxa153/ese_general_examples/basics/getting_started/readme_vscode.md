# Getting Started with VS Code

Resources: *ese_general_examples\basics\getting_started*

## <mark>EXPERIMENTAL</mark>

*Using VS Code is experimental. This guide might therefor not be complete!*

## Hardware

The NXP FRDM-MCXA153 development board is used in this course.
[![FRDM-MCXA153](https://www.nxp.com/assets/images/en/dev-board-image/FRDM-MCXA153-TOP.jpg)](https://www.nxp.com/design/design-center/development-boards/general-purpose-mcus/frdm-development-board-for-mcx-a14x-a15x-mcus:FRDM-MCXA153){:target="_blank"}

A board overview, product details, documentation and other resources is provided
on the [NXP website](https://www.nxp.com/design/design-center/development-boards/general-purpose-mcus/frdm-development-board-for-mcx-a14x-a15x-mcus:FRDM-MCXA153){:target="_blank"}. A board can purchased from ARLE, the NXP website or other electronics suppliers.

## SDE overview

The following diagram shows the tools and dependencies used in the Software Development Environment. A detailed description of where to download and how to configure these tools is given in the rest of this document.

![MCUXpresso Installed SDKs](./../../../../imgs/getting_started_sde_vscode.png)

## Download and install VS Code

Download and install the latest version of VS Code for your operating system.

[VS Code](https://code.visualstudio.com/download){:target="_blank"}

## Download and install extensions

Download and install the MCUXpresso for VS Code extension.

1. Start VS Code
2. Install the MCUXpresso for VS Code extension: [MCUXpresso for VS Code](https://marketplace.visualstudio.com/items?itemName=NXPSemiconductors.mcuxpresso){:target="_blank"}

## Download and install MCUXpresso for VS Code components

After installing the MCUXpresso for VS Code extension, you should be prompted to start the MCUXpresso Installer. If not, click *Open MCUXpresso Installer* from the *Quickstart panel* in the MCUXpresso for VS Code extension.

The MCUXpresso Installer looks like this:

![MCUXpresso Installer](./../../../../imgs/getting_started_vscode_mcuxpresso_installer.png)

1. In this installer, select (at least) the following options:
    - MCUXpresso SDK Developer
    - ARM GNU Toolchain
    - Standalone Toolchain Add-ons
    - LinkServer
    - SEGGER J-Link
2. Click *Install*
3. Close the installer when the installation is finished

## Download SDK

The Software Development Kit (SDK) contains source files, project configuration files, documentation, example projects, etc. to help users get started. The updated SDK is available for download as a ZIP archive on #OO.

1. Download the file SDK_x_y_z_FRDM-MCXA153_FOR_MIC3_Vn.zip from #OO.

*Note. If you would like to generate your own SDK, **without ESE examples**, use the [SDK builder for the FRDM-MCXA153](https://mcuxpresso.nxp.com/en/builder?hw=FRDM-MCXA153){:target="_blank"}*

## Import SDK in MCUXpresso for VS Code

The downloaded SDK must be imported in MCUXpresso for VS Code.

1.	Start VS Code
2.	Select the MCUXpresso for VS Code extension
3.	Click Import Repository
4.	Click *LOCAL ARCHIVE*
5. Archive: select the dowloaded SDK
6. Location: any location will do, but *the path should be as short as possible!*
7. Name: leave default
8. Create Git repository: not checked

The result should look similar to this:

![MCUXpresso for VS Code import repository](./../../../../imgs/getting_started_vscode_import_repo.png)

9.	Click *Import*
10. Wait for the import to finish

After a successful import, the installed SDK is added to the *Installed repositories* view. For example:

![MCUXpresso for VS Code Installed repositories](./../../../../imgs/getting_started_vscode_installed_repos.png)

*Note. An SDK can be removed by a right-mouse click on the SDK and selecting the appropriate option from the context menu that pops-up.*

## Import example project in MCUXpresso for VS Code

Import example projects from the SDK in MCUXpresso for VS Code.

1.	Start VS Code
2.	Select the MCUXpresso for VS Code extension
3.	Click *Import Example from Repository*
4. Repository: select the downloaded SDK
5. Toolchain: ARM GNU Toolchain *(as installed during with MCUXpresso Installer)*
6. Board: FRDM-MCXA153
7. Template: ese_general_examples/basics/ese_general_examples_basics_getting_started
8. Name: leave default
9. Location: any location will do, but *the path should be as short as possible!*

The result should look similar to this:

![MCUXpresso for VS Code import example](./../../../../imgs/getting_started_vscode_import_example.png)

10. Click *Create*
11. Wait for the import to finish

## MCU-Link firmware update (optional)

The FRDM-MCXA153 board comes with an [MCU-Link debug probe](https://docs.nxp.com/bundle/UM12012/page/topics/MCU_Link_OB_debug_probe.html){:target="_blank"}. Out of the box, the CMSIS-DAP firmware is programmed. However, J-Link firmware is preferred. Changing the MCU-Link to J-Link firmware is described [here](https://docs.nxp.com/bundle/UM12012/page/topics/Updating_MCU_Link_firmware.html){:target="_blank"}.

## Check board connection

Check if the board is recognized by MCUXpresso for VS Code.

1. Connect the FRDM-MCXA153 board to the computer with a USB cable (J15 - MCU-Link)
2. Check the *Debug probes* view to make sure the board is detected. If not, click the *Refresh debug probes* icon

![MCUXpresso for VS Code debug probes](./../../../../imgs/getting_started_vscode_debug_probes.png)

## Build and run the Getting Started application

The imported project is visible in the *Projects* view in the MCUXpresso for VS Code extension.

![MCUXpresso for VS Code build and debug](./../../../../imgs/getting_started_vscode_build_debug.png)

Build and run the project.

1.	Click the *Build* icon
2.	Check the Terminal window for the build to finish successfully
3. Click the *debug* icon
4. Wait for the debugger to start
5. Click Run, the green LED starts blinking
