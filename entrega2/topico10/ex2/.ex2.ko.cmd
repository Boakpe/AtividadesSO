savedcmd_ex2.ko := ld -r -m elf_x86_64 -z noexecstack --no-warn-rwx-segments --build-id=sha1  -T /usr/src/kernels/6.17.9-200.fc42.x86_64/scripts/module.lds -o ex2.ko ex2.o ex2.mod.o .module-common.o
