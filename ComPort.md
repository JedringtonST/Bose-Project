We have renamed the package because it had inhouse naming. 

Inside contains:


•	A completely working NORM example to use as a template.

•	A “almost” completely working MotionFX sensorfusion example.  For you to get the motionFX example you need to go to github [Found Here](https://github.com/STMicroelectronics/x-cube-ispu/tree/main/Ispu/ism330is_lsm6dso16is/sensor_fusion_6x) and copy everything found in the ISPU folder on github into the ISPU folder in the project.

•	To get the white box solution you should copy the norm folder rename it to something else and copy paste the euler2quat.c contents into the main.c. You will then have to make edits to the application.c found in the core/src of the project so the output is read and formatted correctly. They can look at the ISPU_DOUT_X in the main.c to understand the output and copy that format to the application.c file to get the corresponding serial output. 



You can build the project but running make inside the root directory for each of the 3 different projects. If you would like to run it in eclipse there is a readme [Found Here](https://github.com/JedringtonST/Bose-Project/tree/main/norm_example) included that describes in detail the steps required. 

To get the command line running you can find the information in our github ispu-examples [Found Here](https://github.com/STMicroelectronics/ispu-examples) .

If you or them have any questions about getting this working let me know. 






To get the uart/com commands:


To be most flexible I added generic commands to write (and read) registers, both in default page and ISPU page:
•	*read XX
•	*write XX YY
•	*read_ispu XX
•	*write_ispu XX YY

where XX is the register address in hexadecimal format and YY is the value to write in the register in hexadecimal format. At the moment these commands print back what was written to or read from the register for visual feedback. Especially for the write, this might be undesired in order to avoid disrupting the data streaming. In that case the print can be easily removed or modified in the code in “Core/Src/application”.

                if (sscanf(uart_buff, "write%02hhX%02hhX", &reg, &val) == 2) {
                     write(reg, val);
                     printf("wrote 0x%02hhX = 0x%02hhX\n", reg, val);
                }

                if (sscanf(uart_buff, "read%02hhX", &reg) == 1) {
                     read(reg, &val, 1);
                     printf("read 0x%02hhX = 0x%02hhX\n", reg, val);
                }

                if (sscanf(uart_buff, "write_ispu%02hhX%02hhX", &reg, &val) == 2) {
                     write(0x01, 0x80);
                     write(reg, val);
                     write(0x01, 0x00);
                     printf("wrote 0x%02hhX = 0x%02hhX\n", reg, val);
                }

                if (sscanf(uart_buff, "read_ispu%02hhX", &reg) == 1) {
                     write(0x01, 0x80);
                     read(reg, &val, 1);
                     write(0x01, 0x00);
                     printf("read 0x%02hhX = 0x%02hhX\n", reg, val);
                }

In case of more complex needs (e.g., write multiple registers in one command), it is very easy to implement custom commands. Commands must always start with the ‘*’ character and end with the ‘\n’ (new line) character.

In order to add a new command, the parsing must be added in “Core/Src/application.c”. When a new command is received, the “uart_received” flag is asserted and the command (with the initial ‘*’ character removed) can be found in “uart_buff”. A new if statement must be added to the code below to recognize and parse the command. This can be done in different ways: for example, a sscanf checking the number of inputs correctly read can be used to identify the command and read the command parameters (if any) at the same time, or in alternative a strcmp or strncmp can be used to compare “uart_buff” with the expected command string.

     while (1) {
           // handle commands received from uart
           if (uart_received) {
                if (sscanf(uart_buff, "res%hu", &print_results) > 0) {
                     if (print_results)
                           printf("Enabled results print.\n");
                     else
                           printf("Disabled results print.\n");
                }

                if (sscanf(uart_buff, "time%hu", &print_time) > 0) {
                     if (print_time)
                           printf("Enabled execution time print.\n");
                     else
                           printf("Disabled execution time print.\n");
                }

                uint8_t reg, val;

                if (sscanf(uart_buff, "write%02hhX%02hhX", &reg, &val) == 2) {
                     write(reg, val);
                     printf("wrote 0x%02hhX = 0x%02hhX\n", reg, val);
                }

                if (sscanf(uart_buff, "read%02hhX", &reg) == 1) {
                     read(reg, &val, 1);
                     printf("read 0x%02hhX = 0x%02hhX\n", reg, val);
                }

                if (sscanf(uart_buff, "write_ispu%02hhX%02hhX", &reg, &val) == 2) {
                     write(0x01, 0x80);
                     write(reg, val);
                     write(0x01, 0x00);
                     printf("wrote 0x%02hhX = 0x%02hhX\n", reg, val);
                }

                if (sscanf(uart_buff, "read_ispu%02hhX", &reg) == 1) {
                     write(0x01, 0x80);
                     read(reg, &val, 1);
                     write(0x01, 0x00);
                     printf("read 0x%02hhX = 0x%02hhX\n", reg, val);
                }

                uart_size = 0;
                uart_received = 0;
           }

In order to write to the device registers, the “write” function can be used:

void write(uint8_t reg, uint8_t val);
