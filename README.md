
# Trabajo Práctico N°2 de Sistemas Operativos

En este trabajo se creará un Kernel propio simple. Para ello, se implementará Memory Management, procesos, scheduling, mecanismos de IPC y sincronización.


## Dependencias

* **C Compiler:** [GCC](https://gcc.gnu.org/)
* **Static Code Analysis Tools:** [Cppcheck](http://cppcheck.sourceforge.net/) y [PVS-Studio](https://pvs-studio.com/en/pvs-studio/)
* **Formatter:** [Clang-format](https://clang.llvm.org/docs/ClangFormat.html)

## Compilación y ejecución

Para compilar y ejecutar, se debe estar ubicado en la carpeta `src/kernel` del mismo repositorio, y ejecutar los siguientes comandos:

```
$> make clean
$> make all MM=MS
```
y luego correr el siguiente comando:
```
sudo qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 -d int -audiodev coreaudio,id=audio0 -machine pcspk-audiodev=audio0
```


## Dependencias

* **C Compiler:** [GCC](https://gcc.gnu.org/)
* **Static Code Analysis Tools:** [Cppcheck](http://cppcheck.sourceforge.net/) y [PVS-Studio](https://pvs-studio.com/en/pvs-studio/)
* **Formatter:** [Clang-format](https://clang.llvm.org/docs/ClangFormat.html)

Puede ejecutar el comando `sudo ./setup.sh` para instalar todas las dependencias necesarias.

## Authors

- [@mquesada02](https://www.github.com/mquesada02)
- [@catamuller](https://www.github.com/catamuller)
