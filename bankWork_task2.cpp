#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

// ===================== КЛАСС BankAccount =====================
class BankAccount {
protected:
    string accountNumber;   // Номер счета
    string ownerName;       // Имя владельца
    double balance;         // Текущий баланс

public:
    // Конструктор с параметрами
    BankAccount(string accNum, string owner, double initialBalance) {
        accountNumber = accNum;
        ownerName = owner;

        if (initialBalance >= 0) {
            balance = initialBalance;
        } else {
            cout << "Ошибка: начальный баланс не может быть отрицательным. Установлено 0." << endl;
            balance = 0;
        }
    }

    // Метод для пополнения счета
    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            cout << "Успешно пополнено: " << fixed << setprecision(2) << amount
                 << " руб." << endl;
        } else {
            cout << "Ошибка: сумма пополнения должна быть положительной!" << endl;
        }
    }

    // Метод для снятия средств
    virtual void withdraw(double amount) {
        if (amount > 0) {
            if (amount <= balance) {
                balance -= amount;
                cout << "Успешно снято: " << fixed << setprecision(2) << amount
                     << " руб." << endl;
            } else {
                cout << "Ошибка: недостаточно средств на счете!" << endl;
                cout << "Запрашиваемая сумма: " << amount
                     << " руб., доступно: " << balance << " руб." << endl;
            }
        } else {
            cout << "Ошибка: сумма снятия должна быть положительной!" << endl;
        }
    }

    // Метод для получения информации о счете
    virtual void displayInfo() const {
        cout << "\n=== Информация о банковском счете ===" << endl;
        cout << "Номер счета: " << accountNumber << endl;
        cout << "Владелец: " << ownerName << endl;
        cout << "Текущий баланс: " << fixed << setprecision(2) << balance << " руб." << endl;
    }

    // Геттер для баланса (может пригодиться)
    double getBalance() const {
        return balance;
    }

    // Виртуальный деструктор для корректного удаления объектов
    virtual ~BankAccount() {
        cout << "Счет " << accountNumber << " закрыт." << endl;
    }
};

// ===================== КЛАСС SavingsAccount =====================
class SavingsAccount : public BankAccount {
private:
    double interestRate;    // Процентная ставка (в процентах)

public:
    // Конструктор с дополнительным параметром - процентной ставкой
    SavingsAccount(string accNum, string owner, double initialBalance, double rate)
        : BankAccount(accNum, owner, initialBalance) {

        if (rate >= 0) {
            interestRate = rate;
        } else {
            cout << "Ошибка: процентная ставка не может быть отрицательной. Установлено 0%." << endl;
            interestRate = 0;
        }
    }

    // Метод для начисления процентов
    void addInterest() {
        double interest = balance * interestRate / 100;
        balance += interest;
        cout << "Начислены проценты по ставке " << interestRate << "%: "
             << fixed << setprecision(2) << interest << " руб." << endl;
    }

    // Переопределяем метод вывода информации
    void displayInfo() const override {
        BankAccount::displayInfo();
        cout << "Процентная ставка: " << fixed << setprecision(1) << interestRate << "%" << endl;
        cout << "Расчетный доход за год: "
             << fixed << setprecision(2) << (balance * interestRate / 100) << " руб." << endl;
    }

    // Переопределяем метод снятия (можно добавить ограничения для сберегательного счета)
    void withdraw(double amount) override {
        cout << "\n--- Операция по сберегательному счету ---" << endl;
        if (amount > 100000) {  // Пример ограничения
            cout << "Внимание: сумма снятия превышает лимит для сберегательного счета!" << endl;
            cout << "Рекомендуется использовать обычный счет для крупных операций." << endl;
        }
        BankAccount::withdraw(amount);
    }
};

// ===================== ОСНОВНАЯ ФУНКЦИЯ =====================
int main() {
    cout << "=== БАНКОВСКАЯ СИСТЕМА ===" << endl;
    cout << "Моделирование работы банковских счетов\n" << endl;

    // ===== 1. Демонстрация работы обычного счета =====
    cout << "\n********** ОБЫЧНЫЙ БАНКОВСКИЙ СЧЕТ **********" << endl;

    // Создание объекта обычного счета
    BankAccount regularAccount("40817810099910004312", "Литичевский Федор Александрович", 50000);

    // Показываем начальную информацию
    regularAccount.displayInfo();

    // Выполняем операции пополнения
    cout << "\n--- Операция пополнения ---" << endl;
    regularAccount.deposit(15000.50);

    // Пытаемся снять средства
    cout << "\n--- Операция снятия ---" << endl;
    regularAccount.withdraw(8000.75);

    // Пытаемся снять больше, чем есть на счете
    cout << "\n--- Попытка снятия превышающей суммы ---" << endl;
    regularAccount.withdraw(70000);

    // Показываем итоговую информацию
    regularAccount.displayInfo();

    // ===== 2. Демонстрация работы сберегательного счета =====
    cout << "\n\n********** СБЕРЕГАТЕЛЬНЫЙ СЧЕТ **********" << endl;

    // Создание объекта сберегательного счета
    SavingsAccount savingsAccount("40817810099910008888",
                                  "Александр Левин",
                                  100000,  // начальный баланс
                                  5.5);    // процентная ставка 5.5%

    // Показываем начальную информацию
    savingsAccount.displayInfo();

    // Начисляем проценты
    cout << "\n--- Начисление процентов ---" << endl;
    savingsAccount.addInterest();

    // Пополняем счет
    cout << "\n--- Операция пополнения ---" << endl;
    savingsAccount.deposit(30000);

    // Снимаем средства (с учетом ограничений сберегательного счета)
    cout << "\n--- Операция снятия ---" << endl;
    savingsAccount.withdraw(15000);

    // Пытаемся снять большую сумму (сработает предупреждение)
    cout << "\n--- Попытка снятия крупной суммы ---" << endl;
    savingsAccount.withdraw(120000);

    // Начисляем проценты еще раз
    cout << "\n--- Начисление процентов ---" << endl;
    savingsAccount.addInterest();

    // Показываем итоговую информацию
    savingsAccount.displayInfo();

    // ===== 3. Дополнительная демонстрация с использованием указателей =====
    cout << "\n\n********** ДОПОЛНИТЕЛЬНАЯ ДЕМОНСТРАЦИЯ **********" << endl;

    // Используем указатель на базовый класс для работы с разными типами счетов
    BankAccount* accounts[2];

    accounts[0] = new BankAccount("40817810099910001111", "Шевцов Федор", 25000);
    accounts[1] = new SavingsAccount("40817810099910002222", "Анисимов Арсений", 75000, 4.2);

    for (int i = 0; i < 2; i++) {
        cout << "\n--- Счет #" << (i + 1) << " ---" << endl;
        accounts[i]->displayInfo();

        // Динамическое приведение типа для вызова специфических методов
        SavingsAccount* savings = dynamic_cast<SavingsAccount*>(accounts[i]);
        if (savings) {
            // Если это SavingsAccount, начисляем проценты
            savings->addInterest();
        }

        // Общие операции
        accounts[i]->deposit(5000);
        accounts[i]->withdraw(3000);
        accounts[i]->displayInfo();
    }

    // Освобождаем память
    for (int i = 0; i < 2; i++) {
        delete accounts[i];
    }

    cout << "\n=== ПРОГРАММА ЗАВЕРШЕНА ===" << endl;
    return 0;
}