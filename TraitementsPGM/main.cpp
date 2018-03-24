#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

const int niveauGrisMax = 255;

void lirePGM(string nomFichier, int &nbLignes, int &nbColonnes, unsigned char *&image)
{
    ifstream fichier(nomFichier.c_str(), ios::binary);
    if (fichier)
    {
    	string extension;
	    fichier >> extension;
	    if (extension == "P5" || extension == "p5") // ne gere pas P3 pour le moment
	    {
	        fichier >> nbColonnes >> nbLignes;
	        int valMax;
	        fichier >> valMax;
	        int nbElements = nbLignes*nbColonnes;
	        image = new unsigned char[nbElements];
	        for (int i = 0; i < nbElements; i++)
            {
                image[i] = (unsigned char)0;
            }
	        int indice = 0;
	        while (fichier)
	        {
	            char tmp;
                fichier.read(&tmp, 1);
                image[indice++] = (unsigned char)tmp;
	        }
        }
        cout << "Fichier " << nomFichier << " lu correctement." << endl;
	}
	else
	{
	    cout << "Fichier introuvable." << endl;
        image = nullptr;
	}
}

void ecrirePGM(string nomFichier, int nbLignes, int nbColonnes, unsigned char *image)
{
    ofstream fichier(nomFichier.c_str());
    fichier << "P5" << endl << nbColonnes << " " << nbLignes << endl << 255 << endl;
    int nbElements = nbLignes*nbColonnes;
    for (int i = 0; i < nbElements; i++)
    {
        fichier.write(reinterpret_cast<char*>(image+i), 1);
    }
    cout << endl;
}

void calculerHistogramme(unsigned char *image, int nbLignes, int nbColonnes, int *&retour, int &maxVal)
{
    maxVal = 0;
    int nbElements = nbLignes*nbColonnes;
    for (int i = 0; i <= niveauGrisMax; i++)
    {
        retour[i] = 0;
    }
    for (int i = 0; i < nbElements; i++)
    {
        retour[(int)image[i]]++;
        if (maxVal < retour[(int)image[i]])
            maxVal = retour[(int)image[i]];
    }
}

void creerHistogramme(int *histogramme, int maxVal, unsigned char *&retour)
{
    int taille = (niveauGrisMax+1)*(niveauGrisMax+1);
    for (int i = 0; i < taille; i++)
    {
        retour[i] = (unsigned char)0;
    }
    for (int i = 0; i <= niveauGrisMax; i++)
    {
        int valeur = histogramme[i]*(niveauGrisMax)/maxVal;
        for (int j = 0; j < valeur; j++)
        {
            int nbl = ((niveauGrisMax+1)-j)*(niveauGrisMax+1);
            int nbc = i+1;
            retour[nbc+nbl] = (unsigned char)255;
        }
    }
}

/* Aide pour calculer le gradient :
[indice-nbColonnes-1]   [indice-nbColonnes]     [indice-nbColonnes+1]
[indice-1]              [indice]                [indice+1]
[indice+nbColonnes-1]   [indice+nbColonnes]     [indice+nbColonnes+1]
*/

double gradient(unsigned char *image, int indice, int nbLignes, int nbColonnes, string methode)
{
    if (methode == "roberts")
    {
        int x = (int)image[indice]-(int)image[indice+nbColonnes+1];
        int y = (int)image[indice+1]-(int)image[indice+nbColonnes];
        double grad = sqrt(pow(x,2)+pow(y,2));
        return grad;
    }
    else
    {
        int x = (int)image[indice]-(int)image[indice+nbColonnes+1];
        int y = (int)image[indice+1]-(int)image[indice+nbColonnes];
        double grad = sqrt(pow(x,2)+pow(y,2));
        return grad;
    }
}

void creerContours(unsigned char *image, int nbLignes, int nbColonnes, string methode, unsigned char *&retour)
{
    int seuil = 30;
    int nbElements = nbLignes*nbColonnes;
    cout << "Allocation memoire..." << endl;
    retour = new unsigned char[nbElements*15]; // POURQUOI CA MARCHE PAS AVEC NBELEMENTS ????????
    cout << "Allocation memoire terminee." << endl;
    for (int i = 0; i < nbElements; i++)
    {
        retour[i] = (unsigned char)0;
    }
    int maxIt = nbElements-nbColonnes-1;
    for (int i = 1; i < maxIt; i++)
    {
        if (gradient(image, i, nbLignes, nbColonnes, methode) > seuil)
            retour[i] = (unsigned char)255;
    }
}

void contours()
{
    string nomFichier, nomMethode;
    int nbLignes, nbColonnes;
    cout << "Entrez le nom du fichier PGM (sans l'extension) : " << endl;
    cin >> nomFichier;
    cout << endl;
    unsigned char *image;
    lirePGM(nomFichier+".pgm", nbLignes, nbColonnes, image);
    if (image != nullptr)
    {
        cout << endl << "Entrez le nom de la methode (roberts, ...) : " << endl;
        cin >> nomMethode;
        cout << endl;
        unsigned char *contours;
        cout << "Nombre de lignes = " << nbLignes << " et de colonnes = " << nbColonnes << "." << endl;
        creerContours(image, nbLignes, nbColonnes, nomMethode, contours);
        ecrirePGM(nomFichier+"_contours.pgm", nbLignes, nbColonnes, contours);
        cout << nomFichier+"_contours.pgm" << " cree." << endl;
    }
}

void histogramme()
{
    string nomFichier;
    int nbLignes, nbColonnes, maxVal;
    cout << "Entrez le nom du fichier PGM (sans l'extension) : " << endl;
    cin >> nomFichier;
    cout << endl;
    unsigned char *image;
    lirePGM(nomFichier+".pgm", nbLignes, nbColonnes, image);
    if (image != nullptr)
    {
        int *histo = new int[niveauGrisMax+1];
        unsigned char *histoImage = new unsigned char[niveauGrisMax*niveauGrisMax*15]; // meme soucis
        calculerHistogramme(image, nbLignes, nbColonnes, histo, maxVal);
        creerHistogramme(histo, maxVal, histoImage);
        ecrirePGM(nomFichier+"_histo.pgm", niveauGrisMax+1, niveauGrisMax+1, histoImage);
        cout << nomFichier+"_histo.pgm" << " cree." << endl;
    }
}

int main()
{
    //contours();
    histogramme();
    return 0;
}
