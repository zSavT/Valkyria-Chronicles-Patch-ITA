# Valkyria Chronicles Patch ITA 

<p align="center">
  <img src="img/Logo.png" /><br>
    Progetto per la traduzione del gioco Valkyria Chronicles in italiano.
</p>

![GitHub Issues or Pull Requests](https://img.shields.io/github/issues/zSavT/Valkyria-Chronicles-Patch-ITA?logo=github)
![GitHub contributors](https://img.shields.io/github/contributors/zSavT/Valkyria-Chronicles-Patch-ITA?logo=refinedgithub)
![GitHub last commit (branch)](https://img.shields.io/github/last-commit/zSavT/Valkyria-Chronicles-Patch-ITA/main?display_timestamp=committer&logo=github&label=last%20commit%3A%20main)
![GitHub last commit (branch)](https://img.shields.io/github/last-commit/zSavT/Valkyria-Chronicles-Patch-ITA/texture_replace?display_timestamp=committer&logo=github&label=last%20commit%3A%20texture_replace)
![GitHub last commit (branch)](https://img.shields.io/github/last-commit/zSavT/Valkyria-Chronicles-Patch-ITA/traduzione_mpta?display_timestamp=committer&logo=github&label=last%20commit%3A%20traduzione_mpta)
[![Donate](https://img.shields.io/badge/Donate-PayPal-blue.svg)](https://www.paypal.com/paypalme/verio12)


# INDICE

- [Valkyria Chronicles Patch ITA](#valkyria-chronicles-patch-ita)
- [INDICE](#indice)
  - [STRUTTURA DEI FILE](#struttura-dei-file)
    - [MTP](#mtp)
    - [MXE](#mxe)
      - [COME CONVERTIRE I CSV NEI FILE MXE e MTP](#come-convertire-i-csv-nei-file-mxe-e-mtp)
    - [HTX - MLX - DDS](#htx---mlx---dds)
      - [COME CONVERTIRE I FILE HTX e MLX](#come-convertire-i-file-htx-e-mlx)
  - [TO DO](#to-do)
  - [COME SEGNALARE ERRORI NELLA TRADUZIONE/BUG](#come-segnalare-errori-nella-traduzionebug)
  - [VIDEO DIMOSTRATIVO](#video-dimostrativo)
  - [SCREEN PROGETTO](#screen-progetto)
  - [CREDITI](#crediti)

## STRUTTURA DEI FILE

### MTP

Nella cartella "_Valkyria Chronicles\data\resource\mtpa_" sono presenti la maggior parte dei testi delle cinematiche, dei dialoghi in battaglia e del menu\libro. In particolare:

- __mtpa_adv_XX__, contiene i dialoghi presenti nelle cinematiche.
    - Limite massimo di caratteri: 120 (60 per riga)
    - Carattere a capo: &
- __mtpa_slg_XX__, contiene i dialoghi dei box presenti nelle battaglie.
    - Limite massimo di caratteri: 42/43
    - Carattere a capo: & (genera anche nuovi box di testo per inserire il testo mancante)
- ~~__mtpa_sys__, contiene testi vari per alcune opzioni del gioco e capitoli.~~ Il file non ha alcun impatto nel gioco, utilizzato solo per la versione PS3 del gioco.
    - ~~Limite massimo di caratteri: ?~~
    - ~~Carattere a capo: &~~
- __mtpa_sys_battle__, contiene testi vari per testi generici delle battaglie.
    - Limite massimo di caratteri: ?
    - Carattere a capo: &
- __mtpa_book__, contiene testi del menu libro dei capitoli.
    - Limite massimo di caratteri: ?
    - Carattere a capo: &
- __mtpa_town__, contiene testi vari, tra cui quelli dei box nel libro.
    - Limite massimo di caratteri: ?
    - Carattere a capo: &

### MXE

Nella cartella "_Valkyria Chronicles\data\mx_" sono presenti tutti i file mxe che contengono tutto il testo restante (Impostazioni, nomi oggetti ecc...)<br>

#### COME CONVERTIRE I CSV NEI FILE MXE e MTP

Per poter convertire i file CSV nei file MTP, bisogna utilizzare il tool creato da dell'utente "__Feunoir__" postato [qui](https://www.jeuxvideo.com/forums/42-14107-38261510-1-0-1-0-traduction-du-jeu-aide-bienvenue.htm), presente nella repo nella cartella "Patch/Script/outil_csv_vers_mtp". Per convertire i file bisogna inserire nella stessa cartella dove è presente il file exe "_mtp_write.exe_", i file mtp originali e i relativi csv. <br>
Nel file "_mtp_write.ini_" bisogna inserire il nome del file da convertire senza l'estensione (Esempio: convertire il file mtpa_adv_XX.mtp, nel file .ini si inserisce solo "_mtpa_adv_XX_"). Una volta impostato tutto, basta avviare il file exe. In questo modo viene creato un nuovo file "_mpt_" con l'aggiunta di "_new_" in coda al nome. Viene generato anche un file "_test.csv_", che contiene le info sulle lunghezze delle stringhe originali, quelle tradotte e la loro differenza.<p>Questo processo bisogna farlo per ogni file. Se bisogna convertire tanti file, per velocizzare il processo, ho creato uno script python "__mtpConverterAutomatico.py__" (L'autore ha rilasciato anche il codice sorgente del programma, ma son pigro e non mi andava di debbuggare per capire come modificarlo per automatizzarlo per tutti i file). L'unica prerogativa è inserire tutti i file originali e csv nella cartella dell'eseguibile.
Lo stesso identico discorso vale anche per i file MXE (cambia solo la cartella).

![Tool](img/toolMXE-MPT.gif)
N.B. <br>
Si, ho dimenticato di resettare il colore della shell a fine esecuzione, son pigro non credo che sistemerò (è solo estetica e vale solo per la singola istanza della shell).



### HTX - MLX - DDS

I file "_htx_" contengono le texture del gioco, dove sono presenti anche le gui del gioco e alcuni testi (come per esempio il messaggio di ending del gioco). Anche i file "_mlx_" contengono le texture per gli ambienti, personaggi ecc... All'interno dei file HTX e MLX sono presenti i file DSS delle texture.<br>
I file _DDS_ bisogna modificarli seguendo le linee guida indicate in questa [issue](https://github.com/zSavT/Valkyria-Chronicles-Patch-ITA/issues/1).


- Nella cartella "_Valkyria Chronicles\data\texture_replace_" sono presenti texture varie tra cui alcune contenente il testo di alcune interfacce di menu.
    - I file delle immagini sono direttamente nel formato "_dds_".
- Nella cartella "_Valkyria Chronicles\data\resource_" nelle varie sottocartella, sono presenti tutti i file HTX e MLX.

#### COME CONVERTIRE I FILE HTX e MLX
  - Per poter modificare questi due tipologie di file, bisogna utilizzare il [tool](https://steamcommunity.com/sharedfiles/filedetails/?id=343016567) di "__FluffyQuack__". 
  - Per estrarre le texture bisogna trascinare il file della texture sul .bat "_extract.bat_". In questo modo viene creata una cartella con lo stesso nome del file contenente tutte le texture presenti in formato .dds da dover modificare.
  - Per ri-compattare il file, bisogna inserire nella cartella (__Con il nome originale della cartella__), le texture modificate (__Con il nome del file originale della texture__). Bisogna trascinare il file originale sul .bat "_replace.bat_". In questo modo verrà modificato.
  - La cartella contenente le texture, i .bat (con annesso exe) ed il file di originale, devono stare nella stessa cartella madre per poter funzionare correttamente.
  - In alternativa si possono utillizzare i commandi tramite CLI indicati nella guida.
![Tool](img/toolHTX-MLX.gif)


## TO DO

- [ ] Comprendere il funzionamento e logica dietro i file mtp
- [ ] Comprendere il funzionamento e logica dietro i file mxe
- [x] Comprendere il funzionamento e logica dietro i file mlx
- [x] Comprendere il funzionamento e logica dietro i file htx
- [ ] Comprendere il funzionamento e logica dietro i file esr
- [ ] Tradurre e adattare tutti i testi mpt
- [ ] Tradurre e adattare tutti i testi mxe
- [ ] Tradurre e adattare tutti i testi esr
- [ ] Rifinire
- [ ] Test completo del gioco

## COME SEGNALARE ERRORI NELLA TRADUZIONE/BUG

Per segnalare errori nella traduzione, bisogna farlo, tramite la sezione "__issue__" della repo e selezionare il template "_Errori di traduzione_".
![Errore1](img/Issue0.png)
![Errore2](img/Issue1.png)
Successivamente bisogna riportare tutte le informazioni richieste per poter correggere l'errore.<br>
Stesso discorso per la segnalazione dei crash/bug del gioco.

## VIDEO DIMOSTRATIVO
[![Video dimostrativo versione pre-alpha](https://img.youtube.com/vi/MNjeAes6j74/0.jpg)](https://youtu.be/MNjeAes6j74?si=MNjeAes6j74)


## SCREEN PROGETTO

![](img/1.jpg)
![](img/2.jpg)
![](img/3.jpg)

## CREDITI

Si ringrazia per tutto lo sfrozo di analisi dell'utente "__Feunoir__" postato [qui](https://www.jeuxvideo.com/forums/42-14107-38261510-1-0-1-0-traduction-du-jeu-aide-bienvenue.htm), per i csv di partenza per la traduzione e per i software di conversione da csv a mtpa e mxe.<br>
Si ringrazia l'utente Steam "__FluffyQuack__" per il tool di estrazione delle texture per i file htx e mlx [tool](https://steamcommunity.com/sharedfiles/filedetails/?id=343016567).
