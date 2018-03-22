#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdlib>

using namespace std;

void afficherImageEnBrute(unsigned char *image, int nbLignes, int nbColonnes)
{
    int indice = 0;
    for (int i = 0; i < nbLignes; i++)
    {
        for (int j = 0; j < nbColonnes; j++)
        {
            cout << (int)image[indice++] << " |";
        }
        cout << endl;
    }
}

bool lirePGM(string nomFichier, int &nbLignes, int &nbColonnes, unsigned char *&image)
{
    ifstream fichier(nomFichier.c_str(), ios::binary);
    if (fichier == NULL)
        return false;
    string extension;
    fichier >> extension;
    if (extension == "P5" || extension == "p5") // rajoute P3 quand t'as le temps
    {
        fichier >> nbLignes >> nbColonnes;
        int valMax;
        fichier >> valMax;
        image = new unsigned char[nbLignes*nbColonnes];
        int indice = 0;
        while (fichier)
        {
            fichier.read(reinterpret_cast<char*>(image+(indice++)), 1);
        }
        fichier.close();
    }
    return true;
}

void ecrirePGM(string nomFichier, int nbLignes, int nbColonnes, unsigned char *image)
{
    ofstream fichier(nomFichier.c_str());
    fichier << "P5" << endl << nbLignes << " " << nbColonnes << " " << 255;
    int nbElements = nbLignes*nbColonnes;
    for (int i = 0; i <= nbElements; i++)
        fichier.write(reinterpret_cast<char*>(image+i), 1);
        fichier.close();
}

double gradient(unsigned char *image, int indice, int nbLignes, int nbColonnes, string methode)
{
    if (methode == "roberts")
    {
        int x = (int)image[indice]-(int)image[indice+nbColonnes+1];
        int y = (int)image[indice+1]-(int)image[indice+nbColonnes-1];
        double grad = sqrt(pow(x,2)+pow(y,2));
        return grad;
    }
}

void calculerContours(unsigned char *image, int nbLignes, int nbColonnes, string methode, unsigned char *&retour)
{
    int seuil = 50;
    int nbElements = nbLignes*nbColonnes;
    retour = new unsigned char[nbElements*10]; // POURQUOI CA MARCHE PAS AVEC NBELEMENTS ????????
    for (int i = 0; i <= nbElements; i++)
    {
        retour[i] = (unsigned char)0;
    }
    int maxIt = nbElements-nbColonnes-1;
    for (int i = 1; i <= maxIt; i++)
    {
        if (gradient(image, i, nbLignes, nbColonnes, methode) > seuil)
            retour[i] = (unsigned char)255;
    }
}

void contours(string nomFichier, string methode)
{
    int nbLignes, nbColonnes;
    unsigned char *image;
    unsigned char *contours;
    lirePGM(nomFichier+".pgm", nbLignes, nbColonnes, image);
    cout << "Nombre lignes = " << nbLignes << " et de colonnes = " << nbColonnes << endl;
    calculerContours(image, nbLignes, nbColonnes, methode, contours);
    ecrirePGM(nomFichier+"contours.pgm", nbLignes, nbColonnes, contours);
    cout << nomFichier+"contours.pgm" << " cree." << endl;
}

int main()
{
    string nomFichier, nomMethode;
    cout << "Entrez le nom du fichier PGM (sans l'extension) : " << endl;
    cin >> nomFichier;
    cout << endl;
    cout << "Entrez le nom de la methode (roberts, ...) : " << endl;
    cin >> nomMethode;
    cout << endl;
    contours(nomFichier, nomMethode);
    return 0;
}
