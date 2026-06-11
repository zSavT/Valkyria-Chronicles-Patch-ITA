# SPIEGAZIONE

_traduttore_ENG_to_ITA_csv.py_ permette di tradurre tutti i file csv presenti nella stessa cartella del .py ed inserire l'output nella cartella "tradotto". <br>
Alcuni file csv hanno una struttura differenti, quello che interessa è la _c_lettura_ e _c_scrittura_. La c_scrittura corrisponde alla colonna di output della traduzione (tendenzialmente l'ultima), la c_lettura è la colonna dove è presente il testo originale in inglese.

Ovviamente la traduzione non sempre è perfetta, alcune parole vengono tradotte per errore (come alcuni nomi), la formatazzione non sempre è corretta ed alcuni caratteri non sono letti correttamente.

## CARATTERI PARTICOLARI

Tutte le lettere accentate non sono supportate dal font originale del gioco, la patch francese modifica il font originale per supportare anche questi caratteri ma non tutti quelli utilizzati anche nella lingua italiana. I caratteri vengono sostituiti da altri nel testo.<br>

Allo stato attuale i caratteri supportati sono:
```
< --> é
> --> è
= --> à
```
Il resto viene modificato con l'aggiunta di " _'_ ".

## LUNGHEZZA MASSIMA - ESEMPIO CON FILE MTPA_ADV_XX

Da cosa ho potuto comprendere il carattere "_&_", ha la stessa funzione del "_\n_". Il carattere per andare a capo, deve essere inserito prima dei 60 caratteri, in caso contrario, la visualizzazione del testo in gioco risulterà errata. Per questo il codice per le stringhe sotto i 120 caratteri (le stringhe che superano i 120 caratteri devono necessariamente essere riassunte\accorciate, ci sono anche dei casi anomali ancora da analizzare), tokenizza la stringa, elimina l'ultima parola dalla prima parte, inserisce il simbolo "&" ed inserisci all'inizio della seconda parte, la parola scartata. In alcuni casi invece, se la seconda parte della stringa supera i 60 caratteri, si, preferisce inserire un semplice "-&" per dividere le stringhe di testo.<br>In ogni caso, tutti i testi rimangono da rivisionare.


### NOTE

Il codice è stato scritto inizialmente a mano, successivamente espanso con ChatGPT e poi modificato continuamente così via...

---

## VALKYRIA_TOOL.PY — TOOL UNIFICATO MXE + MTP

Il file `valkyria_tool.py` sostituisce i vecchi tool C++ (`mxe_write.exe` e `mtp_write.exe`) con un unico script Python 3 che gestisce entrambi i formati binari del gioco.

### Requisiti
- Python 3.8+
- Nessuna libreria esterna necessaria

### Comandi disponibili

#### Estrazione (da binario a CSV)
```bash
# Estrae stringhe da un singolo file MXE
python valkyria_tool.py extract --format mxe --input file.mxe --output output.csv

# Estrae stringhe da un singolo file MTP
python valkyria_tool.py extract --format mtp --input file.mtp --output output.csv

# Estrazione massiva di tutti i file MXE/MTP in una cartella
python valkyria_tool.py extract-all --src ./cartella_binari --dest ./cartella_csv
```

#### Compilazione (da CSV tradotto a binario)
```bash
# Compila un CSV per un file MXE (richiede il template originale .mxe)
python valkyria_tool.py compile --format mxe --csv traduzione.csv --template originale.mxe --output nuovo.mxe

# Compila un CSV per un file MTP (richiede il template originale .mtp)
python valkyria_tool.py compile --format mtp --csv traduzione.csv --template originale.mtp --output nuovo.mtp

# Compilazione massiva di tutti i CSV in una cartella
python valkyria_tool.py compile-all --csv-dir ./csv --template-dir ./originali --dest-dir ./compilati
```

### Struttura CSV attesa
Ogni file CSV deve avere `;` come separatore e la seguente struttura:
- **Colonna 1**: Offset del puntatore nel file binario (decimale)
- **Colonna 2**: Offset del testo nel file binario (decimale)
- **Colonna 3**: Info aggiuntive (capitolo per MXE, lunghezza per MTP)
- **Colonna 4**: Testo originale inglese
- **Colonna 5+**: Traduzione italiana (viene usata l'ultima colonna non vuota)

### Sostituzioni font automatiche
Il tool applica automaticamente le sostituzioni di font durante la compilazione:
- `é` → `<` (e acuta)
- `è` → `>` (e grave)
- `à` → `=` (a grave)
- `ì` → `i'`
- `ò` → `o'`
- `ù` → `u'`

Per file MXE: `£` viene sostituito con `"` (virgolette doppie).
Per file MTP: `£` con spazio, `¤` con `"`, `&` con newline `\n`, `<Y>` con `\x81\xA2`.