# Cartella MXE — Template + Traduzioni

Questa cartella contiene i file necessari per compilare le stringhe di gioco nel formato `.mxe`.

## Struttura
```
mxe/
├── *.mxe          <- Template originali del gioco (necessari per la compilazione)
├── *.csv          <- Traduzioni italiane
└── compilati/     <- Output generato da compile-all (creato automaticamente)
```

## Come usare

### Compilazione massiva di tutti i file
```bash
python ../valkyria_tool.py compile-all --dir .
```
> Genera i file compilati nella sottocartella `compilati/`

### Compilazione di un singolo file
```bash
python ../valkyria_tool.py compile --template game_info_field.mxe --csv game_info_field.csv --output game_info_field_new.mxe
```

### Estrazione stringhe da un file .mxe
```bash
python ../valkyria_tool.py extract --input game_info_field.mxe --output game_info_field.csv
```

## File presenti
| File | Tipo | Descrizione |
|------|------|-------------|
| `book_person_info.mxe` | Template | Informazioni sui personaggi del libro |
| `book_person_info.csv` | Traduzione | Traduzione italiana |
| `game_info_field.mxe` | Template | Informazioni sui campi di battaglia |
| `game_info_field.csv` | Traduzione | Traduzione italiana |
