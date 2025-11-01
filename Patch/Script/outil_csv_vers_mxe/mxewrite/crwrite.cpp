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

    string  Adresse;

    string Texte_anglais;
    string Texte_traduit;
    int difftaille;
    int nombreligne;

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
    string Nomfichiersource;
    string  Nomfichiertraduit;
    string Nomfichiercsv;

    int length2;
    char *array2;
    std::vector<char> contenufile;


    Tableajout Laliste[12000];
    std::ostringstream oss;

    int i=0;
    /****************************************************recup nom fichier****************/
    ifstream optionfile("mxe_write.ini", ios::in);

    if(optionfile)
    {
        string contenuoption;

        getline(optionfile, contenuoption);
        oss.str("");
        oss <<   contenuoption << ".mxe";
        Nomfichiersource = oss.str();
        oss.str("");
        oss <<  contenuoption << ".csv";
        Nomfichiercsv = oss.str();
        oss.str("");
        oss  <<  contenuoption << "_new.mxe";
        Nomfichiertraduit = oss.str();
        oss.str("");
    }

    else cout << "probleme ouverture fichier option"  << endl;




    /*******************************lecture csv et rangement dans la struct********/
    ifstream fichier(Nomfichiercsv.c_str(), ios::in);  // on ouvre en lecture

    if(fichier)  // si l'ouverture a fonctionné
    {
        string contenu;  // déclaration d'une chaîne qui contiendra la ligne lue

        int t=0;
        int p =0;
        int q= p;
        while(getline(fichier, contenu))
        {


            // getline(fichier, contenu);  // on met dans "contenu" la ligne
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
                if (t==2) Laliste[i].Chapitre =oss.str();
                if (t==3) Laliste[i].Texte_anglais =oss.str();
                if (t==4) Laliste[i].Texte_traduit =oss.str();
                t++;

                oss.str("");
            }

            i++;
        }

        fichier.close();
    }
    else
        cerr << "Impossible d'ouvrir le fichier !" << endl;


    cout << "lecture fichier source fini" << endl;

    Laliste[0].nombreligne = i;
    oss.str("");
    /**********************************modif ancienne adresse pour refleter le changement de taille entre francais et anglais**********/

    int j = 0;
    for (i=0; i<Laliste[0].nombreligne; i++)
    {
        Laliste[i].difftaille = Laliste[i].Texte_traduit.length() - Laliste[i].Texte_anglais.length();
        if (Laliste[i].difftaille && (  ((string_dec_to_int (Laliste[i+1].Adresse)) != (string_dec_to_int (Laliste[i].Adresse)) )   ))
        {
            for (j=i+1; j< Laliste[0].nombreligne; j++)
            {

                if (  ((string_dec_to_int (Laliste[j].Adresse)) > (string_dec_to_int (Laliste[i].Adresse)) )   )
                {
                    std::istringstream iss( Laliste[j].Adresse );
                    std::ostringstream o2ss;
                    int nombre;
                    iss >> nombre;
                    nombre = nombre + Laliste[i].difftaille  ;
                    o2ss << nombre;
                    Laliste[j].Adresse =  o2ss.str();
                    iss.str("");
                    o2ss.str("");
                    nombre =0;
                }

            }


        }
    }


    cout << "modif adresse suivant diff taille des textes finie" << endl;



    /*************************************************ecriture fichier intermediaire pour verif bonne modif************************
          ofstream save("test.csv", ios::out | ios::trunc);  //déclaration du flux et ouverture du fichier

            if(save)  // si l'ouverture a réussi
            {
                for ( int i=0; i< Laliste[0].nombreligne; i++)
     {
         save << Laliste[i].Adresse_de_l_adresse << ";" << Laliste[i].Adresse << ";" << Laliste[i].Texte_traduit.length() << ";" << Laliste[i].Texte_anglais.length() << ";"  <<  Laliste[i].difftaille << ";" << Laliste[i].Texte_anglais << ";" << Laliste[i].Texte_traduit << endl;
     }
                    save.close();  // on referme le fichier
            }
            else  // sinon
                    cerr << "Erreur à l'ouverture !" << endl;

     cout << "save fichier intermediaire ok";

    */



    /***************************************on remplie un vector avec le fichier source***********************************************/
    std::ostringstream ossconv;

    std::ifstream lecture;
    lecture.open(Nomfichiersource.c_str(), std::ios_base::binary);

    if (lecture)
    {
        length2 = lecture.rdbuf()->pubseekoff(0, std::ios_base::end);
        //	cout << length2 << endl;
        array2 = new char[length2];

        lecture.rdbuf()->pubseekoff(0, std::ios_base::beg);
        lecture.read(array2, length2);


        lecture.close();
    }
    else cout << "probleme ouverture fichier"  << endl;

    contenufile.clear();
    for(int i(0); i<length2; i++)        contenufile.push_back(array2[i]) ;


    /*****************************************modif des adresses du nouveau texte***********************************************/

    cout << "debut modif contenu fichier destination" << endl;


    for (int j=0; j < Laliste[0].nombreligne ; j++ )
    {
        std::istringstream iss( Laliste[j].Adresse_de_l_adresse );

        string  txt1, txt2,txt3, txt4;
        int nombre;
        int x, o ;
        iss >> nombre;

        txt1 = string_dec_to_hex(Laliste[j].Adresse);
        txt2 = "";
        txt2.insert(0, txt1);
        while (txt2.size() < 8) txt2.insert(0, "0");

        contenufile.erase(contenufile.begin()+nombre,contenufile.begin()+nombre+4);
        o=0;
        for ( int y=0; y <8; y=y+2)
        {
            // conversion en hexa puis ecriture
            txt3="";
            txt3.insert(0, txt2,y,2 ) ;
            std::istringstream issasc ( txt3 );
            issasc >> std::hex >> x;
            contenufile.insert(contenufile.begin()+nombre+o, char (x));
            issasc.str("");
            o++;
        }




    }


    cout << "fin ecriture des nouvelles adresse" << endl;




    cout << "debut ecriture texte traduit" << endl;


    /*******************************ecriture zone de 0 dans vector pour separer le nouveau contenu de l'ancien***************************/
    int espaceprevu =0;
    for (int i=0; i<Laliste[0].nombreligne; i++)  espaceprevu = espaceprevu + Laliste[i].difftaille;
    cout << "espaceprevu" <<   espaceprevu  << "  nombre lignes:" << Laliste[0].nombreligne <<endl;
    for ( int i= string_dec_to_int (Laliste[0].Adresse)+32 ; i< string_dec_to_int (Laliste[0].Adresse)+espaceprevu; i++) contenufile.insert(contenufile.begin()+i ,  char (0));



    /*******************************ecriture texte dans vector *****************************************************/

    for (int k=0; k< Laliste[0].nombreligne; k++)
    {
        int addprec;

        std::istringstream issint (Laliste[k].Adresse);
        int nombre;
        issint >> nombre;
        //   cout <<(Laliste[k].Adresse) << endl;
        int temp = nombre;
        if (k==0) addprec= nombre;
        //cout << "i est " << i << " et temp est " << temp << endl;
        if ((k==0)|| (addprec< nombre))
        {

            for ( int l = 0; l <  Laliste[k].Texte_traduit.length() ; l++ ) contenufile.insert(contenufile.begin()+temp+32+l , Laliste[k].Texte_traduit[l]);



            contenufile.insert(contenufile.begin()+temp+32+Laliste[k].Texte_traduit.length() , char (0));

        }
        addprec= nombre;


    }

    /*******************************ecriture vector dans new mxe *****************************************************/
    cout << "ecriture dans fichier final" << endl;
    ofstream sortiefile(Nomfichiertraduit.c_str(), ios::out | ios::trunc | std::ios_base::binary);

    if(sortiefile)  // si l'ouverture a réussi
    {
        for ( int i=0; i< contenufile.size(); i++)
        {
            sortiefile << contenufile[i];
        }
        sortiefile.close();  // on referme le fichier
    }
    else  // sinon
        cerr << "Erreur à l'ouverture !" << endl;

    /***********************************************************************************************************/






    return 0;
}
