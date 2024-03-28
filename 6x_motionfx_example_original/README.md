## Add ISPU code for 6x sensor fusion

In order to use this firmware to test the MotionFX library running on ISPU, fill the "ispu" folder with the contents of the "ispu" folder of the "sensor_fusion_6x" example available on GitHub: [https://github.com/STMicroelectronics/ispu-examples/tree/master/ism330is_lsm6dso16is/sensor_fusion_6x/ispu](https://github.com/STMicroelectronics/ispu-examples/tree/master/ism330is_lsm6dso16is/sensor_fusion_6x/ispu). The same files can also be obtained from the X-CUBE-ISPU software package: [https://www.st.com/en/embedded-software/x-cube-ispu.html](https://www.st.com/en/embedded-software/x-cube-ispu.html).

## Command line

In order to build the project using the command line, run *make* inside the root directory. Both the ISPU project and the Nucleo project will be automatically built.

## Eclipse

In order to import both the ISPU and Nucleo projects, from the main menu, go to "File", "Import...", select "General", "Existing Projects into Workspace", browse for the root directory of this project, check "Search for nested projects" in "Options", select both projects and click on "Finish".

Make sure the ISPU project is built first. If not, from the main menu, go to "Window", "Preferences", browse to "General", "Workspace", "Build", uncheck "Use default build order" and change the "Project build order" (ISPU project first and Nucleo project second).

In order to build the projects according to the selected order, from the main menu, go to "Project" and click on "Build All".

