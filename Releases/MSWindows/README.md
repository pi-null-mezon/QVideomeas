Здесь хранятся актуальные релизы проектных приложений для ОС семейства MS Windows

Videomeasurements
===
Приложение для бесконтактного измерения частоты сердечных сокращений и температуры лба.
Разрабатывалось в качестве замены устаревшего приложения QPULSECAPTURE. Поэтому исполняемый файл
унаследовал наименование QPULSECAPTURE, а его запуск из командной строки полностью совместим со старой
версией.

Минимальным требованием для работы приложения является наличие в системе веб-камеры Logitech B910
(работает и с другими, но нужно учитывать что указанная камера подбиралась специально с относительно
узким углом поля зрения). Опционально поддерживает измерения температуры лба, при установке в системе
датчика MELEXIS mlx916DCI через мост Arduino Nano v3.0 c USB-UART преобразователем на базе микросхемы CH30G.

Замечания по веб-камере Logitech B910
===
По умолчанию, без специализированного драйвера, веб-камера Logitech B910 устанавливается в системе как USB-видеоустройство.
При этом, с работой камеры в приложении не должно возникать никаких проблем. Тем не менее, установка специализированного[драйвера Logitech B910](http://support.logitech.com/en_us/product/b910-hd-webcam#download)    
открывает дополнительную опцию автоматической корректировки положения видоискателя, что может быть удобно в том случае,
если положение головы чеолвека может сильно меняться (например измерения проводятся для людей разного роста и на различном удалениии от объектива).
Для того, чтобы задействовать эту опцию, после установки указанного драйвера, откройте в приложении меню Settings >> Settings (Настройки >> Настройки).
В появившемся диалоговом окне "Свойства веб-камеры Logitech" найдите опцию "Следовать за моим лицом" и установите её в положение "включено".  