## Command line

In order to build the project using the command line, run *make* inside the root directory. Both the ISPU project and the Nucleo project will be automatically built.

## Eclipse

In order to import both the ISPU and Nucleo projects, from the main menu, go to "File", "Import...", select "General", "Existing Projects into Workspace", browse for the root directory of this project, check "Search for nested projects" in "Options", select both projects and click on "Finish".

Make sure the ISPU project is built first. If not, from the main menu, go to "Window", "Preferences", browse to "General", "Workspace", "Build", uncheck "Use default build order" and change the "Project build order" (ISPU project first and Nucleo project second).

In order to build the projects according to the selected order, from the main menu, go to "Project" and click on "Build All".

