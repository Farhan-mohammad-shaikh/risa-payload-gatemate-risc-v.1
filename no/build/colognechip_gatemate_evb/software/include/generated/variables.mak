PACKAGES=libc libcompiler_rt libbase libfatfs liblitespi liblitedram libliteeth liblitesdcard liblitesata bios
PACKAGE_DIRS=/home/lucas/Dokumente/litex/litex/litex/soc/software/libc /home/lucas/Dokumente/litex/litex/litex/soc/software/libcompiler_rt /home/lucas/Dokumente/litex/litex/litex/soc/software/libbase /home/lucas/Dokumente/litex/litex/litex/soc/software/libfatfs /home/lucas/Dokumente/litex/litex/litex/soc/software/liblitespi /home/lucas/Dokumente/litex/litex/litex/soc/software/liblitedram /home/lucas/Dokumente/litex/litex/litex/soc/software/libliteeth /home/lucas/Dokumente/litex/litex/litex/soc/software/liblitesdcard /home/lucas/Dokumente/litex/litex/litex/soc/software/liblitesata /home/lucas/Dokumente/litex/litex/litex/soc/software/bios
LIBS=libc libcompiler_rt libbase libfatfs liblitespi liblitedram libliteeth liblitesdcard liblitesata
TRIPLE=riscv64-unknown-elf
CPU=vexriscv
CPUFAMILY=riscv
CPUFLAGS=-march=rv32i2p0       -mabi=ilp32 -D__vexriscv__
CPUENDIANNESS=little
CLANG=0
CPU_DIRECTORY=/home/lucas/Dokumente/litex/litex/litex/soc/cores/cpu/vexriscv
SOC_DIRECTORY=/home/lucas/Dokumente/litex/litex/litex/soc
PICOLIBC_DIRECTORY=/home/lucas/.local/lib/python3.12/site-packages/pythondata_software_picolibc/data
PICOLIBC_FORMAT=integer
COMPILER_RT_DIRECTORY=/home/lucas/.local/lib/python3.12/site-packages/pythondata_software_compiler_rt/data
export BUILDINC_DIRECTORY
BUILDINC_DIRECTORY=/home/lucas/Dokumente/risa-payload-gatemate-risc-v/no/build/colognechip_gatemate_evb/software/include
LIBC_DIRECTORY=/home/lucas/Dokumente/litex/litex/litex/soc/software/libc
LIBCOMPILER_RT_DIRECTORY=/home/lucas/Dokumente/litex/litex/litex/soc/software/libcompiler_rt
LIBBASE_DIRECTORY=/home/lucas/Dokumente/litex/litex/litex/soc/software/libbase
LIBFATFS_DIRECTORY=/home/lucas/Dokumente/litex/litex/litex/soc/software/libfatfs
LIBLITESPI_DIRECTORY=/home/lucas/Dokumente/litex/litex/litex/soc/software/liblitespi
LIBLITEDRAM_DIRECTORY=/home/lucas/Dokumente/litex/litex/litex/soc/software/liblitedram
LIBLITEETH_DIRECTORY=/home/lucas/Dokumente/litex/litex/litex/soc/software/libliteeth
LIBLITESDCARD_DIRECTORY=/home/lucas/Dokumente/litex/litex/litex/soc/software/liblitesdcard
LIBLITESATA_DIRECTORY=/home/lucas/Dokumente/litex/litex/litex/soc/software/liblitesata
BIOS_DIRECTORY=/home/lucas/Dokumente/litex/litex/litex/soc/software/bios
LTO=0
BIOS_CONSOLE_FULL=1