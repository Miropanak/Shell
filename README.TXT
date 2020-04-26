Author projektu: Miroslav Pavlák

Myslím si, že sa mi podarilo spniť všetky požiadavky na zadanie. Program rozpoznáva všetky špeciálne znaky: # ; < > | \. Príkazy
je možné zadať aj cez štandardný vstup aj zo spojení unixovým či internetovým socketom. Vykonávanie príkazov cez sokety som riešil
dvomi rozličnými spôsobmi. Ak je shell v móde 3 alebo 4, čiže čaká na prichádzajúce spojenie. Následne vykonáva príkazy, ktoré sú
poskytnuté shellom klienta prostredníctvom socketu a výstup z príkazov je zobrazený na štandardnom výstupe unix servera. Keď klient
zadá príkaz "quit" ukončí spojenie klient shell sa vypne a z shell servera sa prepne do módu 0. Ale ak je shell v móde 5 alebo 6,
čiže sa správa ako démon a čaká na prichádzajúce spojenie. Následne vykonáva príkazy, ktoré sú poskytnuté klientom prostredníctvom
unixového/internetového socketu a výstup z príkazov je presmerovaný do socketu a následne zobrazený na štandardnom výstupe shellu
klienta.

Módy shellu:                Spustenie Shellu:                                  Pripojenie na druhý shell:
mode 0 normal mode          ./shell                                                     x
mode 1 unix client          ./shell -c -u <socket_path>                                 x
mode 2 inet client          ./shell -c -i <IP_address> -p <port_number>                 x
mode 3 unix server          ./shell -u <socket_path>                           ./shell -c -u <socket_path> 
mode 4 inet server          ./shell -i <IP_address> -p <port_number>           ./shell -c -i <IP_address> -p <port_number>
mode 5 unix server deamon   ./shell -d -u <socket_path>                        ./shell -c -u <socket_path> 
mode 6 inet server deamon   ./shell -d -i <IP_address> -p <port_number>        ./shell -c -i <IP_address> -p <port_number>

Shel podporuje reťazenie viacerých príkazov typu: "ls -al | head -5 | grep drw" a taktiež presmerovanie výstupu zo zreťazených
príkazov ako napr. "ls -al | grep -rwx | head -3 > out.txt". Shell taktiež podporuje presmerovanie vstupu "grep pass < out.txt".
Na vytvorenie promptu som v inline assembleri použil systémové volania: gettimeofday 116, getuid 24 a SYS___sysctl 202. Na
spúšťanie príkazov a presmerovanie vstupu a výstupu som použil systémové volania fork, execvp, wait, waitpid, pipe, dup a dup2.
Shell som implementoval na stroji student.fiit.stuba.sk, takže je kompatibilný s OS FreeBSD 5.2.1..

Zhodnotenie:
Na začiatku som si hovoril načo mi je Makefile a gdb, ale keď som sa s nimi zoznámil uvedomil som si, že sú veľmi užitočné. Preto som
si vytvoril vlastný Makefile, ktorý mi výrazne pomohol pri preklade a linkovaní môjho programu. Taktiež som využil aj gdb,ktorý mi
nejeden krát pomohol nájsť chybu v programe. Plus ma naozaj milo prekvapilo, čo všetko umožňuje makefile a gdb.




Manuál na obsluhu:
./shell - spustenie shellu v normálnom móde, t.j. po spustení je možné zadávať príkazy na STDIN a výstupy sú zobrazené na STDOUT

./shell -h - zobrazí manuál k programu a spustí sa shell v normálnom móde

./shell -c -help | more - vypíše manuál použitia a program sa ukončí

./shell -c -halt - program sa spustí a automaticky sa hneď vypne

./shell -u <socket_path> - prepnutie shellu do módu 3, shell čaká na spojenie na unixovom sockete

./shell -c -u <socket_path> - prepnutie shellu do módu 1, shell sa pripojí na konkrétny unixový socket, na ktorom počúva druhý shell

./shell -d -u <socket_path> - prepnutie shellu do módu 5, shell sa správa ako démon a čaká na spojenie na unixovom sockete

./shell -d -i <IP_address> -p <port_number> - prepnutie shellu do módu 5, shell sa správa ako démon a čaká na spojenie na internetovom sockete

./shell -i <IP_address> -p <port_number> - prepnutie shellu do módu 4, shell čaká na spojenie na internetovom sockete

