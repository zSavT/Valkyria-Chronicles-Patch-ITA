import csv
import chardet
import os
import time
import sys
from deep_translator import GoogleTranslator
import threading

# Colori ANSI per la shell
GREEN = '\033[92m'
YELLOW = '\033[93m'
RED = '\033[91m'
RESET = '\033[0m'


def rileva_codifica(file_path):
    with open(file_path, "rb") as f:
        result = chardet.detect(f.read())
    return result["encoding"]


def split_string(s):
    parts = s.split('&')  # Divide la stringa in base a tutti i '&'
    parts = [p for p in parts if p]  # Rimuove eventuali stringhe vuote

    return parts  # Restituisce tutte le sottostringhe trovate


def modify_substring(substring, translator):
    substring = translator.translate(substring)
    return substring


def process_string(s, translator):
    substrings = split_string(s)
    tradotto_temp = translator.translate(s)


    if tradotto_temp is not None and len(tradotto_temp) <= 120:
        # Se la lunghezza della stringa è maggiore o uguale a 59 e 60, dividerla
        if len(tradotto_temp) > 59:
            # Inserisce il separatore "-&" tra i caratteri 59 e 60
            part1 = tradotto_temp[:59]
            part2 = tradotto_temp[59:60]
            part3 = tradotto_temp[60:]
            out = part1 + '-&' + part2 + part3
        else:
            out = tradotto_temp
    else:
        modified_substrings = [modify_substring(sub, translator) for sub in substrings]
        if modified_substrings[0] is None:
            out = s
        else:
            out = '&'.join(modified_substrings)  # Ricombina le sottostringhe

    print(f"\n{YELLOW}ENG: {s} {RESET}")
    print(f"{GREEN}ITA: {out} {RESET}")
    return out


def mostra_caricamento():
    simboli = ['|', '/', '-', '\\']
    while not stop_loading:
        for simbolo in simboli:
            sys.stdout.write(f'\r{simbolo} Traduzione in corso...')
            sys.stdout.flush()
            time.sleep(0.2)
    sys.stdout.write('\r')


def traduci_csv(file_input, file_output):
    c_scrittura = 5
    c_lettura = 4

    print(f"Traduzione del file: {file_input}")

    global stop_loading
    stop_loading = False

    thread_caricamento = threading.Thread(target=mostra_caricamento)
    thread_caricamento.start()

    translator = GoogleTranslator(source='en', target='it')
    encoding = rileva_codifica(file_input)

    try:
        with open(file_input, 'r', encoding=encoding, errors='ignore') as infile, open(file_output, 'w',
                                                                                       encoding=encoding, newline='',
                                                                                       errors='ignore') as outfile:
            reader = csv.reader(infile, delimiter=';')
            writer = csv.writer(outfile, delimiter=';')

            for row in reader:
                if len(row) > 4 and row[c_lettura]:
                    if isinstance(row[c_lettura], str):
                        translated_text = process_string(row[c_lettura], translator)
                        translated_text = translated_text.replace("é", "<").replace("è", ">").replace("à", "=").replace(
                            "ì", "i").replace(
                            "ò", "o'").replace("ù", "u'").replace("Ì", "I'").replace("È", "E'")
                    row[
                        c_scrittura] = translated_text  # Sovrascrive parte francese della traduzione con quella italiana
                writer.writerow(row)

    except UnicodeDecodeError:
        print(f"Errore durante la lettura del file {file_input}.")

    # Ferma il thread di caricamento
    stop_loading = True
    thread_caricamento.join()

    print(f"✅ Traduzione completata! File salvato in: {file_output}\n")


def traduci_cartella(cartella_input):
    cartella_output = os.path.join(cartella_input, "tradotto")
    os.makedirs(cartella_output, exist_ok=True)

    for file in os.listdir(cartella_input):
        if file.endswith(".csv"):
            file_input = os.path.join(cartella_input, file)
            file_output = os.path.join(cartella_output, file)
            traduci_csv(file_input, file_output)

    print("🎉 Tutti i file sono stati tradotti e salvati nella cartella 'tradotto'.")


# Start
start = time.time()
traduci_cartella(".")
end = time.time()
print("🏁 Terminato in: ", end - start)
