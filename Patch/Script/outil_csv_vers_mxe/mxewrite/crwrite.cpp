#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <vector>
#include <algorithm> 
#include <stdexcept> 

using namespace std;

// --- INIZIO FUNZIONI DI SUPPORTO ---

void sostituisci_carattere(std::string& str) {
    const char PLACEHOLDER = (char)0xA3; // £
    const char TARGET_CHAR = (char)0x22; // "
    
    size_t start_pos = 0;
    while((start_pos = str.find(PLACEHOLDER, start_pos)) != std::string::npos) {
        str.replace(start_pos, 1, 1, TARGET_CHAR);
        start_pos += 1;
    }
}

int cerca_marker(const std::vector<char>& file_data, const std::vector<char>& marker, int start_index = 0) {
    auto it = std::search(
        file_data.begin() + start_index, 
        file_data.end(), 
        marker.begin(), 
        marker.end()
    );

    if (it == file_data.end()) {
        // Fallback: cerca dall'inizio
        it = std::search(
            file_data.begin(), 
            file_data.end(), 
            marker.begin(), 
            marker.end()
        );
        
        if (it == file_data.end()) {
            std::ostringstream oss_hex;
            for(size_t i = 0; i < marker.size(); ++i) {
                oss_hex << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)marker[i] << " ";
            }
            throw std::runtime_error("Marcatore esadecimale '" + oss_hex.str() + "' non trovato nel file.");
        }
    }
    
    return std::distance(file_data.begin(), it);
}

void scrivi_indirizzo_le(std::vector<char>& file_data, int posizione, int valore_indirizzo) {
    if ((size_t)posizione + 3 >= file_data.size()) {
        throw std::runtime_error("Posizione di scrittura indirizzo non valida (fuori dai limiti).");
    }
    file_data[posizione]     = (valore_indirizzo & 0x000000FF);
    file_data[posizione + 1] = (valore_indirizzo & 0x0000FF00) >> 8;
    file_data[posizione + 2] = (valore_indirizzo & 0x00FF0000) >> 16;
    file_data[posizione + 3] = (valore_indirizzo & 0xFF000000) >> 24;
}

// --- FINE FUNZIONI DI SUPPORTO ---

struct  Tableajout
{
    string  Adresse_de_l_adresse;
    string Chapitre;
    string  Adresse;
    string Texte_anglais;
    string Texte_traduit;
    int difftaille;
    int nombreligne;
};

unsigned long int hex_to_dec (string input) {
    return strtol(input.c_str(), NULL, 16);
}
string DecToHex(int n) {
    ostringstream oss;
    oss << hex << n;
    return oss.str();
}
string string_dec_to_hex (string t) {
    std::istringstream iss( t );
    int nombre;
    iss >> nombre;
    return DecToHex(nombre);
}
int string_dec_to_int (string t) {
    std::istringstream issbasetexte( t );
    int nombre;
    issbasetexte >> nombre;
    return nombre;
}