./shell -c -i <IP_address> -p <port_number> - prepnutie shellu do módu 2, shell sa pripojí na konkrétny internetový socket, na ktorom počúva druhý shell


Interné príkazy:
help    zobrazenie manuálu k programu
quit    klient ukončí spojenie so serverom, na ktorom bol pripojený
halt    ukončenie programu

Splnené bonusové úlohy:
9. (3 body) S prepínačom "-c" v kombinácii s "-i", resp. "-u" sa bude program správať ako klient, teda pripojí sa na daný socket
a bude do neho posielať svoj štandardný vstup a zobrazovať prichádzajúci obsah na výstup.

Po zadaní prepínača "-c" pri spúšťaní shellu sa nastaví mód shellu na klienta. Ak je zadaný prepínač "-u" a názov socketu tak sa
shell pripojí nakonkrétny unix socket, cez ktorý naň posiela štandardný vstup a spúšťa príkazy. Ak sa server správa ako démon,
(je v móde 5 alebo 6) tak výstup je zobrazovaný na štandardnom výstupe klieta, ale ak je server v móde 3 alebo 4 tak výstup je
zobrazený na štandardnom výstupe servera. Ak je spolu s prepínačom "-c" zadaný prepínač "-i" a "-p", klient si môže zvoliť konkrétnu
IP adresu a port na, ktorej počúva druhý shell. Ak je spolu s prepínačom "-c" zadaný iba prepínač "-p", tak sa klient pripojí na
predvolenú IP adresu 127.0.0.1.

11. (2 body) S prepínačom "-i" bude možné zadať aj IP adresu na ktorej bude program očakávať spojenia (nielen port).

Prepínač "-i" je možné použiť v prípade klientského shellu, ktorý sa bude pripájať na druhý shell, ale aj v prípade shellu, ktorý bude
očakávať pripojenie na konkrétnej adrese.

12.	(2 body) Jeden z príkazov bude využívať funkcie implementované v samostatnej knižnici, ktorá bude "prilinkovaná" k hlavnému programu.

Funkcia na vypísanie manuálu použitia sa nachádza v externej knižnici builtin.c, ktorá je prilinkovaná k hlavnému programu. 

14.	(2 body) Príkazy musia byť rozoznané aj ako argumenty na príkazovom riadku v kombinácii s prepínačom "-c" (interné príkazy ako prepínače,
-halt, -help), vykonajú sa jednorazovo a program sa ukončí.

Po zadaní príkazu:  "./shell -c -help" vypíše manuál použitia a program sa ukončí.
                    "./shell -c -halt" program sa spustí a automaticky sa ukončí.
                    "./shell -c -quit" vypíše, že neexistuje spojenie na ukončenie a program sa ukončí.

15. (4 body) Program s prepínačom "-d" sa bude správať ako démon (neobsadí terminál), nebude používať štandardný vstup a výstup.

Program po zadaní príkazu "./shell -d -u <socket_path>" sa spustí a správa sa ako démon. Na pozadí čaká na pripojenie klienta a následne
vykoná príkazy, ktoré klient zadá na štandardný vstup. Výstup je presmerovaný cez unixový socket priamo ku klientovi. Keď klient chce ukončiť
spojenie, zadá do terminálu príkaz "quit", čím ukončí spojenie. Po ukončení spojenia sa ukončí program klienta a aj démon. Prepínač "-d"
sa dá použiť aj v sojení s internetovým socketom nasledovne "./shell -d -i <IP_address> -p <port_number>". Démon sa bude správať úplne rovnako,
ako v predchádzajúcom prípade s tým rozdielom, že komunikácia bude prebiehať cez internetový socket.

16. (2 body) Poriadny Makefile.

Vytvoril som Makefile, ktorý som používal počas implementovania úlohy, až vtedy som si uvedomil aká je to neskutočne užitočná vec. Avšak neviem
posúdiť či sa môže počítať ako poriadny, lebo pri zoznamovaní sa s tvorbou Makefilov som natrafil na internete na neskutočne divoké Makefily.

17.	(1 bod) Dobré komentáre, resp. dokumentácia, v anglickom jazyku.

Kód som okomentoval a man page k programu som napísal v angličtine a myslím si, že je celkom intuitívne napísaný a čitateľovi by
malo byť jasné, na čo program slúží a ako ho použiť.