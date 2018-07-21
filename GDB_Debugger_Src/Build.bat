arm-none-eabi-gcc -g -mcpu=cortex-m0plus -mthumb --specs=nosys.specs -c src/startup_MKL46Z4.s
arm-none-eabi-gcc -c -g -mcpu=cortex-m0plus -mthumb --specs=nosys.specs src/main.c -Iinc
arm-none-eabi-gcc -c -g -mcpu=cortex-m0plus -mthumb --specs=nosys.specs src/system_MKL46Z4.c -Iinc
# arm-none-eabi-ld startup_MKL46Z4.o system_MKL46Z4.o main.o -Tsrc/MKL46Z4.ld -o MKL46Z4.elf -Map=MKL46Z4.map
arm-none-eabi-gcc -g -mcpu=cortex-m0plus -mthumb --specs=nosys.specs startup_MKL46Z4.o system_MKL46Z4.o main.o -Tsrc/MKL46Z4.ld -o MKL46Z4.elf