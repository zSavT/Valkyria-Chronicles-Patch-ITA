#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <vector>




using namespace std;
struct  Tableajout
{
    string  Adresse_de_l_adresse;

    string Chapitre;
    int Adressedecimale;
    string  Adresse;
    int Taille_texte;
    string Texte_taille;
    string Texte_anglais;
    string Texte_traduit;
    int difftaille;

    int taille;

};

unsigned long int hex_to_dec (string input)
{
    unsigned long int output = strtol(input.c_str(), NULL, 16);
    return output;
}
string DecToHex(int n)
{
    ostringstream oss;
    oss<<hex<<n;
    return oss.str();
}

string string_dec_to_hex (string t)
{
    std::istringstream iss( t  );

    int nombre;

    iss >> nombre;

    return DecToHex(nombre);

}

int string_dec_to_int (string t)
{
    std::istringstream issbasetexte( t );
    int nombre;
    issbasetexte >> nombre;
    return nombre;
}


int main()
{
    int diff;
    string Nomfichiersource;
    string  Nomfichiertraduit;
    string Nomfichiercsv;
    Tableajout Laliste[1200];
    std::ostringstream oss;
    int debutdutexte =0;
    int tailledufichier=0;
    int positionENRS =0;
    int positionEOFC1=0;
    int positionEOFC2=0;

    int i=0;

    char *array2;
    std::vector<char> contenufile;

    ifstream optionfile("mtp_write.ini", ios::in);
    if(optionfile)
    {
        string contenuoption;


        getline(optionfile, contenuoption);
        oss.str("");
        oss <<   contenuoption << ".mtp";
        Nomfichiersource = oss.str();
        oss.str("");
        oss <<  contenuoption << ".csv";
        Nomfichiercsv = oss.str();
        oss.str("");
        oss  <<  contenuoption << "_new.mtp";
        Nomfichiertraduit = oss.str();
        oss.str("");
    }

    else cout << "probleme ouverture fichier option"  << endl;
/******************************************************************************************/


    std::ifstream lecture(Nomfichiersource.c_str(), std::ios_base::binary);
    if (lecture)
    {
        tailledufichier = lecture.rdbuf()->pubseekoff(0, std::ios_base::end);

        array2 = new char[tailledufichier];

        lecture.rdbuf()->pubseekoff(0, std::ios_base::beg);
        lecture.read(array2, tailledufichier);


        lecture.close();
    }
    else cout << "probleme ouverture fichier mtp"  << endl;

    contenufile.clear();


    for(int i(0); i<tailledufichier; i++)        contenufile.push_back(array2[i]) ;
    tailledufichier = tailledufichier;
    cout<<  "lecture fichier original " << Nomfichiersource << " finie" << endl;
/**********************************************************************************************/



    ifstream fichier(Nomfichiercsv.c_str(), ios::in);

    if(fichier)
    {
        string contenu;

        int t=0;
        int p =0;
        int q= p;
        while(getline(fichier, contenu))
        {



            char sep = ';';
            std::string s=contenu;
            t=0;
            p =0;
            q= p;
            for(size_t p=0, q=0; p!=s.npos; p=q)
            {
                oss << ( s.substr(p+(p!=0), (q=s.find(sep, p+1))-p-(p!=0))) ;
                if (t==0) Laliste[i].Adresse_de_l_adresse =oss.str();
                if (t==1) Laliste[i].Adresse = oss.str();
                if (t==2) Laliste[i].Taille_texte = string_dec_to_int (oss.str());
                if (t==4) Laliste[i].Texte_anglais =oss.str();
                if (t==5) Laliste[i].Texte_traduit =oss.str();
                t++;

                oss.str("");
            }

            i++;
        }

        fichier.close();
    }
    else
        cout << "Impossible d'ouvrir le fichier csv" << endl;


    cout << "lecture fichier source "<< Nomfichiercsv<<  " finie" << endl;
    /*******************************************************************************************************/


    Laliste[0].taille =i;
    oss.str("");

    debutdutexte = string_dec_to_int (Laliste[0].Adresse);

    Laliste[1].taille = 0;



    for (i=0; i<Laliste[0].taille; i++)
    {
        bool adresseok = true;
        Laliste[i].difftaille = Laliste[i].Texte_traduit.length() - Laliste[i].Texte_anglais.length();
        std::ostringstream o2ss;
        int nombre ;
        if (i==0) nombre = debutdutexte;
        else
        {


            nombre = nombre + Laliste[i-1].Texte_traduit.length()+1;

            while (adresseok)
            {
                for (int k=string_dec_to_int (Laliste[i-1].Adresse) ; k < string_dec_to_int (Laliste[i-1].Adresse) + Laliste[i-1].Texte_traduit.length() + 40 ; k=k+4  )
                {
                    if ( nombre == k) adresseok = false;


                }
                if (adresseok) nombre = nombre +1;
            }

            nombre = nombre+4;
            o2ss << nombre;
            Laliste[i].Adresse =  o2ss.str();
            o2ss.str("");




        }

        Laliste[1].taille = Laliste[1].taille + Laliste[i].difftaille;



    }
    positionENRS = string_dec_to_int (Laliste[Laliste[0].taille-1].Adresse) +Laliste[Laliste[0].taille-1].Texte_traduit.length();


    bool adresseok = true;
    while (adresseok)
    {
        for (int k=0; k<string_dec_to_int (Laliste[Laliste[0].taille-1].Adresse) + Laliste[Laliste[0].taille-1].Texte_traduit.length() + 128 ; k=k+16  )
        {
            if ( positionENRS == k) adresseok = false;


        }
        if (adresseok) positionENRS = positionENRS +1;
    }


    cout << "modif des adresses suivant la diff de taille des textes finie" << endl;
    /*****************************************************************************************************/
    /*


          ofstream save("test.csv", ios::out | ios::trunc);

            if(save)
            {
                for ( int i=0; i< Laliste[0].taille; i++)
     {
         save << Laliste[i].Adresse_de_l_adresse << ";" << Laliste[i].Adresse << ";" << Laliste[i].Texte_traduit.length() << ";" << Laliste[i].Taille_texte << ";" << Laliste[i].Texte_anglais.length() << ";"  <<  Laliste[i].difftaille << ";" << Laliste[i].Texte_anglais << ";" << Laliste[i].Texte_traduit << endl;
     }
                    save.close();
            }
            else
                    cerr << "Erreur à l'ouverture !" << endl;

     cout << "save fichier intermediaire de verif  ok";
     */
    /***********************************************************************************************************/


    cout << "debut preparation du contenu du fichier final" << endl << "modif des adresses relatives" << endl;

    for (int i=0; i<Laliste[0].taille; i++)
    {
        int adresse;
        int adressadresse;
        int x=0;

        std::ostringstream ossconv;
        string  txt1, txt2,txt3, txt4;
        adresse = string_dec_to_int(Laliste[i].Adresse) - debutdutexte;

        adressadresse = string_dec_to_int(Laliste[i].Adresse_de_l_adresse);
        txt4="0";
        txt3="";
        ossconv.str("");


        cout << i << " ";

        if (adresse<17)
        {
            ossconv.str("");
            ossconv << "0" << DecToHex(adresse) << "00" ;
        }
        if ( (adresse>16) && (adresse<257))

        {
            ossconv.str("");
            ossconv << DecToHex(adresse) << "00";

        }
        if ( (adresse>256) && (adresse<4095))
        {
            ossconv << "0" << DecToHex(adresse) ;
            txt1 = ossconv.str();
            txt2= txt1.substr(2);
            txt1.erase(2,2);
            ossconv.str("");
            ossconv << txt2 << txt1 ;
        }





        if (adresse >4094)
        {
            txt1= DecToHex(adresse);
            txt2= txt1.substr(2);
            txt1.erase(2,2);
            ossconv.str("");
            ossconv << txt2 << txt1 ;
        }

        contenufile.erase(contenufile.begin()+adressadresse,contenufile.begin()+adressadresse+2);

        txt4 = ossconv.str();
        txt1=txt4.substr(2);
        txt4.erase(2,2);
        std::istringstream issasc ( txt4 );

        issasc >> std::hex >> x;

        contenufile.insert(contenufile.begin()+adressadresse,char (x));

        issasc.str("");
        x=0;
        std::istringstream issasc2 ( txt1 );

        issasc2 >> std::hex >> x;

        contenufile.insert(contenufile.begin()+adressadresse+1,char (x));

        issasc2.str("");
    }







    cout << "fin ecriture des nouvelles adresse" << endl << "debut ecriture du texte traduit" << endl;;
    /******************************************************************************************************************/


    for ( int i= debutdutexte-7 ; i< string_dec_to_int(Laliste[Laliste[0].taille-1].Adresse)+Laliste[Laliste[0].taille-1].Texte_traduit.length() ; i++) contenufile.insert(contenufile.begin()+i ,  char (0));

    for ( int i= 0 ; i< Laliste[0].taille; i++)


    {
        int hextaille;
        string textconv1, textconv2;
        std::ostringstream ossconv;
        string  tailletemp;
        if (Laliste[i].Texte_traduit.length()<17)
        {
            ossconv.str("");
            ossconv << "0" << DecToHex(Laliste[i].Texte_traduit.length())  ;
        }
        if ( (Laliste[i].Texte_traduit.length()>16) && (Laliste[i].Texte_traduit.length()<257))

        {
            ossconv.str("");
            ossconv << DecToHex(Laliste[i].Texte_traduit.length()) ;

        }
        if (Laliste[i].Texte_traduit.length()>256)
        {
            ossconv.str("");
            ossconv << DecToHex(Laliste[i].Texte_traduit.length()) ;
            tailletemp =  ossconv.str();
            textconv2 = tailletemp.substr(0,1);
            textconv1 = tailletemp.substr(1);
            ossconv.str("");
            ossconv <<  textconv1   ;
            tailletemp =  ossconv.str();

            std::istringstream isstaille1 ( tailletemp );

            isstaille1 >> std::hex >>hextaille;

            if (Laliste[i].Taille_texte) contenufile.insert(contenufile.begin()+string_dec_to_int(Laliste[i].Adresse)-4,char (hextaille));

            isstaille1.str("");
            ossconv.str("");
            ossconv  << "0" << textconv2  ;
            tailletemp =  ossconv.str();

            std::istringstream isstaille2 ( tailletemp );

            isstaille2 >> std::hex >>hextaille;

            if (Laliste[i].Taille_texte) contenufile.insert(contenufile.begin()+string_dec_to_int(Laliste[i].Adresse)-3,char (hextaille));

            isstaille2.str("");
        }

        else
        {




            tailletemp = ossconv.str();
            std::istringstream isstaille ( tailletemp );

            isstaille >> std::hex >>hextaille;

            if (Laliste[i].Taille_texte) contenufile.insert(contenufile.begin()+string_dec_to_int(Laliste[i].Adresse)-4,char (hextaille));

            isstaille.str("");
        }


        // Inserisce il testo tradotto "così com'è" dal CSV
        contenufile.insert(contenufile.begin()+string_dec_to_int(Laliste[i].Adresse), Laliste[i].Texte_traduit.begin(), Laliste[i].Texte_traduit.end());
        

        /*****************************************************************/
        /* INIZIO BLOCCO SOSTITUZIONE CARATTERI SPECIALI                 */
        /* */
        /* Eseguiamo tutte le sostituzioni *dopo* l'inserimento          */
        /* e *dall'indietro* (rfind) per evitare problemi con gli offset */
        /* quando la lunghezza della sostituzione è diversa.             */
        /*****************************************************************/
        
        int base_address = string_dec_to_int(Laliste[i].Adresse);
        string placeholder;
        std::size_t found;

        // --- 1. Sostituzione Simbolo Y: "<Y>" -> 0x81 0xA2 ---
        // Sostituzione N-a-M (3 byte -> 2 byte)
        // !! CAMBIA "<Y>" CON IL PLACEHOLDER CHE USI NEL TUO CSV !!
        placeholder = "<Y>"; 
        found = Laliste[i].Texte_traduit.rfind(placeholder);
        while (found != std::string::npos)
        {
            // Rimuove il placeholder (es. 3 byte per "<Y>")
            contenufile.erase(contenufile.begin() + base_address + found,
                              contenufile.begin() + base_address + found + placeholder.length());

            // Inserisce i nuovi byte (2 byte)
            contenufile.insert(contenufile.begin() + base_address + found, (char)0x81);
            contenufile.insert(contenufile.begin() + base_address + found + 1, (char)0xA2);
            
            if (found == 0) break;
            found = Laliste[i].Texte_traduit.rfind(placeholder, found - 1);
        }

        // --- 2. Sostituzione "R£D" -> "R&D" (da Lisezmoi.txt) ---
        // Sostituzione 1-a-1 (3 byte -> 3 byte)
        placeholder = "R£D";
        found = Laliste[i].Texte_traduit.rfind(placeholder);
        while (found != std::string::npos)
        {
            // Sostituisce i 3 byte
            contenufile.erase(contenufile.begin() + base_address + found,
                              contenufile.begin() + base_address + found + placeholder.length());
            contenufile.insert(contenufile.begin() + base_address + found, 'R');
            contenufile.insert(contenufile.begin() + base_address + found + 1, '&');
            contenufile.insert(contenufile.begin() + base_address + found + 2, 'D');

            if (found == 0) break;
            found = Laliste[i].Texte_traduit.rfind(placeholder, found - 1);
        }

        // --- 3. Sostituzione "£" -> " " (spazio) (da Lisezmoi.txt) ---
        // Sostituzione 1-a-1 (1 byte -> 1 byte)
        placeholder = "£";
        found = Laliste[i].Texte_traduit.rfind(placeholder);
        while (found != std::string::npos)
        {
            contenufile.erase(contenufile.begin() + base_address + found,
                              contenufile.begin() + base_address + found + placeholder.length());
            contenufile.insert(contenufile.begin() + base_address + found, ' ');
            
            if (found == 0) break;
            found = Laliste[i].Texte_traduit.rfind(placeholder, found - 1);
        }
        
        // --- 4. Sostituzione "&" -> 0x0A (A capo) ---
        // Sostituzione 1-a-1 (1 byte -> 1 byte)
        placeholder = "&";
        found = Laliste[i].Texte_traduit.rfind(placeholder);
        while (found != std::string::npos)
        {
            contenufile.erase(contenufile.begin() + base_address + found,
                              contenufile.begin() + base_address + found + placeholder.length());
            contenufile.insert(contenufile.begin() + base_address + found, (char)0x0A);
            
            if (found == 0) break;
            found = Laliste[i].Texte_traduit.rfind(placeholder, found - 1);
        }

        /*****************************************************************/
        /* FINE BLOCCO SOSTITUZIONE                                      */
        /*****************************************************************/

    }


    for ( int i= positionENRS+4 ; i< contenufile.size()-10; i++)
    {
        std::ostringstream scantxt ;
        string testtxt;
        string compp = "ENRS";
        scantxt << contenufile[i] << contenufile[i+1] << contenufile[i+2] << contenufile[i+3] ;
        testtxt = scantxt.str();
        std::size_t found = testtxt.find(compp, 0);
        if (found!=std::string::npos)
        {
            contenufile.erase(contenufile.begin()+positionENRS,
                              contenufile.begin()+i);
            break;

        }

        scantxt.str("");
    }

    for ( int i= positionENRS+4 ; i< contenufile.size()-10; i++)
    {
        std::ostringstream scantxt ;
        string testtxt;
        string compp = "EOFC";
        scantxt << contenufile[i] << contenufile[i+1] << contenufile[i+2] << contenufile[i+3] ;
        testtxt = scantxt.str();
        std::size_t found = testtxt.find(compp, 0);
        if (found!=std::string::npos)
        {
            if ( positionEOFC1 )
            {
                positionEOFC2 = i;
                break;
            }
            else  positionEOFC1 = i;


        }

        scantxt.str("");
    }




    /***********************************************inscription position EOFC ********************************/


    int x=0;

    std::ostringstream ossconv;
    string  txt1, txt2,txt3, txt4;

    txt4="0";
    txt3="";
    ossconv.str("");

    if (positionEOFC2<16)
    {
        ossconv.str("");
        ossconv << "0" << DecToHex(positionEOFC2) << "00" ;
    }
    if ( (positionEOFC2>16) && (positionEOFC2<256))

    {
        ossconv.str("");
        ossconv << DecToHex(positionEOFC2) << "00";

    }
    if ( (positionEOFC2>255) && (positionEOFC2<4095))
    {
        ossconv << "0" << DecToHex(positionEOFC2) ;
        txt1 = ossconv.str();
        txt2= txt1.substr(2);
        txt1.erase(2,2);
        ossconv.str("");
        ossconv << txt2 << txt1 ;
    }





    if (positionEOFC2 >4094)
    {
        txt1= DecToHex(positionEOFC2);
        txt2= txt1.substr(2);
        txt1.erase(2,2);
        ossconv.str("");
        ossconv << txt2 << txt1 ;
    }

    contenufile.erase(contenufile.begin()+4,contenufile.begin()+4+2);

    txt4 = ossconv.str();
    txt1=txt4.substr(2);
    txt4.erase(2,2);

    std::istringstream issasc ( txt4 );

    issasc >> std::hex >> x;

    contenufile.insert(contenufile.begin()+4,char (x));

    issasc.str("");
    x=0;
    std::istringstream issasc2 ( txt1 );

    issasc2 >> std::hex >> x;

    contenufile.insert(contenufile.begin()+4+1,char (x));

    issasc2.str("");


    /***********************************************inscription position ENRS ********************************/



    x=0;

    txt4="0";
    txt3="";
    ossconv.str("");


    positionENRS = positionENRS-32;
    if (positionENRS<16)
    {
        ossconv.str("");
        ossconv << "0" << DecToHex(positionENRS) << "00" ;
    }
    if ( (positionENRS>16) && (positionENRS<256))

    {
        ossconv.str("");
        ossconv << DecToHex(positionENRS) << "00";

    }
    if ( (positionENRS>255) && (positionENRS<4095))
    {
        ossconv << "0" << DecToHex(positionENRS) ;
        txt1 = ossconv.str();
        txt2= txt1.substr(2);
        txt1.erase(2,2);
        ossconv.str("");
        ossconv << txt2 << txt1 ;
    }





    if (positionENRS >4094)
    {
        txt1= DecToHex(positionENRS);
        txt2= txt1.substr(2);
        txt1.erase(2,2);
        ossconv.str("");
        ossconv << txt2 << txt1 ;
    }

    contenufile.erase(contenufile.begin()+20,contenufile.begin()+20+2);

    txt4 = ossconv.str();
    txt1=txt4.substr(2);
    txt4.erase(2,2);

    std::istringstream  issasc3 ( txt4 );

    issasc3 >> std::hex >> x;

    contenufile.insert(contenufile.begin()+20,char (x));

    issasc3.str("");
    x=0;
    std::istringstream   issasc4 (txt1 );

    issasc4 >> std::hex >> x;

    contenufile.insert(contenufile.begin()+20+1,char (x));

    issasc4.str("");

    /****************************************************************************************************************/

    cout << "ecriture dans fichier final" << endl;
    ofstream sortiefile(Nomfichiertraduit.c_str(), ios::out | ios::trunc | std::ios_base::binary);

    if(sortiefile)
    {
        for ( int i=0; i< contenufile.size(); i++)
        {
            sortiefile << contenufile[i];
        }
        sortiefile.close();
    }
    else
        cerr << "Erreur à l'ouverture !" << endl;




    return 0;
}