# code_generator
Uma ferramenta para transformar código fonte gerado pelo Busmaster em código executável para plataforma arduino usando trampolineRTOS

## Compilação
### Para compilar os testes unitários utilizando o qmake no windows com kit Qt com mingw:
> $ qmake.exe code_generator.pro -spec win32-g++ "CONFIG+=test" && mingw32-make.exe qmake_all

### Para compilar no modo debug utilizando o qmake no windows com kit Qt com mingw:
> $ qmake.exe code_generator.pro -spec win32-g++ "CONFIG+=debug" "CONFIG+=qml_debug" && mingw32-make.exe qmake_all

### Para compilar no modo release utilizando o qmake no windows com kit Qt com mingw:
> $ qmake.exe code_generator.pro -spec win32-g++ && mingw32-make.exe qmake_all

## Execução
### Para executar os testes unitários no windows:
> $ test.exe

### Para executar o programa com interface gráfica no windows:
> $ code_generator.exe

### Para executar o programa sem interface gráfica no windows:
> $ Usage: code_generator.exe [options] cpp_source_file def_file dbf_file trampoline_root destination_prefix goil_exe out_dir
