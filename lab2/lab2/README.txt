Лабораторная работа 2 — System V Message Queues (msgget, msgsnd, msgrcv)
=======================================================================

Все программы получают параметры через командную строку, как указано в методичке.

Структура:
- common/include/init_queue.hpp         — общие утилиты (ftok, строки ошибок)
- 01_tools/
    message_create.cpp                  — реализация опций: -c, -x, -f, -k, -p, [octal-perms]
    message_send.cpp                    — реализация: [-n] msqid msg-type [msg-text]
    message_receive.cpp                 — реализация: [-e] [-n] [-t type] [-x] msqid [max-bytes]
- 02_client_server/
    msg_file.hpp                        — структуры requestMsg/responseMsg и константы SERVER_KEY
    msg_file_server.cpp                 — конкурентный сервер (fork) — читает запросы из Server MQ и шлёт данные в Client MQ
    msg_file_client.cpp                 — клиент — создаёт Client MQ, шлёт запрос, получает файл

Сборка (Linux/WSL, C++17):
--------------------------
mkdir -p build

g++ -std=gnu++17 -O2 -I common/include 01_tools/message_create.cpp -o build/message_create
g++ -std=gnu++17 -O2 -I common/include 01_tools/message_send.cpp    -o build/message_send
g++ -std=gnu++17 -O2 -I common/include 01_tools/message_receive.cpp -o build/message_receive

g++ -std=gnu++17 -O2 -I common/include 02_client_server/msg_file_server.cpp -o build/msg_file_server
g++ -std=gnu++17 -O2 -I common/include 02_client_server/msg_file_client.cpp -o build/msg_file_client

Примеры запуска (соответствуют методичке):
-----------------------------------------
1) Создать очередь с IPC_PRIVATE:
   ./build/message_create -c -p 700

2) Создать очередь с ключом -k:
   ./build/message_create -c -k 1234 666

   Разница -k (явный числовой ключ) и -p (IPC_PRIVATE): -k создаёт/открывает
   переиспользуемую очередь с фиксированным key; -p всегда создаёт уникальную
   приватную очередь, её id нужно сообщать явно.

3) Отправить 3 сообщения в выбранную очередь (пример msqid=0):
   ./build/message_send 0 10 "message 1"
   ./build/message_send 0 20 "message 2"
   ./build/message_send 0 25 "message 3"

4) Прочитать mtype=20:
   ./build/message_receive -t 20 0 1024

5) Дочитать всё и проверить неблокирующий режим:
   ./build/message_receive 0 4096     # блокирующе читает следующее
   ./build/message_receive -n 0 4096  # не блокирует — вернёт ENOMSG

6) Клиент/Сервер:
   # в одной консоли
   ./build/msg_file_server
   # в другой консоли
   ./build/msg_file_client /etc/hosts

   Сервер создаёт Server MQ по известному ключу (ftok("server.key",'S')) и на каждый
   запрос делает fork(), чтобы обслуживать конкурирующие запросы параллельно.

Замечания:
- Работает на Linux/WSL. На Windows (без WSL) System V IPC недоступен.
- Для просмотра очередей: ipcs -q; для удаления: ipcrm -q <id>.
- Пустые каталоги в git не сохраняются — добавляйте .gitkeep при необходимости.
