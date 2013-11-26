# Документация lib2safe

## Abstract
[2Safe](https://www.2safe.com) – облачное хранилище данных от компании _ROSA Lab_, позволяющее получить доступ к данным везде, где есть Интернет – по сути аналог Dropbox и схожих продуктов.

Библиотека **[lib2safe]** является обёрткой над API сервиса 2Safe, разработанная с применением [Qt Framework](http://ru.wikipedia.org/wiki/Qt).
Среди поддерживаемых вызовов API:
 * Авторизация пользователя;
 * Получение информации об аккаунте;
 * Листинг каталогов;
 * Получение информации о файлах на сервере;
 * Загрузка файлов с сервера;
 * Загрузка файлов на сервер;
 * Настройка публичного доступа для загруженных файлов;
 * ...

Полный список используемых в сервисе вызовов можно получить из [официальной документации API](https://www.2safe.com/api-docs/ru/).
Сигнатуры методов, реализованных в данной библиотеке, а также вспомогательные сигналы (сигнал срабатывает после успешного выполнения метода, иначе говоря является событием) можно изучить по заголовочному файлу [safeapi.h](https://github.com/md5crew/lib2safe/blob/master/safeapi.h).
На данный момент реализовано _**100%**_ покрытие методов API, т.е. любую поддерживаемую сервисом операцию можно осуществить через вызов метода из библиотеки **[lib2safe]**.

[lib2safe]: https://github.com/md5crew/lib2safe

## Внутренний механизм lib2safe

![lib2safe workers](http://f.cl.ly/items/0N122W0f1C0C3D0T3Q0S/Image%202013.11.27%200%3A49%3A05.png)

## SafeAPI

_**SafeApi**_ – по сути master-объект, который инициализируется для конкретного пользователя единожды на протяжении всего runtime программы. Объект хранит состояние сесcии, например токен авторизации. Принцип использования объекта на примере вызова `CALL_GET_DISK_QUOTA`:

```cpp
// Инициализация объекта
auto api = new SafeApi(API_HOST);

// Вешаем обработчик на событие успешной авторизации
connect(api, &SafeApi::authUserComplete, [=](ulong id, const QString& user_id){
    api->getDiskQuota(); // после успешной авторизации вызываем нужный метод
});

// Вешаем обработчик на событие успешного получения информации о DISK_QUOTA
connect(api, &SafeApi::getDiskQuotaComplete,
        [=](ulong id, ulong used, ulong total){
        // Выводим полученную информацию
        // можно было ещё вывести id – номер worker'a, который
        // вызвал это событие (см. далее в документации SafeWorker)
    	qDebug() << "used:" << used << "total: " << total;
});

// Вешаем обработчик на событие ошибки вызова API
connect(api, &SafeApi::errorRaised, [=](ulong id, quint16 code, QString text){
	// Печать ошибки
    qDebug() << "[" << id << "] Error:" << text;
});

// Запускаем цепочку – первым делом авторизуемся
// объект будет хранить и обновлять токен авторизации
// так что если интервал между обращениями к API
// меньше 24 часов (время жизни токена), то явная авторизация
// вообще не нужна. Пароль настоящий, прошу фигнёй не срадать.
api->authUser("md5@kc.vc", "12345678");
```

Файлы:
 * _safeapi.h_ – все сигнатуры методов API и ответов API;
 * _safecalls.h_ – константы для фиксации API и настройки;
 * _safeapi.cpp_ – реализация служебных методов класса SafeApi;
 * *safeapi_?.cpp* – реализация различных категорий методов API.

## SafeWorker

В момент вызова метода у объекта SafeApi, внутри создаётся временной объект типа _**SafeWorker**_. Данный объект является транспортом, то есть бизнес-логика на этот уровень не проникает. Он отвечает за передачу данных на сервер и за получение данных с сервера (или кода ошибки). Обработкой соответствующих данных (или ошибок) занимается уже класс SafeApi.

У объекта SafeWorker есть метод для добавления POST-данных к запросу – `void addParam(QString name, QVariant value)` и метод `run()` для начала обмена данными с сервером (worker начинает делать POST-запрос и ожидать ответ).

В зависимости от ответа сервера у worker срабатывет один из двух сигналов: `void done(SafeWorker *worker, QByteArray data)` или `void error(QString text)`, которые обрабатываются в классе SafeApi, который и создаёт этих рабочих.

SafeWorker был немного расширен для работы с файлами – были добавлены методы `void pushFile()` и `pullFile()`, и сигнал `void progress(ulong bytes, ulong total_bytes)`. Вместо POST-данных, такой рабочий считывает байты с диска, если смог открыть файл. Ну вы поняли.

Файлы:
 * _safeworker.h_
 * _safeworker.cpp_

## WorkersQueue & WorkersPool 

Очевидно, что нельзя просто так запускать обращение к серверу при каждом вызове метода у SafeApi. Потому что при _**1000**_ обращений в секунду у вас упадёт сеть. Следовательно, запросы надо кешировать и не допускать более _**50**_ одновременных обращений к серверу –
`#define MAX_THREADS 50` и не более _**8**_ файлов за раз – `#define MAX_FILE_THREADS 8`.

Объявим два контейнера – конечный пул соединений `QHash<ulong, SafeWorker*> workersPool` и бесконечную очередь `QQueue<SafeWorker*> workersQueue`

Принципиальная последовательность внутренних процессов (в классе SafeApi):
 1. `SafeWorker *createWorker(QString cmd)`;
 2. `void routeWorker(SafeWorker *worker)` – в зависимости от наличия свободных мест в `workersPool` суёт `*worker` либо в `workersPool`, либо в `workersQueue`;
 3. Если в `workersPool`, то заодно и активирует рабочего: `worker->run()`;
 4. Как только какой-то worker отработал и его события обработались, он удаляется из `workersPool` и вызывается метод `void processWorkersQueue()`, который перемещает крайнего рабочего из `workersQueue` в `workersPool` (и запускает его).

Это весь фокус. Файловые обработчики имеют отдельные очереди и методы, просто они называются иначе, например `void processFileWorkersQueue()`.

## Типы

Для удобства работы с информацией, полученной от сервера в формате JSON, были сделаны дополнительные типы для абстракции: _**SafeDir**_, _**SafeFile**_. Оба передстваляют собой ифнормацию о файле или о директории. Можно обращаться к полям этих структур через точку, что гораздо удобнее навигации по JSON-дереву. Однако, другие объекты (например, информация о версии) пока не удалось инкапсулировать подобными структурами – они никак не задекларированы в документации и будет удобнее обращаться с ними как с JSON-документами. Отдельного внимания заслуживает тип _**SafeCaptcha**_:

```cpp
struct SafeCaptcha {
    QByteArray picture;
    QString id;
};
```

Файлы:
 * _safetypes.h_

## Лицензия

```
The MIT License (MIT)

Copyright (c) 2013 Maxim Kouprianov <maxim@kouprianov.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
```
