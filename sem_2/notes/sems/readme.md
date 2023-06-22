# О чем семинары?

## Сем 1:
Сокеты. Типы сокетов: AF_UNIX/AF_INET, DGRAM/STREAM. sys_socketcall(). sys_socket(). struct socket. State - free, connected, listen. struct sockaddr.

## Сем 2:
### Часть 1
Продолжение сокетов. Сетевой стек. bind(), listen(), connect(), accept(). Select, poll.

—

### Часть 2
VFS proc, файлы и директории внутри /proc. 

## Сем3:
### Часть 1
Пример вывода переменных среды с помощью proc.

—

### Часть 2
Загружаемые модули ядра. Простой пример. Пример-итерация по task_struct. Уровни протоколирования printk. 

## Сем 4:
Пример взаимодействия нескольких загружаемых модулей. __local, extern…
Copy_to_user, copy_from_user, fortunes. - они в методичке, тут просто упомянаются
struct proc_dir_entry. proc_create().

## Сем 5:
proc_ops, file_ops. Пример создания файлов в proc, директории- он же пример фортунки.  Sequence-файлы, seq operations. 

## Сем 6:
Взаимосвязь функций для работы с Sequence-файлами - show(), stop(), next(). Примеры на single_open(), single file.

## Сем 7:
open(). fopen(). 2 варианта open(). O_CREAT | O_EXECL. SYSCALL_DEFINE3. do_sys_open(), do_filp_open(), path_openat().

## Сем 8:
O_PATH. Примеры проблем буферизации.
