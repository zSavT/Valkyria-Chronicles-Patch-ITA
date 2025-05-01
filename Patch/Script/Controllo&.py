import os
import csv

def analyze_csv_files():
    input_folder = os.path.dirname(os.path.abspath(__file__))
    output_folder = input_folder

    output_file = "results.txt"
    strings_file = "stringhe.txt"
    results_path = os.path.join(output_folder, output_file)
    strings_path = os.path.join(output_folder, strings_file)

    with open(results_path, "w", encoding="utf-8") as results, open(strings_path, "w", encoding="utf-8") as strings:
        for filename in os.listdir(input_folder):
            if filename.endswith(".csv"):
                file_path = os.path.join(input_folder, filename)

                with open(file_path, "r", encoding="utf-8", errors="replace") as csv_file:
                    reader = csv.reader(csv_file, delimiter=";")

                    for row_num, row in enumerate(reader, start=1):
                        if len(row) > 0:  
                            last_column = row[-2]
                            if last_column.count("&") > 1:
                                results.write(f"File: {filename}, Riga: {row_num}, Occorrenze di '&': {last_column.count('&')}\n")
                                strings.write(f"{last_column}\n")

    print(f"Analisi completata. Risultati salvati in {results_path} e {strings_path}")

analyze_csv_files()
