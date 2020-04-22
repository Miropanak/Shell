Author projektu: Miroslav Pavlák

Manuál na obsluhu:
./shell     spustenie shellu v normálnom móde, t.j. po spustení je možné zadávať príkazy na STDIN a výstupy sú zobrazené na STDOUT
./shell -h  zobrazí manuál k programu a spustí sa shell v normálnom móde
./shell -c -help
./shell -c -halt
./shell -u <socket_path>
./shell -c -u <socket_path>
./shell -p <port_number>
./shell -i <IP_address> -p <port_number>
./shell -c -i <IP_address> -p <port_number>

Interné príkazy:
help    zobrazenie manuálu k programu
quit    klient ukončí spojenie so serverom, na ktorom bol pripojený
halt    ukončenie programu

Interpretácia špeciálnych znakov:
 #  
 ;
 <
 >
 |
 \

Prednastavený prompt:

Splnené bonusové úlohy:
9. (3 body) S prepínačom "-c" v kombinácii s "-i", resp. "-u" sa bude program správať ako klient, teda pripojí sa na daný socket
a bude do neho posielať svoj štandardný vstup a zobrazovať prichádzajúci obsah na výstup.

Po zadaní prepínača "-c" pri spúšťaní shellu sa nastaví mód shellu na klienta. Ak je zadaný prepínač "-u" a názov socketu tak sa
shell pripojí nakonkrétny unix socket, cez ktorý naň posiela štandardný vstup a spúšťa príkazy. Výstup nie je zobrazovaný na STDOUT
klienta, ale iba servera.Ak je spolu s prepínačom "-c" zadaný prepínač "-i" a "-p", klient si môže zvoliť konkrétnu IP adresu a port
na, ktorej počúva druhý shell. Ak je spolu s prepínačom "-c" zadaný iba prepínač "-p", tak sa klient pripojí na predvolenú IP adresu
127.0.0.1.

11. (2 body) S prepínačom "-i" bude možné zadať aj IP adresu na ktorej bude program očakávať spojenia (nielen port).

Prepínač "-i" je možné použiť v prípade klientského shellu, ktorý sa bude pripájať na druhý shell, ale aj v prípade shellu, ktorý bude
očakávať pripojenie

12.	(2 body) Jeden z príkazov bude využívať funkcie implementované v samostatnej knižnici, ktorá bude "prilinkovaná" k hlavnému programu.

Funkcia na vypísanie manuálu použitia sa nachádza v externom súbore builtin.c, ktorý je prilinkovaný k hlavnému programu. 

//13.	(5 bodov) Ak je niektoré spojenie nečinné zadanú dobu, bude zrušené.

14.	(2 body) Príkazy musia byť rozoznané aj ako argumenty na príkazovom riadku v kombinácii s prepínačom "-c" (interné príkazy ako prepínače,
-halt, -help), vykonajú sa jednorazovo a program sa ukončí.

Po zadaní príkazu:  "./shell -c -help" sa vypíše manuál použitia a program sa ukončí.
                    "./shell -c -halt" sa program spustí a automaticky sa hneď vzpne.

//15. (4 body) Program s prepínačom "-d" sa bude správať ako démon (neobsadí terminál), nebude používať štandardný vstup a výstup.

16. (2 body) Poriadny Makefile.

Vytvoril som Makefile, ktorý som používal počas implementovania úlohy, až vtedy som si uvedomil aká je to neskutočne užitočná vec. Avšak neviem
posúdiť či sa môže počítať ako poriadny, lebo pri zoznamovaní sa s tvorbou Makefilov som natrafil na internete na neskutočne divoké Makefily.

17.	(1 bod) Dobré komentáre, resp. dokumentácia, v anglickom jazyku.
