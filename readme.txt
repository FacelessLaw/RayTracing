Запуск

из /dir
cmake ..
make
./raytracer -w x
-w - необязательный флаг, по умолчанию x = 512

Комментарии к выполнению:
1.База
    1.1 Стекло как стекло. ok
    1.2. Вода. Вода никогда не меняется. Внутри пирамиды находится шар, в который вшита текстурка воды.
        Также у него еть небольшие преломления. Да, переход в итоге достаточно резкий, да и вообще
        видно крайне плохо, хотя прозрачность там есть. Зато это честная работа! Я правда пытался сделать по красоте через 
        displacement, как в статье про взрыв, но не получилось вставить, не ломая всё остальное. ok?
    1.3. Подставка банальная, но есть. Вместо скучно посаженной пирамиды на цилиндр, 
        пытался сделать некий пятый элемент с асимметрией в картинке, чтобы не скучно было. ok
    1.4. Источников света много.
    1.5. Рендеринг быстрый, резкий. ok
    1.6. ok
    1.7. В папке /build ok
2. Допки. Тут тени и текстуры панормаиные с фильтрацией. С удовольствием дополнил бы анти-алиасингом,
   но, увы, тогда бы видел результат выполнения раз в сутки.

P.S. Пожалуйста, скажите, что в этом году будет аукцион невиданной 
     щедрости со степиком - мне ещё курсовую писать(

С уважением,
Волков Андрей, 312.
