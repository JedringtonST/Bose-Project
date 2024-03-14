We have renamed the package because it had inhouse naming. 

Inside contains:


•	A completely working NORM example to use as a template.

•	A “almost” completely working MotionFX sensorfusion example.  For you to get the motionFX example you need to go to github https://github.com/STMicroelectronics/x-cube-ispu/tree/main/Ispu/ism330is_lsm6dso16is/sensor_fusion_6x and copy everything found in the ISPU folder on github into the ISPU folder in the project.

•	To get the white box solution you should copy the norm folder rename it to something else and copy paste the euler2quat.c contents into the main.c. You will then have to make edits to the application.c found in the core/src of the project so the output is read and formatted correctly. They can look at the ISPU_DOUT_X in the main.c to understand the output and copy that format to the application.c file to get the corresponding serial output. 



You can build the project but running make inside the root directory for each of the 3 different projects. If you would like to run it in eclipse there is a readme included that describes in detail the steps required. 

To get the command line running you can find the information in our github ispu-examples.

If you or them have any questions about getting this working let me know. 
