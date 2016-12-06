#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

/* Viikkotehtävä 4
    Joonas soudunsaari, opiskelija numero: 2374682

    Olin tehnyt viikkotehtävä kolmosessa pankkiautomaattini omien speksieni mukaan valmiiksi. Tehtävä neljä vaikutti odottavan
    että kolmosessa tehtävä olisi jätetty kesken. Olin hiukan epävarma kuinka jatkaa joten lisäsin pankkiautomaattiin
    ominaisuuksia alkuperäisten speksieni ulkopuolelta. Nyt automaattilla pystyy myös suorittamaan maksuja, mikä ei ehkä ole
    oikeiden automaattien ominaisuus mutta sopi minusta tähän. Lisäksi automaatti tallentaa tilin saldon sekä kaikki maksut.
    Tästä seuraa että automaatti olettaa käyttäjän olevan aina sama. Tämä on seurausta siitä että ei ole oikeita kortteja
    joilla erotella käyttäjät.
    Lisäksi vaihdoin rahan tietotyypin integeristä doubleksi että saadaan sentitkin laskettua.
*/

void tiliTapahtuma();
double otto();
double talletus();
int kysyToiminto();
int odotaKortti();
int tilitiedot();

//uudet funktiot alla
int maksu();
void printTilitapahtumat();
void tallennaSaldo(double);
double checkSaldo();
int historia();

//rahan jako funktio
int* jaaRahaa(int);


int main()
{
    int kortti;
    srand(time(NULL));  //alustaa randin aijan mukaan
    while(1){
        kortti = odotaKortti();
        if (kortti == 1){           //jos kortti syötetty koneeseen
            tiliTapahtuma();
        }
        else{               //ei korttia, sulje kone
            break;
        }
    }
    printf("Automaatti sulkeutuu");
    return 0;
}

void tiliTapahtuma(){
    int turha;
    printf("Salasana pls: ");
    scanf("%d", &turha); // Ei ole pankkitietoja joihin verrata eika pankkiautomaatit ruukaa antaa saataa tilitietoja
    fseek(stdin,0,SEEK_END);
    int tapahtuma = kysyToiminto();     //mitä tehdään tilille
    double rahaa;
    switch(tapahtuma){
        case 1:
            rahaa = otto();
            printf("Ota rahat\n");
            break;
        case 2:
            rahaa = talletus();
            break;
        case 3:
            rahaa = maksu();
            break;
    }
    fseek(stdin,0,SEEK_END);
    int tiedot = tilitiedot();      //tilillä rahaa
    switch(tiedot){
        case 1:
            printf("Tilillasi on %lf euroa\n", rahaa);
            break;
        case 2:
            break;
    }
    tiedot = historia();
    switch(tiedot){
    case 1:
        printTilitapahtumat();
        printf("\n");
        break;
    case 2:
        break;
    }
    printf("Kiitos asioinnista, muista ottaa korttisi.\n\n");

}

double otto(){
    double summa = checkSaldo();    //tarkastaa onko korttia käytetty aiemmin, jos ei niin luodaan satunnainen summa rahaa tilille
    double temp = 0;
    if (summa < 0){
        summa = rand() % 10000;//tilillä alustetaan satunnainen summa rahaa koska ei ole tietokantaa eikä automaatti oikein voi kysyä asiakkaalta.
    }
    while(1){
        printf("Paljonko nostat: ");
        fseek(stdin,0,SEEK_END);
        scanf("%lf", &temp);
        if (temp > 0 && temp <= summa && ((int)temp%10 == 0 && (temp == 20 || temp == 40 || temp >= 50))){      //hiukan räjähti lause mutta toimii
            summa -= temp;
            int *palautus;
            palautus = jaaRahaa(temp);
            printf("saat %d 50 ja %d 20 setelia\n", palautus[1], palautus[0]);
            tallennaSaldo(summa);
            return summa;
        }
        else if(temp > summa){
            printf("Sinulla ei ole nain paljon rahaa. Rahaa sinulla on %lf euroa\n", summa);
            temp = 0;
        }
        else{
            printf("Ei hyva otto. Voit nostaa vain kahden ja viidenkympin seteleita\n");
        }
    }
}

double talletus(){

    double summa = checkSaldo();    //tarkastaa onko korttia käytetty aiemmin, jos ei niin luodaan satunnainen summa rahaa tilille
    double talletus = 0;
    if (summa < 0){
        summa = rand() % 10000;//tilillä alustetaan satunnainen summa rahaa koska ei ole tietokantaa eikä automaatti oikein voi kysyä asiakkaalta.
    }
    while(1){
        printf("Paljonko talletat: ");
        fseek(stdin,0,SEEK_END);
        scanf("%lf", &talletus);
        if (talletus > 0){
            summa += talletus;
            tallennaSaldo(summa);
            return summa;
        }
        else{
            printf("ei hyvaksyttava Summa\n");
        }
    }
}

