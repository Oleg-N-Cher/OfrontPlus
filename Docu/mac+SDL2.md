# Работа с Ofront+ под macOS и прикручивание SDL2

### Andrew G. Semenov <andrew.g.semenov _at_ yandex.ru>

## Предварительные замечания

Все манипуляции делались на моем рабочем ноубуке Macbook Pro 2018 года, на последней версии MacOs (Sonoma 14.5). Для работы следует установить инструменты разработчика для командной строки
```
xcode-select --install
```
Вроде этого должно быть достаточно. 

## Установка Ofront+

Для установки офронта следуем рекомендациям автора.

1. Скачиваем OfrontPlus и OfrontPlus-bootstrap
   ```
   git clone https://github.com/Oleg-N-Cher/OfrontPlus.git
   git clone https://github.com/Oleg-N-Cher/OfrontPlus-bootstrap.git
   ```
2. Из папочки OfrontPlus-bootstrap/unix-88/ все содержимое копируете в  OfrontPlus/Target/macOS/Obj/ и в OfrontPlus/Target/macOS/Lib/Obj/
   ```
   cp OfrontPlus-bootstrap/unix-88/* OfrontPlus/Target/macOS/Obj
   cp OfrontPlus-bootstrap/unix-88/* OfrontPlus/Target/macOS/Lib/Obj
   ```
3. Запускаем /Bin/cc и /Lib/Bin/cc в следующей последовательности 
   ```
   ./OfrontPlus/Target/macOS/Lib/Bin/cc
   ./OfrontPlus/Target/macOS/Bin/cc
   ./OfrontPlus/Target/macOS/Lib/Bin/build
   ./OfrontPlus/Target/macOS/Bin/build
   ```

Ofront+ должен быть собран и работать. Можно проверить на чем-то простом типа Helloworld.

## Прикручивание SDL2

Основное отличие в macOS от юниксовых систем - это использование фреймворков вместо библиотек. Это то же самое, но подключать надо чуть по иному. Для начала установим SDL2, SDL2_image и SDL2_ttf. Для этого скачаем их отсюда:
1. SDL2: https://github.com/libsdl-org/SDL
2. SDL2_image: https://github.com/libsdl-org/SDL_image
3. SDL2_ttf: https://github.com/libsdl-org/SDL_ttf
   
При скачивании выбираем в релизе файлы __*.dmg__ . Внутри этих файлов есть папки __*.framework__ . Их надо скопировать в системную папку /Library/Frameworks/  На самом деле не обязательно туда, но при стандартной разработке под мак вроде так делают.

## Сборка с использованием SDL2

Для сборки файлов я написал следующий скрипт, которому на входе дается файл __*.cp__ а на выходе получается исполняемый файл.
```
#!/bin/bash

if [ "$#" -ne 1 ]
then
  echo "No input filename"
  exit 1
fi

if ! test -f "$1"
then
  echo "$1 not a file"
  exit 1
fi

cd "$(dirname $1)"
PATH="~/OfrontPlus/Target/macOS:$PATH"
export OBERON=.:~/OfrontPlus/Target/macOS/Obj:~/OfrontPlus/Target/macOS/Lib/Sym:~/OfrontPlus/Target/macOS/Sym
MYFILE="$(basename $1)"
CFILE="${MYFILE%.*}.c"
EXEFILE="${MYFILE%.*}"

#echo $OBERON
echo Input file: $MYFILE
echo ---

ofront+ -mC -88 $1

if [ $? != 0 ]; then exit 1; fi

LIB="-I/Users/andrewsemenov/OfrontPlus/Target/macOS/Lib/Obj -I/Users/andrewsemenov/OfrontPlus/Mod/Lib -L/Users/andrewsemenov/OfrontPlus/Target/macOS/Lib -lOfront"
CC="cc -m64 -Os -g0 -fvisibility=hidden -fomit-frame-pointer -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables"

$CC $CFILE  -o $EXEFILE $LIB -I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include -rpath /Library/Frameworks -F/Library/Frameworks -framework SDL2 -framework SDL2_image
if [ $? != 0 ]; then exit 1; fi

echo ---
echo C-file generated: $CFILE
echo Executable generated: $EXEFILE
```

Не скажу что я понимаю все ключи до компилятора конца, но сделаю ряд комментариев. То, что относится к Ofront+ никак не изменено. Единственное - указаны соответствующие директории к обероновским библиотекам в Target/macOS . А вот при компиляции clang-ом надо обязательно указать следующие ключи:
1. ___-I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include___  - Здесь находятся стандартные сишные библиотеки.
2. ___-F/Library/Frameworks___ - Здесь находятся фреймворки, используемые при компиляции/сборке.
3. ___-rpath /Library/Frameworks___ - Здесь программа будет искать библиотеку/фреймворк когда будет она запускаться будучи уже скомпилированной. Дело в том, что приложения под маком распространяются в виде файла __*.app__ , который на самом деле является папкой со специальной структурой, где лежат все ресурсы и библиотеки. Ключ и нужен для того,чтобы знать куда программе смотреть при запуске.
4. ___-framework SDL2___ - Ключ говорит компилятору, что при сборке надо использовать соответствующий фреймворк.