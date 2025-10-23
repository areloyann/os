System V Message Queues — Структура проектов
===========================================

Общий хедер:
- common/include/init_queue.hpp

Папки задач:
- 01_create/                  — создание/открытие очереди по ключу (ftok)
- 02_send/                     — отправка сообщений
- 03_receive/
    ├─ v1_blocking/           — блочный приём и «осушение» очереди
    ├─ v2_nowait/             — неблокирующий приём (IPC_NOWAIT)
    └─ v3_noerror/            — приём с MSG_NOERROR и маленьким буфером
- 04_new_queue/               — создание новой очереди (IPC_PRIVATE)
- 05_remove/                  — удаление очереди
- 06_limits/                  — изменение msg_qbytes и тест отправки

Сборка (g++/Linux, C++17):
---------------------------
Пример для каждого бинарника (ключевые флаги одинаковые):

g++ -std=gnu++17 -O2 -I common/include 01_create/message_create_v1.cpp -o build/message_create_v1
g++ -std=gnu++17 -O2 -I common/include 02_send/message_send.cpp -o build/message_send
g++ -std=gnu++17 -O2 -I common/include 03_receive/v1_blocking/message_receive_v1.cpp -o build/message_receive_v1
g++ -std=gnu++17 -O2 -I common/include 03_receive/v2_nowait/message_receive_v2_nowait.cpp -o build/message_receive_v2_nowait
g++ -std=gnu++17 -O2 -I common/include 03_receive/v3_noerror/message_receive_v3_noerror.cpp -o build/message_receive_v3_noerror
g++ -std=gnu++17 -O2 -I common/include 04_new_queue/message_create_v2_new.cpp -o build/message_create_v2_new
g++ -std=gnu++17 -O2 -I common/include 05_remove/message_rm.cpp -o build/message_rm
g++ -std=gnu++17 -O2 -I common/include 06_limits/message_chqbytes.cpp -o build/message_chqbytes
g++ -std=gnu++17 -O2 -I common/include 06_limits/message_send_test_message.cpp -o build/message_send_test_message

Примечания:
- Перед использованием ftok создаётся ./queue.key (см. common/include/init_queue.hpp).
- Убедись, что собираешь под Linux/WSL. Команда `ipcs -q` покажет очереди, `ipcrm -q <id>` удалит.
- Для 06_limits может потребоваться повышенное право/настройки ядра для изменения msg_qbytes.