int maksu(){
    double rahaa = checkSaldo();
    double temp = 0;
    int maksettu = 0;
    if (rahaa < 0){
        rahaa = rand() % 10000;
    }

    while(1){
        printf("Paljonko maksat: ");
        fseek(stdin,0,SEEK_END);
        scanf("%lf", &temp);
        if (temp > 0 && temp <= rahaa){
            rahaa -= temp;
            maksettu = 1;
            break;
        }
        else if(temp > rahaa){
            printf("Sinulla ei ole nain paljon rahaa. Maksua ei suoriteta\n");
            temp = 0;
            break;
        }
        else{
            printf("Ei hyva maksu. Yritä uudestaan\n");
        }
    }
    if (maksettu){
        int c;
        while ((c = getchar()) != EOF && c != '\n');
        printf("Kelle maksetaan? ");
        char input[255];
        while(1){
            fseek(stdin,0,SEEK_END);
            fgets(input, 255, stdin);
            input[strlen(input) -1] = '\0';
            if (strlen(input) ==253){
                printf("Liian pitka string \n");
                fseek(stdin,0,SEEK_END);}
            else{
                break;
            }
        }
        time_t curtime;
        time(&curtime);
        FILE *tiedosto = fopen("maksut.txt", "a");
        fprintf(tiedosto, "%s\n\tMaksettu: %lf\n\t%s\n\n", ctime(&curtime), temp, input);
        fclose(tiedosto);
        tallennaSaldo(rahaa);
        return rahaa;
    }
    else{
        tallennaSaldo(rahaa);
        return -1;
    }
}

void printTilitapahtumat(){
    if (access("maksut.txt", F_OK ) != -1){
        printf("\n");
        FILE * tiedosto = fopen("maksut.txt", "r");
        int c;
        while(1){
            c = fgetc(tiedosto);
            if( feof(tiedosto) ){
                break ;
            }
            printf("%c", c);
        }
        fclose(tiedosto);
    }
    else{
        printf("Ei tilihistoriaa!");
    }
}

void tallennaSaldo(double summa){
    FILE * tiedosto = fopen("Saldo.txt", "w");
    fprintf(tiedosto, "%lf", summa);
    fclose(tiedosto);
}
double checkSaldo(){
    if (access("Saldo.txt", F_OK ) != -1){
        FILE * tiedosto = fopen("Saldo.txt", "r");
        double summa;
        while(1){
            fscanf (tiedosto, "%lf", &summa);
            if(feof(tiedosto)){
                break ;
            }
        }
        fclose(tiedosto);
        return summa;
    }
    else{
        return -1;
    }


}



int kysyToiminto(){
    int toimi;
    while(1){
        printf("Nosto, paina 1:\nTalletus, paina 2\nMaksu, paina 3\n");
        scanf("%d", &toimi);
        if (toimi == 1 || toimi == 2 || toimi == 3){
            switch(toimi){
            case 1:
                return 1;
            case 2:
                return 2;
            case 3:
                return 3;
            }
        }
        else{
            printf("Vaara syote\n");
            fseek(stdin,0,SEEK_END);
        }

    }

}

int odotaKortti(){          //odottaa kunnes koneeseen on syötetty kortti tai se suljetaan
    char input[20];
    printf("Syota kortti Y/N: ");
    while(1){
        fgets(input, 20, stdin);
        input[strlen(input) -1] = '\0';
        if (strlen(input) ==18){
            printf("Liian pitka string \n");
            fseek(stdin,0,SEEK_END);}
        if(strcmp(input, "Y") == 0){
            return 1;}
        else if (strcmp(input, "N") == 0){
            return 0;}
        else{
            printf("Syotteen on oltava Y tai N: ");
        }
    }
}

int tilitiedot(){       //kysyy printataanko tilitiedot.
    char input[20];
    printf("Tahdotko tilitiedot? Y/N: ");
    while(1){
        fgets(input, 20, stdin);
        input[strlen(input) -1] = '\0';
        if (strlen(input) ==18){
            printf("Liian pitka string \n");
            fseek(stdin,0,SEEK_END);}
        if(strcmp(input, "Y") == 0){
            return 1;}
        else if (strcmp(input, "N") == 0){
            return 0;}
        else{
            printf("Syotteen on oltava Y tai N: ");
        }
    }
}

int historia(){       //kysyy printataanko tilitiedot.
    char input[20];
    printf("Tahdotko tilihistorian? Y/N: ");
    while(1){
        fgets(input, 20, stdin);
        input[strlen(input) -1] = '\0';
        if (strlen(input) ==18){
            printf("Liian pitka string \n");
            fseek(stdin,0,SEEK_END);}
        if(strcmp(input, "Y") == 0){
            return 1;}
        else if (strcmp(input, "N") == 0){
            return 0;}
        else{
            printf("Syotteen on oltava Y tai N: ");
        }
    }
}
int* jaaRahaa(int summa){

    int *palautus = malloc(sizeof(int[2]));
    if ((summa%10) == 0 ){;                 //hyväksyy vain kympillä jaelliset luvut
        if (summa == 20 || summa == 40 || summa >= 50){ //hyväksyy vain luvut jotka voidaan jakaa 50 ja kahdenkympin seteleihin
            double temp;
            int temp2;
            temp = (double)summa/50.0;

            int temp3;
            temp3 = temp;       //muunnos inttiin jotta reaaliluku pyöristyisi alaspäin.
            temp2 = summa-temp3*50;     //paljon jää yli

            switch(temp2){          //määritellään setelien lukumäärät

            case 0:{
                palautus[0] = 0;
                palautus[1] = temp3;
                return palautus;
            }
            case 10:{
                palautus[0] = 3;
                palautus[1] = temp3-1;
                return palautus;
            }
            case 20:{
                palautus[0] = 1;
                palautus[1] = temp3;
                return palautus;
            }
            case 30:{
                palautus[0] = 4;
                palautus[1] = temp-1;
                return palautus;
            }
            case 40:{
                palautus[0] = 2;
                palautus[1] = temp;
                return palautus;
            }
            }
        }
    }


}