int main()
{
    string Nomfichiersource;
    string  Nomfichiertraduit;
    string Nomfichiercsv;
    string contenutooption; 

    std::vector<char> contenufile;
    Tableajout Laliste[12000];
    std::ostringstream oss;

    int i=0;
    /****************************************************recup nom fichier****************/
    ifstream optionfile("mxe_write.ini", ios::in);
    if(optionfile) {
        getline(optionfile, contenutooption); 
        oss.str("");
        oss <<   contenutooption << ".mxe"; 
        Nomfichiersource = oss.str();
        oss.str("");
        oss <<  contenutooption << ".csv"; 
        Nomfichiercsv = oss.str();
        oss.str("");
        oss  <<  contenutooption << "_new.mxe"; 
        Nomfichiertraduit = oss.str();
        oss.str("");
    }
    else {
        cout << "probleme ouverture fichier option"  << endl;
        return 1;
    }

    /*******************************lecture csv et rangement dans la struct********/
    ifstream fichier(Nomfichiercsv.c_str(), ios::in);
    if(fichier) {
        string contenuto; 
        int t=0;
        while(getline(fichier, contenuto)) { 
            char sep = ';';
            std::string s=contenuto; 
            t=0;
            for(size_t p=0, q=0; p!=s.npos; p=q) {
                oss << ( s.substr(p+(p!=0), (q=s.find(sep, p+1))-p-(p!=0))) ;
                if (t==0) Laliste[i].Adresse_de_l_adresse =oss.str();
                if (t==1) Laliste[i].Adresse = oss.str();
                if (t==2) Laliste[i].Chapitre =oss.str();
                if (t==3) Laliste[i].Texte_anglais =oss.str();
                if (t==5) {
                    Laliste[i].Texte_traduit =oss.str();
                    sostituisci_carattere(Laliste[i].Texte_traduit); // [cite: 4]
                }
                t++;
                oss.str("");
            }
            i++;
        }
        fichier.close();
    }
    else {
        cerr << "Impossible d'ouvrir le fichier CSV !" << endl;
        return 1;
    }

    cout << "Lettura file CSV terminata." << endl;
    Laliste[0].nombreligne = i;
    oss.str("");

    /**********************************modif ancienne adresse per riflettere il cambiamento di dimensione********/
    int j = 0;
    for (i=0; i<Laliste[0].nombreligne; i++) {
        Laliste[i].difftaille = Laliste[i].Texte_traduit.length() - Laliste[i].Texte_anglais.length();
        if (i + 1 < Laliste[0].nombreligne && Laliste[i].difftaille && 
           (string_dec_to_int(Laliste[i+1].Adresse) != string_dec_to_int(Laliste[i].Adresse))) 
        {
            for (j=i+1; j< Laliste[0].nombreligne; j++) {
                if (string_dec_to_int(Laliste[j].Adresse) > string_dec_to_int(Laliste[i].Adresse)) {
                    std::istringstream iss( Laliste[j].Adresse );
                    std::ostringstream o2ss;
                    int nombre;
                    iss >> nombre;
                    nombre = nombre + Laliste[i].difftaille;
                    o2ss << nombre;
                    Laliste[j].Adresse =  o2ss.str();
                }
            }
        }
    }
    cout << "Modifica indirizzi per differenza dimensione testi terminata." << endl;

    /***************************************on remplie un vector avec le fichier source***********************************************/
    std::ifstream lecture(Nomfichiersource.c_str(), std::ios_base::binary);
    if (lecture) {
        lecture.seekg(0, std::ios_base::end);
        int length2 = lecture.tellg();
        lecture.seekg(0, std::ios_base::beg);
        
        contenufile.resize(length2);
        lecture.read(&contenufile[0], length2);
        lecture.close();
    }
    else {
        cout << "problema apertura file sorgente"  << endl;
        return 1;
    }


    /***************************************** 1. Modif des adresses du nouveau texte (COMUNE A ENTRAMBI) *****************************/
    cout << "Inizio modifica puntatori di testo..." << endl;
    for (int j=0; j < Laliste[0].nombreligne ; j++ ) {
        int addr_ptr = string_dec_to_int(Laliste[j].Adresse_de_l_adresse);
        int addr_text = string_dec_to_int(Laliste[j].Adresse);
        scrivi_indirizzo_le(contenufile, addr_ptr, addr_text);
    }
    cout << "Fine modifica puntatori di testo." << endl;


    // --- *** INIZIO LOGICA CONDIZIONALE *** ---
    
    // Il marcatore è SEMPRE POF0 (con lo zero)
    const std::vector<char> POF0_MARKER = {(char)0x50, (char)0x4F, (char)0x46, (char)0x30}; // "POF0"
    const std::vector<char> EOFC_MARKER = {(char)0x45, (char)0x4F, (char)0x46, (char)0x43}; // "EOFC"

    if (contenutooption == "game_info_field") 
    {
        // --- LOGICA DI AUTOMAZIONE COMPLETA (per game_info_field) ---
        cout << "Rilevato 'game_info_field', avvio automazione completa..." << endl;
        try {
            
            int base_text_data_addr = string_dec_to_int(Laliste[0].Adresse) + 32;
            
            // Trova il marcatore POF0 (sarà PRIMA del testo)
            int pofo_orig_addr = cerca_marker(contenufile, POF0_MARKER, 0); // Cerca dall'inizio
            
            // Trova il marcatore EOFC (sarà DOPO il testo)
            int eofc_after_text_addr = cerca_marker(contenufile, EOFC_MARKER, base_text_data_addr);
            
            cout << "Marcatore POF0 originale trovato a: 0x" << hex << pofo_orig_addr << dec << endl;
            cout << "Marcatore EOFC dopo il testo trovato a: 0x" << hex << eofc_after_text_addr << dec << endl;

            // Salva il blocco di fine (da EOFC in poi). Questo NON include POF0.
            std::vector<char> end_markers_block;
            end_markers_block.assign(contenufile.begin() + eofc_after_text_addr, contenufile.end());
            
            // Cancella il vecchio testo (dall'inizio del testo fino a EOFC)
            contenufile.erase(contenufile.begin() + base_text_data_addr, contenufile.begin() + eofc_after_text_addr);

            cout << "Vecchio testo inglese rimosso." << endl;

            // Costruzione nuovo blocco di testo
            cout << "Inizio costruzione nuovo blocco di testo..." << endl;
            std::vector<char> new_text_block;
            int base_text_data_address_new = string_dec_to_int(Laliste[0].Adresse) + 32;

            for (int k=0; k < Laliste[0].nombreligne; k++) {
                if (k > 0 && Laliste[k].Adresse == Laliste[k-1].Adresse) continue;
                int current_abs_text_addr = string_dec_to_int(Laliste[k].Adresse) + 32;
                int relative_offset = current_abs_text_addr - base_text_data_address_new;
                
                if (relative_offset < 0) {
                    cerr << "Errore: Offset negativo calcolato." << endl;
                    continue;
                }
                
                size_t required_size = relative_offset + Laliste[k].Texte_traduit.length() + 1;
                if (new_text_block.size() < required_size) {
                    new_text_block.resize(required_size, 0x00);
                }
                std::copy(Laliste[k].Texte_traduit.begin(), Laliste[k].Texte_traduit.end(), new_text_block.begin() + relative_offset);
                new_text_block[relative_offset + Laliste[k].Texte_traduit.length()] = 0x00;
            }
            contenufile.insert(contenufile.end(), new_text_block.begin(), new_text_block.end());
            cout << "Nuovo blocco di testo inserito." << endl;

            // Allineamento (deve allineare l'EOFC ora)
            int marker_alignment_addr = contenufile.size();
            int padding = (16 - (marker_alignment_addr % 16)) % 16; 
            contenufile.insert(contenufile.end(), padding, 0x00);
            int final_marker_addr = contenufile.size(); // Questo è l'indirizzo del nuovo EOFC1
            cout << "Marcatore di fine (EOFC) allineato a: 0x" << hex << final_marker_addr << dec << " (aggiunti " << dec << padding << " byte di padding)" << endl;

            // Inserisci il blocco di fine salvato (che inizia con EOFC)
            contenufile.insert(contenufile.end(), end_markers_block.begin(), end_markers_block.end());

            // Aggiornamento Header
            cout << "Inizio aggiornamento header..." << endl;
            int final_pofo_addr_for_header = pofo_orig_addr; // L'indirizzo del POF0 non è cambiato
            int final_eofc1_addr = final_marker_addr; // L'indirizzo del primo EOFC è quello allineato
            int final_eofc2_addr = cerca_marker(contenufile, EOFC_MARKER, final_eofc1_addr + EOFC_MARKER.size());
            
            int val_addr_eofc_ultimo = final_eofc2_addr - 0x20;
            int val_addr_eofc_penultimo = final_eofc1_addr - 0x20;
            int val_addr_pofo = final_pofo_addr_for_header - 0x40;

            scrivi_indirizzo_le(contenufile, 0x04, val_addr_eofc_ultimo);
            scrivi_indirizzo_le(contenufile, 0x24, val_addr_eofc_penultimo);
            scrivi_indirizzo_le(contenufile, 0x34, val_addr_pofo);

            cout << "Header aggiornato." << endl;

        } catch (const std::exception& e) {
            cerr << "\nERRORE CRITICO DURANTE LA RICOSTRUZIONE: " << e.what() << endl;
            return 1;
        }
    } 
    else 
    {
        // --- LOGICA ORIGINALE (per game_info_game_param e altri) ---
        // Come da video, questo NON cancella il testo inglese e NON aggiorna l'header.
        
        cout << "Rilevato file '" << contenutooption << "', uso la logica di inserimento originale (senza cancellazione)..." << endl; 
        
        cout << "Inizio scrittura testo tradotto..." << endl;

        // Calcola spazio extra necessario
        int espaceprevu = 0;
        for (int i=0; i<Laliste[0].nombreligne; i++)  espaceprevu = espaceprevu + Laliste[i].difftaille;
        cout << "Spazio extra previsto: " <<   espaceprevu  << "  byte | Righe: " << Laliste[0].nombreligne <<endl;
        
        // Inserisce/Rimuove byte di padding
        int start_addr = string_dec_to_int (Laliste[0].Adresse)+32;
        if (espaceprevu > 0) {
            cout << "Inserimento " << espaceprevu << " byte di padding a 0x" << hex << start_addr << dec << endl;
            contenufile.insert(contenufile.begin() + start_addr, espaceprevu, char(0));
        } else if (espaceprevu < 0) {
            cout << "Testo tradotto piu' corto. Rimozione " << abs(espaceprevu) << " byte da 0x" << hex << start_addr << dec << endl;
            contenufile.erase(contenufile.begin() + start_addr, contenufile.begin() + start_addr + abs(espaceprevu));
        }


        // Scrive il testo tradotto nelle posizioni corrette
        for (int k=0; k< Laliste[0].nombreligne; k++)
        {
            if (k > 0 && Laliste[k].Adresse == Laliste[k-1].Adresse) {
                 continue; 
            }

            std::istringstream issint (Laliste[k].Adresse);
            int nombre;
            issint >> nombre;
            int temp = nombre + 32; // +32 offset
            
            for ( size_t l = 0; l <  Laliste[k].Texte_traduit.length() ; l++ ) { 
                int write_pos = temp + l;
                if ((size_t)write_pos < contenufile.size()) { 
                    contenufile[write_pos] = Laliste[k].Texte_traduit[l];
                } else {
                    contenufile.insert(contenufile.begin() + write_pos, Laliste[k].Texte_traduit[l]);
                }
            }
            int null_pos = temp + Laliste[k].Texte_traduit.length();
            if ((size_t)null_pos < contenufile.size()) { 
                contenufile[null_pos] = char(0);
            } else {
                contenufile.insert(contenufile.begin() + null_pos, char(0));
            }
        }
        cout << "Scrittura testo tradotto completata." << endl;
    }
    
    // --- *** FINE LOGICA CONDIZIONALE *** ---


    /*******************************ecriture vector dans new mxe *****************************************************/
    cout << "Scrittura file finale: " << Nomfichiertraduit << endl;
    ofstream sortiefile(Nomfichiertraduit.c_str(), ios::out | ios::trunc | std::ios_base::binary);

    if(sortiefile) {
        sortiefile.write(&contenufile[0], contenufile.size());
        sortiefile.close();
    }
    else {
        cerr << "Errore all'apertura del file di output!" << endl;
        return 1;
    }

    cout << "--- Processo completato con successo ---" << endl;

    return 0;
}