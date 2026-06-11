# Cartella MTP — Template + Traduzioni

Questa cartella contiene i file necessari per compilare le stringhe di gioco nel formato `.mtp` (dialoghi, cinematiche, interfacce di battaglia).

## Struttura
```
mtp/
├── templates/     <- File .mtp originali del gioco (da estrarre dalla propria copia)
│   └── (vuota, vedi sotto)
└── csv/           <- Traduzioni italiane (67 file)
    ├── mtpa_adv_00.csv   <- Dialoghi cinematica capitolo 0
    ├── mtpa_adv_01.csv   <- Dialoghi cinematica capitolo 1
    ├── mtpa_slg_XX.csv   <- Dialoghi in battaglia
    ├── mtpa_book.csv     <- Testi del libro di gioco
    ├── mtpa_sys.csv      <- Testi di sistema
    └── ...
```

## Come ottenere i template .mtp
I file `.mtp` originali devono essere estratti dalla propria copia legale del gioco Valkyria Chronicles (Steam).
Si trovano in: `Steam\steamapps\common\Valkyria Chronicles\data\resource\mtpa\`

Copiare i file `.mtp` nella cartella `templates/`.

## Come usare

### Compilazione massiva di tutti i file MTP
```bash
# Template in templates/, CSV in csv/, output in compilati/
python ../../valkyria_tool.py compile-all --dir ./templates --csv-dir ./csv --dest ./compilati
```

### Compilazione di un singolo file
```bash
python ../../valkyria_tool.py compile --template templates/mtpa_adv_00.mtp --csv csv/mtpa_adv_00.csv --output mtpa_adv_00_new.mtp
```

### Estrazione stringhe da un file .mtp
```bash
python ../../valkyria_tool.py extract --input templates/mtpa_adv_00.mtp --output csv/mtpa_adv_00.csv
```
