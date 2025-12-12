savedcmd_ex2.mod := printf '%s\n'   ex2.o | awk '!x[$$0]++ { print("./"$$0) }' > ex2.mod
