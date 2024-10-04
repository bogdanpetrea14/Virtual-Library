### Petrea Bogdan-Vasile 321CA

## Tema 4, Protocoale de Comunicatii

> Precizari
- am folosit functiile din parson pentru partea de prelucrare a JSON-urilor
- tema a fost construita pe scheletul de la laboratorul 9, luand de acolo majoritatea implementarilor si functiilor

--- 

> Flow-ul programului
- la deschiderea acestuia, clientul are doua variante, fie sa se logheze (daca are un user si o parola care au fost inregistrare in prealabil), fie sa se inregistreze;
- fara un register inainte, programul nu va functiona, deoarece clientul nu va avea acces la librarie
- daca nu e nevoie de inregistrare, clientul se poate loga cu userul si parola preinregistrate, urmand sa acceseze biblioteca pentru a putea executa operatiuni pe aceasata
- el poate sa vada toate cartile, sa extraga o carte dupa ID sau sa stearga o carte din librarie
- in final, se poate deloga din contul respectiv, putand dupa, fie sa se logheze cu alt cont, fie sa incheie programul prin comanda *exit*

> Logica efectiva
- fisierele principale sunt **client.c** si **comands.c**
- cliet.c se ocupa doar de prelucrarea comenzilor primite de la STDIN, apelarea functiilor principale si afisarea de mesaje, lasand logica cea mai complexa pentru comands.c
- in prima faza, fiecare comanda este citita de la *STDIN* si transformata intr-un integer, pentru a putea fi mai usor de impartit logica;
- comenzile au un numar de la 1 la 9, 0 semnificand faptul ca nu a fost introdusa o comanda valida (default-ul de la switch case);
- dupa citirea fiecare comenzi si transformarea sa intr-un int, se vor face verificarile necesare si apelarea de functii corespunzatoare pentru a putea rula programul (spre exemplu, un enter_library fara un login inainte este invalid);

## Comenzile
### Register
- aici se citesc user-ul si parola pentru contul ce rumeaza a fi inregistrat, urmand a fi transformare in format JSON pentru a fi trimise catre server;
- dupa ce a fost trimits request-ul (de tip POST) catre server, se primeste un mesaj, verificand ulterior daca operatia a avut succes sau nu;
- pentru a usura acest lucru, SUCCESS si ERROR cauta prefixul specific pentru fiecare mesaj, urmat de prefixul de cod (adica, ambele au **HTTP/1.1**, urmate de **2**, pentru succes, respectiv, **4**, pentru eroare);
- probleme care ar putea aparea: nu este verificat daca userul introduce un username valid (sa nu aiba spatii in el);

### Login
- foarte asemanator cu ce se intampla la register, singura diferenta fiind ca, din raspunsul primit de la server, se va extrage un cookie ce va fi returnat catre client si pastrat acolo;
- se va face tot un request de tipul POST;

### Enter Library
- aici, mesajul va contine si cookie-ul primit de user atunci cand a facut login-ul, raspunsul continand de aceasta data un token, pe baza caruia se asigura accesul in libraria asociata user-ului;

### Get Books
- se va face un request de tipul GET, avand nevoie de token-ul obtinut dupa comanda *enter_library*, pentru a extrage toate cartile din librarie;
- raspunsul primit de la server va contine cartile si id-urile lor, afisandu-le doar pe ele;

### Get Book
- comanda asteapta introducerea unui id, pentru a reusi sa extraga toate informatiile despre o carte, si pentru a le afisa;

### Add Book
- citim prima data informatiile despre cartea care urmeaza a fi adaugata, adaugandu-le pe acestea in request ce urmeaza a fi facut, impreuna cu token-ul;
- verificam in raspunsul primit de la server daca operatia a avut loc cu succes sau nu;

### Delete Book
- pe baza token-ului primit la comanda enter_library, se va face o cerere de tipul DELETE pentru a elimina din librarie carte cu id-ul primit;

### Logout
- se va face o cerere de tipul GET catre server pentru a deloga user-ul, iar in client se va sterge cookie-ul si token-ul, pentru a relua tot procesul in cazul in care se doreste relogarea cu alt user;

### Exit
- aici, se elibereaza resursele alocate, se inchide socket-ul, si se incheie programul definitiv;