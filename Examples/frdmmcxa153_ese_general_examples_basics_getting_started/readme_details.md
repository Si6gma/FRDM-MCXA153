# Getting Started

Resources: *ese_general_examples\basics\getting_started*

*Rather use VS Code? Check this [getting started](./readme_vscode.md){:target="_blank"} instead!*

## Hardware

The NXP FRDM-MCXA153 development board is used in this course.
[![FRDM-MCXA153](https://www.nxp.com/assets/images/en/dev-board-image/FRDM-MCXA153-TOP.jpg)](https://www.nxp.com/design/design-center/development-boards/general-purpose-mcus/frdm-development-board-for-mcx-a14x-a15x-mcus:FRDM-MCXA153){:target="_blank"}

A board overview, product details, documentation and other resources is provided
on the [NXP website](https://www.nxp.com/design/design-center/development-boards/general-purpose-mcus/frdm-development-board-for-mcx-a14x-a15x-mcus:FRDM-MCXA153){:target="_blank"}. A board can purchased from ARLE, the NXP website or other electronics suppliers.

## SDE overview

The following diagram shows the tools and dependencies used in the Software Development Environment. A detailed description of where to download and how to configure these tools is given in the rest of this document.

![MCUXpresso Installed SDKs](./../../../../imgs/getting_started_sde.png)

## Download and install MCUXpresso-IDE

The preferred SDE is MCUXpresso-IDE and is downloaded from the NXP website:

[MCUXpresso-IDE](https://www.nxp.com/design/design-center/software/development-software/mcuxpresso-software-and-tools-/mcuxpresso-integrated-development-environment-ide:MCUXpresso-IDE){:target="_blank"}

Select MCUXpresso-IDE v24.12.148 or newer!

## Download SDK

The Software Development Kit (SDK) contains source files, project configuration files, documentation, example projects, etc. to help users get started. The updated SDK is available for download as a ZIP archive on #OO.

1. Download the file SDK_x_y_z_FRDM-MCXA153_FOR_ESE_n.zip from #OO.

*Note. Successfully tested MCUXpresso-IDE and SDK versions are:*

- *SDK_24.12.00_FRDM-MCXA153 with MCUXpresso-IDE v24.12_148*
- *SDK_2.16.000_FRDM-MCXA153 with MCUXpresso-IDE v11.10.0_3148*
- *SDK_2.14.2_FRDM-MCXA153 with MCUXpresso-IDE v11.8.0_1165*

*Note. If you would like to generate your own SDK, **without ESE examples**, use the [SDK builder for the FRDM-MCXA153](https://mcuxpresso.nxp.com/en/builder?hw=FRDM-MCXA153){:target="_blank"}*

## Import SDK in MCUXpresso-IDE

The downloaded SDK must be imported in MCUXpresso-IDE.

1.	Start MCUXpresso-IDE
2.	Close the welcome screen (if opened)
3.	Window -> Show View -> *Installed SDKs*
4.	Drag-and-drop the file SDK_x_y_z_FRDM-MCXA153_FOR_ESE_n.zip into the *Installed SDKs* view
5.	Wait until the import finishes

After a successful import, the installed SDK is added to the *Installed SDKs* view. For example:

![MCUXpresso Installed SDKs](./../../../../imgs/mcuxpresso_sdk_overview.png)

*Note. If you need to re-import the same SDK version, chances are that the previous SDK files were not removed from your file system. In order to remove an SDK version completely from your file system, delete the following folder:*

```txt
c:\Users\<user>\mcuxpresso\02\SDKPackages\SDK_x_y_z_FRDM-MCXA153_FOR_ESE_n
```

## Import example project in MCUXpresso-IDE

Example projects are imported from the SDK in MCUXpresso-IDE as follows:

1.	Start MCUXpresso-IDE
2.	Close the welcome screen (if opened)
3.	Click *Import SDK example(s)…*
4.	Click the *frdmmcxa153* board
5.	Click *Next >*
6.	Select one or more projects from the list:
   ![Select an example project](./../../../../imgs/getting_started_example_projects.png)
7.	Click *Finish*
8.	Wait for the import to finish

## MCU-Link firmware update (optional)

The FRDM-MCXA153 board comes with an [MCU-Link debug probe](https://docs.nxp.com/bundle/UM12012/page/topics/MCU_Link_OB_debug_probe.html){:target="_blank"}. Out of the box, the CMSIS-DAP firmware is programmed. However, J-Link firmware is preferred. Changing the MCU-Link to J-Link firmware is described [here](https://docs.nxp.com/bundle/UM12012/page/topics/Updating_MCU_Link_firmware.html){:target="_blank"}.

## Build and run the Getting Started application

If all of the above prerequisites have been downloaded and installed, test the
installation by building and running it as described in the [readme](./readme.md){:target="_blank"}
