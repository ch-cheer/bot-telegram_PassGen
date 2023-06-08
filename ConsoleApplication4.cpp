#include <random>
#include <regex>
#include "tgbot/tgbot.h"

using namespace std;
using namespace TgBot;

// Функция, которая генерирует случайный набор символов без специальных
char getRandomChar() {
    static const char charset[] =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    static const uint32_t max_index = (sizeof(charset) - 1);
    static random_device rd;
    static mt19937 rng(rd());
    static uniform_int_distribution<uint32_t> dist(0, max_index);

    return charset[dist(rng)];
}

//Функция, которая генерирует случайный набор символов, включая спецальные
char getRandomSpecChar() {
    static const char charset[] =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "!@#$%^&*()_+{}[]|:;'<>,.?/";

    static const uint32_t max_index = (sizeof(charset) - 1);
    static random_device rd;
    static mt19937 rng(rd());
    static uniform_int_distribution<uint32_t> dist(0, max_index);

    return charset[dist(rng)];
}

// Функция, которая генерирует пароль
string generatePassword(int length, bool includeSpecialChars) {
    string password = u8"Сгенерированный пароль: ";

    // Проверка длины пароля, если больше 1000, вывести ошибку 
    if (length <= 1000) {
        // Проверка наличия флага для использования спец символов
        if (includeSpecialChars == false) {
            for (int i = 0; i < length; i++) {
                password += getRandomChar();
            }
        }
        else {
            for (int i = 0; i < length; i++) {
                password += getRandomSpecChar();
            }
        }
    }
    else {
        password = u8"Ошибка, введите длину меньше, либо равную 1000 символов";
    }
    return password;
}

int main() {
    // Задаем токен для нашего бота
    Bot bot("token");

    // Обработчк для команды "/start"
    bot.getEvents().onCommand("start", [&bot](Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, u8"Здравствуте, этот бот был создан для генерации паролей, чтобы вы смогли использовать различные сервисы, не боясь , что ваши данные смогут украсть из-за слабого пароля.");
        bot.getApi().sendMessage(message->chat->id, u8"Чтобы начать, введите команду '/gen  {длина пароля} {специальные символы (true(использовать) / false(не использовать)}'.");
        bot.getApi().sendMessage(message->chat->id, u8"Для вызова справки введите команду '/help'.");
        });

    // Обработчик для команды "/help"
    bot.getEvents().onCommand("help", [&bot](Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, u8"Это бот для генерации паролей.");
        bot.getApi().sendMessage(message->chat->id, u8"Чтобы сгенерировать пароль, отправьте сообщение с текстом в формате:\n'/gen {длина пароля} {специальные символы (true(использовать) / false(не использовать)}'.");
        bot.getApi().sendMessage(message->chat->id, u8"Например, '/gen 10 true' сгенерирует пароль длиной 10 символов, включая специальные символы.");
        bot.getApi().sendMessage(message->chat->id, u8"В случаи, если длина пароля не указана, будет использовано стандартное количество символов - 15\nА в случаи, если не указано использование специальных символов, они не будут использоваться");
        });

    // Обработчик для команды "/gen"
    bot.getEvents().onCommand("gen", [&bot](Message::Ptr message) {
        // Разбираем сообщение на аргументы.
        vector<string> args;
        regex pattern("[^\\s]+");
        copy(sregex_token_iterator(message->text.begin(), message->text.end(), pattern), sregex_token_iterator(), back_inserter(args));

        // Получаем длину пароля, указанную пользователем.
        int passwordLength = 15;
        if (args.size() > 1) {
            string input = args[1];
            bool isNumber = true;
            for (char c : input) {
                if (!isdigit(c)) {
                    isNumber = false;
                    break;
                }
            }
            if (isNumber) {
                passwordLength = stoi(input);
            }
        }

        // Получаем значение второго параметра - использовать ли специальные символы.
        bool includeSpecialChars = false;
        if (args.size() > 2) {
            includeSpecialChars = (args[2] == "true");
        }

        // Генерируем пароль и отправляем его пользователю.
        string password = generatePassword(passwordLength, includeSpecialChars);
        bot.getApi().sendMessage(message->chat->id, password);
        });

    // Запускаем бота и начинаем получать сообщения от пользователей
    bot.getApi().deleteWebhook();
    TgLongPoll longPoll(bot);
    while (true) {
        try {
            longPoll.start();
        }
        catch (exception& e) {
            printf("Error: %s\n", e.what());
        }
    }

    return 0;
}