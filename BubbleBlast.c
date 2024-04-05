/****************************************************************************************************
 *   BubbleBlast GAME devoloped by Gabriel Pedranzini                                               *
 ****************************************************************************************************/

// Inclusione librerie
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>    //libreria per operare con valori booleani (vero/falso)
#include <time.h>       //libreria per l'uso della funzione di random
#include <string.h>     //libreria per gestione delle stringhe
#include <ctype.h>      //libreria per gestione del tipo della variabile

// Definizione costanti
#define ROW 5           //numero di righe della matrice di gioco
#define COL 6           //numero di colonne della matrice di gioco
#define NOME_FILE "giocatori.txt"
#define MAX_LENGTH 100

// Definizione di quale comando usare per pulire il prompt in base al sistema operativo
#ifdef _WIN32
    #define CLEAR_COMMAND "cls"     // Comando per Windows
#else
    #define CLEAR_COMMAND "clear"   // Comando per sistemi non-Windows (presumibilmente UNIX-like come Linux)
#endif

// Definizione struttura Giocatore
typedef struct {
    char *nome;                     //Contiene il nome del giocatore
    int partite_giocate;            //Contiene il totale di partite giocate
    int partite_vinte;              //Contiene il totale di partite vinte
    int miglior_sequenza;           //Contiene la miglior seguenza di partite vinte di fila
    int punteggio;                  //Contiene il miglior punteggio
    char *data_ultima_partita;      //Contiene la data dell'ultima partita
    int matrice_partita[ROW][COL];  //Contiene la matrice della partita in corso
    int max_mosse;                  //Contiene il numero delle mosse massime della partita in corso
    int mosse_eff;                  //Contiene il numero delle mosse effettuate della partita in corso
    int diff_partita;               //Contiene la difficoltà della partita in corso
} Giocatore;    

// Definizione variabile globale mosse massime, che verrà valorizzata con le mosse minime per risolvere la matrice
int maxMosse = 100;

/****************************************************************************************************
 *              Gestione inseremento e calcolo del minimo della matrice di gioco                    *
 ****************************************************************************************************/
 
// Inserimento randomico nella matrice di gioco delle bolle da 0 a 3
void inserisciBolle(int matrice[ROW][COL]);

// Calcolo delle mosse minime della matrice di gioco
void calcolaNumeroMosse(int matrice[ROW][COL], int mosse);

/****************************************************************************************************
 *                     Gestione esplosioni della matrice di gioco                                   *
 ****************************************************************************************************/
 
// Gestione dell'esplosioni della matrice di gioco
void esplosione(int matrice[ROW][COL], int row, int col);
void esplosioneSu(int matrice[ROW][COL], int row, int col);
void esplosioneGiu(int matrice[ROW][COL], int row, int col);
void esplosioneDestra(int matrice[ROW][COL], int row, int col);
void esplosioneSinistra(int matrice[ROW][COL], int row, int col);

/****************************************************************************************************
 *                                Gestione modalita' GIOCA                                          *
 ****************************************************************************************************/
 
// Gestisce la modalità GIOCA in base agli input dell'utente
int partita(Giocatore *giocatore);

/****************************************************************************************************
 *                                Gestioni varie matrice                                            *
 ****************************************************************************************************/
 
// Copia della matrice in una nuova matrice
void copiamatrice(int *matriceCopia, int *matriceOrigine);

// Verifica se la matrice contieni tutti 0 (le bolle sono tutte esplose)
bool controllaVincita(int matrice[ROW][COL]);

/****************************************************************************************************
 *                                    Gestioni Giocatore                                            *
 ****************************************************************************************************/
 
// Valorizza la struttura Giocatore con tutti valori di default ad eccezione del nome
Giocatore valorizzaGiocatore(char *user);

// Verifica la presenza di un giocatore con lo stesso nome nel file definito in NOME_FILE ed eventualmente
// carica i dati nella struttura Giocatore
Giocatore leggiGiocatoriDaFile(const char *nomeFile, char *user);

// Scrive su file il nuovo Giocatore (solo se inizia una partita) o aggiorna i dati del giocatore già presente
void scriviGiocatoreSuFile(const char *nomeFile, Giocatore giocatore);

// Trasforma la matrice del Giocatore in una stringa da scrivere e salvare su file
char *matrice_toString(int matrice[ROW][COL]);

// Ritorna la data corrente, viene utilizzata per valorizzare la data dell'ultima partita del giocatore
char *ottieni_data();

// Aggiunge o rimuove mosse alle mosse massime in base:
// alle mosse massime (se presente), alla difficolta precedente (se presente) e alla difficolta nuova
int impostaDifficolta(int max_mosse, int difficolta_old, int difficolta_new);

/****************************************************************************************************
 *                                       FUNZIONI DI STAMPA                                         *
 ****************************************************************************************************/

// Stampa la matrice della partita a video
void stampamatrice(int stampa[ROW][COL]);

// Aggiunge alla matrice della partita le informazioni di mosse massime, mosse effettuate e difficoltà
void stampamatriceUtente(int matrice[ROW][COL], int max_mosse, int mosse, int difficolta);

// Stampa a video le statistiche del Giocatore
void stampaStatistiche(Giocatore giocatore);

// Stampa a video e chiede all'utente la difficoltà da impostare al gioco
int stampaDifficolta(int diff);

// Stampa a video le regole del gioco
void stampaRegole();

//Pulisce il prompt dei comandi in base al comando definito in CLEAR_COMMAND
void clear();

// Verifica che l'input passato sia di un INT
bool isInteger(const char *str);


/****************************************************************************************************
 *                                     MAIN gestione menù                                           *
 ****************************************************************************************************/

int main() {

    // Definizione variabili

    // Input
    char nome_giocatore[MAX_LENGTH] = "";
    char opzione = ' ';

    // Variabili per gestione giocatore
    int seq_partite = 0;    //Contiene la sequenza delle partite vinte di fila
    int punteggio = 0;          //Contiene il punteggio
    int diff = 0;           //Contiene la difficolta precedente

    // Stampe a video
    clear();
    printf("\tB U B B L E\t B L A S T\n");
    printf("\n Benvenuto!\n Questo e' Bubble Blast!\n");
    stampaRegole();
    printf("\n\n Dammi il tuo nome: ");
    scanf(" %s",nome_giocatore);    //Richiesta nome giocatore da valorizzare
    getchar();

    // Valorizzazione della struttura giocatore in base alla presenza o meno del nome nel file
    Giocatore giocatore = leggiGiocatoriDaFile(NOME_FILE, nome_giocatore);

    /*
     *  Main controller effettua diverse operazioni in base all'input inserito nella variabile OPZIONE:
     *      G richiama la modalità di gioco
     *      S mostra a video le statistiche del giocatore
     *      D imposta/cambia la difficoltà del giocatore
     *      N permette di cambiare il giocatore
     *      R stampa a video le regole
     *      E termina il programma
     */
    do {
        // Stampe a video
        clear();
        printf("\tB U B B L E\t B L A S T\n");
        printf("\n******************************************************************************************");
        printf("\n Benvenuto nel MENU' %s (seleziona un'opzione) \n\n  G gioca \tS statistiche \t D difficolta' \t N giocatore \t R regole \t E esci\n",giocatore.nome);
        printf("\n******************************************************************************************\n");
        scanf(" %c",&opzione);
        getchar();
        opzione = toupper(opzione);

        // Se l'opzione è uguale a G entra nel ciclo modalità gioco
        while(opzione == 'G') {
            // Stampe a video
            clear();
            printf("\tB U B B L E\t B L A S T\n");

            char riprendi = ' ';

            // Verifica la presenza di una partita in corso non completata
            if(giocatore.max_mosse != 0 && giocatore.mosse_eff < giocatore.max_mosse) { //il controllo va fatto sulla matrice e non sul maxMosse

                // Verifica se l'utente ha cambiato la difficoltà e quindi non ha più mosse disponibili
                // Stampe a video
                if(giocatore.max_mosse > giocatore.mosse_eff) {
                    printf("\n E' stata trovata una partita in data %s con il nome: %s ",giocatore.data_ultima_partita,giocatore.nome);
                    do {
                        printf("\n Vuoi riprendere la vecchia partita? (Y o N) ");
                        scanf(" %c",&riprendi);
                        getchar();
                        riprendi = toupper(riprendi);
                    }while(riprendi != 'Y' && riprendi != 'N');
                }
            }

            // Verifica se si è deciso di riprendere o meno la partita in corso
            if(riprendi != 'Y') {
                // Valorizza la matrice del giocatore con bolle in stato random da 0 a 3
                inserisciBolle(giocatore.matrice_partita);

                // Copia la matrice per calcolare le mosse minime
                int matriceCopiata[ROW][COL];
                copiamatrice( (int *)matriceCopiata, (int *)giocatore.matrice_partita);
                printf("\n Calcolo mossse minime in corso...");

                // Calcolo delle mosse minime che verranno valorizza nella variabile globale
                maxMosse = 100;
                calcolaNumeroMosse(matriceCopiata, 0);
                // Verifica se il giocatore ha una difficolta' impostata
                while(giocatore.diff_partita != 1 && giocatore.diff_partita != 2 && giocatore.diff_partita != 3) {
                    // Stampa a video
                    clear();
                    printf("\tB U B B L E\t B L A S T\n");
                    giocatore.diff_partita = stampaDifficolta(0);
                }
                
                // Aggiunge o sottrae mosse alle mosse minime in base alla difficoltà impostata
                giocatore.max_mosse = maxMosse + impostaDifficolta(0,diff,giocatore.diff_partita);
                // Imposta al giocatore le mosse effettuate a 0
                giocatore.mosse_eff = 0;
            }

            // Stampa a video
            clear();
            printf("\tB U B B L E\t B L A S T\n");
            // Richiama la modalita' di gioco che ritorna:                
            int flg_return = partita(&giocatore);
            
            if(flg_return == -1) { // -1 uscita dalla partita dopo aver inserito E
                opzione = ' '; // imposta l'opzione scelta a space
            }
            else {
                if(flg_return == 1) { // 1 partita vinta
                    // Incrementa i record del giocatore
                    giocatore.partite_vinte++;
                    seq_partite++;
                    punteggio += seq_partite*10*giocatore.diff_partita;
                    printf("\n Punteggio attuale: %d \tSequenza di partite vinte in corso: %d ",punteggio,seq_partite);
                }
                else { // 0 partita persa
                    // Imposta a 0 i record in corso del giocatore
                    printf("\n Punteggio: %d \tSequenza di partite vinte: %d ",punteggio,seq_partite);
                    seq_partite = 0;
                    punteggio = 0;
                }
                
                // Verifica se i record in corso siano maggiori dei record precedenti ed eventualmente li sovrascrive
                if (seq_partite > giocatore.miglior_sequenza) giocatore.miglior_sequenza = seq_partite;
                if (punteggio > giocatore.punteggio) giocatore.punteggio = punteggio;
                // Setta i vari record del giocatore
                giocatore.partite_giocate++;
                giocatore.data_ultima_partita = ottieni_data();
                giocatore.max_mosse = 0;
                giocatore.mosse_eff = 0;
                // Salva su file le nuove informazioni
                scriviGiocatoreSuFile("giocatori.txt", giocatore);
                
                //Stampa a video la richiesta di continuare a giocare oppure di tornare al menu
                printf("\n******************************************************************************************\n");
                printf("\n\tG continua a giocare \tE torna al menu'\n");
                do {
                    scanf(" %c",&opzione);
                    opzione = toupper(opzione);
                    getchar();
                }while(opzione != 'G' && opzione != 'E');
                if(opzione == 'E') opzione = ' '; //Evita che finisca completamente il programma
            }
        }
        // Se l'opzione è uguale a S stampa a video le statische del giocatore 
        if(opzione == 'S') {
            clear();
            printf("\tB U B B L E\t B L A S T\n");
            stampaStatistiche(giocatore);
        }
        // Se l'opzione è uguale a D stampa a video la possibilità di cambiare la difficolta'
        if(opzione == 'D') {
            clear();
            printf("\tB U B B L E\t B L A S T\n");
            // Imposta la difficolta' attuale in una variabile temporanea 
            int temp_diff = giocatore.diff_partita;
            // Imposta la nuova difficolta' al giocatore
            giocatore.diff_partita = stampaDifficolta(giocatore.diff_partita);
            // Verifica che la nuova difficolta' sia diversa da quella vecchia e la salva nella variabile diff
            if(temp_diff != giocatore.diff_partita) {
                diff = temp_diff;
                giocatore.max_mosse = giocatore.max_mosse + impostaDifficolta(giocatore.max_mosse,diff,giocatore.diff_partita);
                diff = giocatore.diff_partita;
            }

        }
        // Se l'opzione è uguale a N stampa a video la possibilita' di cambiare giocatore
        if(opzione == 'N') {
            // Stampe a video
            clear();
            printf("\tB U B B L E\t B L A S T\n");
            printf("\n******************************************************************************************\n");
            printf(" CAMBIA GIOCATORE");
            printf("\n\n  Inserisci il nome del giocatore: ");
            scanf(" %s",nome_giocatore);

            Giocatore giocatore1;
            if(strcmp(nome_giocatore,giocatore.nome) == 0) giocatore1 = giocatore;
            else giocatore1 = leggiGiocatoriDaFile("giocatori.txt", nome_giocatore); // Cerca il giocatore nel file se non lo trova lo valorizza default

            // Stampe a video
            printf("\n  E' stato impostato il giocatore: %s",giocatore1.nome);
            printf("\n******************************************************************************************\n");
            printf(" Seleziona Y per confermare o N per annullare\n");
            char opzione1 = ' ';
            do {
                scanf(" %c",&opzione1);
                opzione1 = toupper(opzione1);
            }while (opzione1 != 'Y' && opzione1 != 'N');
            
            if(opzione1 == 'Y') {
                // Copia il nuovo giocatore
                giocatore = giocatore1;
                // Resetta le variabile
                punteggio = 0;
                seq_partite = 0;
                diff = 0;
            }
        }
        // Se l'opzione è uguale a R stampa a video le regole
        if (opzione == 'R') {
            clear();
            printf("\tB U B B L E\t B L A S T\n");
            stampaRegole();
            printf("\n\n Premi E per tornare al menu'\n");
            char opzione1 = ' ';
            do {
                scanf(" %c",&opzione1);
                opzione1 = toupper(opzione1);
            }while (opzione1 != 'E');
        }
    } while (opzione != 'E');

    return 0;
}


/****************************************************************************************************
 *              Gestione inserimento e calcolo del minimo della matrice di gioco                    *
 ****************************************************************************************************/
 
// Inserimento randomico nella matrice di gioco delle bolle da 0 a 3
void inserisciBolle(int matrice[ROW][COL]) {
    // Verifica che la matrice non contenga bolle a 0
    bool flag_vuota = true;
    srand(time(NULL));
    while (flag_vuota) {
        for(int x=0;x<ROW;x++) {
            for(int y=0;y<COL;y++) {
                matrice[x][y] = rand()%4;
                if(matrice[x][y] != 0) flag_vuota = false;
            }
        }
    }
}

// Calcolo delle mosse minime della matrice di gioco
void calcolaNumeroMosse(int matrice[ROW][COL], int mosse) {
    // Salva la mossa passata come parametro
    int save_mosse = mosse;
    // Cicla gli stati delle bolle da cercare (si parte dalle bolle in procinto di esplodere)
    for(int stato = 1; stato < 4; stato++){
        // Cicla tutta la matrice passata come parametro
        for(int x = 0; x < ROW; x++){
            for (int y = 0; y < COL; y++) {
                // Verifica se le mosse salvate + stato (che indica numero minimo di altre mosse che sicuramente si andranno a fare)
                // è minore delle mosse massime
                if((save_mosse + stato) >= maxMosse) return;
                // Verifica che la cella della matrice contenga lo stato cercato
                if(matrice[x][y] == stato) {
                    // Si imposta mosse = mosse passate per parametro
                    mosse = save_mosse;
                    // Si fa copia della matrice da modificare
                    int matrice_2[ROW][COL];
                    copiamatrice(*matrice_2,*matrice);
                    // Si decrementa lo stato della cella e nel caso si fa esplodere
                    matrice_2[x][y]--;
                    if(matrice_2[x][y] == 0) esplosione(matrice_2, x, y);

                    // Si incrementa il numero di mosse
                    mosse++;

                    // Si controlla se la matrice è tutta a 0 e si salva nella variabile globale maxMosse le mosse
                    if(controllaVincita(matrice_2)) {
                        maxMosse = mosse;
                        return;
                    }
                    // Copia la matrice modificata
                    int matrice_copia_2[ROW][COL];
                    copiamatrice(*matrice_copia_2,*matrice_2);

                    // Chiama ricorsivamente la funzione passando come parametro la matrice modificata
                    calcolaNumeroMosse(matrice_copia_2, mosse);
                }
            }
        }
    }
}


/****************************************************************************************************
 *                     Gestione esplosioni della matrice di gioco                                   *
 ****************************************************************************************************/
 
// Esplosione main che viene richiamata dalle altre funzioni esplosioni "figlie"
void esplosione(int matrice[ROW][COL], int row, int col) {
    esplosioneSu(matrice, row, col);         // L'esplosione si propaga in alto
    esplosioneGiu(matrice, row, col);        // L'esplosione si propaga in basso
    esplosioneDestra(matrice, row, col);     // L'esplosione si propaga a destra
    esplosioneSinistra(matrice, row, col);   // L'esplosione si propaga a sinistra
}

// Propaga l'esplosione verso l'alto
void esplosioneSu(int matrice[ROW][COL], int row, int col) {
    //Verifica che l'esplosione non sia avvenuta nella prima riga
    do {
        row--;
    } while(row > 0 && matrice[row][col] == 0);
    if(row >= 0 && matrice[row][col] != 0){ //Verifica che si è  usciti dal ciclo perché si è trovata una riga div da 0
        matrice[row][col]--;
        if(matrice[row][col] == 0) esplosione(matrice, row, col);
    }
}

// Propaga l'esplosione verso il basso 
void esplosioneGiu(int matrice[ROW][COL], int row, int col) {
    //Verifica che l'esplosione non sia avvenuta nell'ultima riga 
    do {
        row++;
    } while(row < ROW && matrice[row][col] == 0);
    if(row < ROW && matrice[row][col] != 0){ //Verifica che si è  usciti dal ciclo perché si è trovata una riga div da 0
        matrice[row][col]--;
        if(matrice[row][col] == 0) esplosione(matrice, row, col);
    }
}

// Propaga l'esplosione verso destra
void esplosioneDestra(int matrice[ROW][COL], int row, int col) {
    //Verifica che l'esplosione non sia avvenuta nella prima riga
    do {
        col++;
    } while(col < COL && matrice[row][col] == 0);
    if(col < COL && matrice[row][col] != 0){ //Verifica che si è  usciti dal ciclo perché si è trovata una colonna div da 0
        matrice[row][col]--;
        if(matrice[row][col] == 0) esplosione(matrice, row, col);
    }
}

// Propaga l'esplosione verso sinistra 
void esplosioneSinistra(int matrice[ROW][COL], int row, int col) {
    //Verifica che l'esplosione non sia avvenuta nella prima riga
    do {
        col--;
    } while(col > 0 && matrice[row][col] == 0);
    if(col >= 0 && matrice[row][col] != 0){ //Verifica che si è  usciti dal ciclo perché si è trovata una colonna div da 0
        matrice[row][col]--;
        if(matrice[row][col] == 0) esplosione(matrice, row,col);
    }
}

/****************************************************************************************************
 *                                Gestione modalita' GIOCA                                          *
 ****************************************************************************************************/

// Gestisce la modalità GIOCA in base agli input dell'utente
int partita(Giocatore *giocatore) {
    // Stampe a video
    stampamatriceUtente(giocatore->matrice_partita,giocatore->max_mosse, giocatore->mosse_eff, giocatore->diff_partita); //Da inserire in input utente

    // Cicla finché la matrice non è tutta a 0 o finche il giocatore ha mosse disponibili
    while(!controllaVincita(giocatore->matrice_partita) && giocatore->mosse_eff < giocatore->max_mosse) {
        // Salva su file la partita in corso ad ogni mossa
        scriviGiocatoreSuFile(NOME_FILE,*giocatore);

        // Input utente
        int row, col;
        char input[10];
        do{
            printf("\n Inserisci una riga: ");
            fgets(input, sizeof(input), stdin);
            size_t length = strlen(input);
            if (input[length - 1] == '\n') {
                input[length - 1] = '\0';
            }
            if(toupper(*input) == 'E') return -1; //Torna al menu principale (la partita ripartirà da capo)

            if (isInteger(input)) {
                row = atoi(input);
                row--;
                if(row < 0 || row >= ROW) printf("\n Inserisci una riga da 1 a %d.",ROW);
            } else {
                printf("\n Input non valido.");
            }
        } while(!isInteger(input) || row>=ROW || row<0);

        do{
            printf("\n Inserisci una colonna: ");
            fgets(input, sizeof(input), stdin);
            size_t length = strlen(input);
            if (input[length - 1] == '\n') {
                input[length - 1] = '\0';
            }
            if(toupper(*input) == 'E') return -1; //Torna al menu principale (la partita ripartirà da capo)
            if (isInteger(input)) {
                col = atoi(input);
                col--;
                if(col < 0 || col >= COL) printf("\n Inserisci una colonna da 1 a %d.",COL);
            } else {
                printf("\n Input non valido.");
            }
        } while(!isInteger(input) || col>=COL || col<0);

        // Verifica che la cella inserita dal giocatore non sia vuota
        if (giocatore->matrice_partita[row][col] != 0) {
            // Stampe a video
            clear();
            printf("\tB U B B L E\t B L A S T\n");
            printf("\n******************************************************************************************\n");
            printf(" Hai inserito R: %d e C: %d\n\n",row+1,col+1);

            // Incremento mosse effettuate giocatore
            giocatore->mosse_eff++;
            // Decremento stato bolla cella e gestione esplosione
            giocatore->matrice_partita[row][col]--;
            if (giocatore->matrice_partita[row][col] == 0) {
                esplosione(giocatore->matrice_partita,row, col);
                printf(" Hai colpito una bolla in procinto di esplodere! Pronti all'esplosione.\n BOOOM!");
            }
            else if(giocatore->matrice_partita[row][col] == 1) printf(" Hai colpito una bolla gonfia a meta'! Ora e' in procinto di esplodere.");
            else if(giocatore->matrice_partita[row][col] == 2) printf(" Hai colpito una bolla sgonfia! Si gonfia a meta'");
            stampamatriceUtente(giocatore->matrice_partita,giocatore->max_mosse, giocatore->mosse_eff, giocatore->diff_partita); //Da inserire in input utente
        }
        else printf("\n *Attenzione hai selezionato una cella vuota, riprova!*\n");
    }
    printf("\n******************************************************************************************\n");
    //Verifica che la partita si sia conclusa per vittoria o per perdita
    if(controllaVincita(giocatore->matrice_partita) && giocatore->mosse_eff <= giocatore->max_mosse) {
        printf("\n Complimenti hai vinto!\t Difficolta': %d \t Mosse Minime: %d \t Mosse utilizzate: %d ",giocatore->diff_partita,giocatore->max_mosse,giocatore->mosse_eff);
        return 1;
    }
    printf("\n Hai perso!\t Difficolta': %d \t Mosse Minime: %d \t Mosse utilizzate: %d ",giocatore->diff_partita,giocatore->max_mosse,giocatore->mosse_eff);
    return 0;
}


/****************************************************************************************************
 *                                Gestioni varie matrice                                            *
 ****************************************************************************************************/

// Copia della matrice in una nuova matrice
void copiamatrice(int *matriceCopia, int *matriceOrigine){
    memcpy(matriceCopia, matriceOrigine, ROW*COL*sizeof(int));
}

// Verifica se la matrice contiene tutti 0 (le bolle sono tutte esplose)
bool controllaVincita(int matrice[ROW][COL]) {
    for(int x=0;x<ROW;x++) {
        for(int y=0;y<COL;y++){
            if(matrice[x][y] != 0) {
                return false;
            }
        }
    }
    return true;
}


/****************************************************************************************************
 *                                    Gestione Giocatore                                            *
 ****************************************************************************************************/

// Valorizza la struttura Giocatore con tutti valori di default ad eccezione del nome
Giocatore valorizzaGiocatore(char *user) {
    Giocatore giocatore;
    giocatore.nome = strdup(user);
    giocatore.partite_giocate = 0;
    giocatore.partite_vinte = 0;
    giocatore.miglior_sequenza = 0;
    giocatore.data_ultima_partita = "";
    giocatore.matrice_partita;
    giocatore.max_mosse = 0;
    giocatore.mosse_eff = 0;
    giocatore.diff_partita = 0;
    return giocatore;
}

// Verifica la presenza di un giocatore con lo stesso nome nel file definito in NOME_FILE ed eventualmente
// carica i dati nella struttura Giocatore
Giocatore leggiGiocatoriDaFile(const char *nomeFile, char *user) {
    // Valorizzazione giocatore di default
    Giocatore giocatore = valorizzaGiocatore(user);

    // Apertura file di lettura
    FILE *file = fopen(nomeFile, "r");

    if (file == NULL) {
        // Il file non esiste, lo creiamo
        file = fopen(nomeFile, "w"); // Apre il file in modalità scrittura
        if (file != NULL) {
           fclose(file); // Chiude il file dopo averlo creato
        } else {
            perror("Errore nella creazione del file.\n");
            return giocatore;
        }
    }

    char buffer[MAX_LENGTH]; // Buffer per salvare la riga letta
    char *token;
    char *tokens[150];
    int num_tokens = 0;
    // Legge una riga alla volta finché non raggiunge la fine del file
    bool flg_trovato = false;
    while (fgets(buffer, MAX_LENGTH, file) != NULL) {
        // Verifica se nella riga letta nella prima posizione è presente la stringa uguale al nome del giocatore in cas valorizza il flg_trovato
        token = strtok(buffer, ";");
        if(strcmp(token, giocatore.nome) == 0) {
            flg_trovato = true;
            break;
        }
    }

    //Se è stato trovato il giocatore nel file valorizza il giocatore con i dati trovati
    if(flg_trovato) {
        while (token != NULL && num_tokens < 100) {
            tokens[num_tokens++] = token;
            token = strtok(NULL, ";");
        }
        if(tokens[1] != NULL && isInteger(tokens[1])) giocatore.partite_giocate = atoi(tokens[1]);
        if(tokens[2] != NULL && isInteger(tokens[2])) giocatore.partite_vinte = atoi(tokens[2]);
        if(tokens[3] != NULL && isInteger(tokens[3])) giocatore.miglior_sequenza = atoi(tokens[3]);
        if(tokens[4] != NULL && isInteger(tokens[4])) giocatore.punteggio = atoi(tokens[4]);
        if(tokens[5] != NULL) giocatore.data_ultima_partita = strdup(tokens[5]);

        // Verifica se è presente la stringa della matrice corretta e la valorizza
        bool flag_matrice = true;
        bool flag_vuota = true;
        if(tokens[6] != NULL && isInteger(tokens[6])) {
            int indice_carattere = 0;
            for (int riga = 0; riga < ROW; riga++) {
                for (int colonna = 0; colonna < COL; colonna++) {
                    if (tokens[6][indice_carattere] != '\0') {
                        giocatore.matrice_partita[riga][colonna] = tokens[6][indice_carattere] - '0'; // Conversione da carattere a intero
                        if(giocatore.matrice_partita[riga][colonna] != 0) flag_vuota = false;
                        indice_carattere++;
                    }
                    else flag_matrice = false;
                }
            }
        }
        // Se è presente la matrice valorizza anche gli altri campi relativi alla partira in corso
        if(flag_matrice && !flag_vuota) {
            if(tokens[7] != NULL && isInteger(tokens[7])) giocatore.max_mosse   = atoi(tokens[7]);

            if(giocatore.max_mosse == 0) {
                printf("\n Calcolo numero mosse minime in corso...");
                maxMosse = 100;
                calcolaNumeroMosse(giocatore.matrice_partita,0);
                giocatore.max_mosse = maxMosse;
            }

            if(tokens[8] != NULL && isInteger(tokens[8])) giocatore.mosse_eff   = atoi(tokens[8]);
            if(tokens[9] != NULL && isInteger(tokens[9])) giocatore.diff_partita = atoi(tokens[9]);
        }
    }
    // Chiusura del file
    fclose(file);

    // Ritorna la struttura del giocatore
    return giocatore;
}

// Scrive su file il nuovo Giocatore (solo se inizia una partita) o aggiorna i dati del giocatore già presente
void scriviGiocatoreSuFile(const char *nomeFile, Giocatore giocatore) {
    FILE *fileIn, *fileTemp;
    char riga[MAX_LENGTH];

    // Apre i due file in modalita' lettura e in modalità scrittura
    fileIn = fopen(nomeFile, "r");
    fileTemp = fopen("temp.txt", "w");

    if (fileIn == NULL || fileTemp == NULL) {
        perror("Errore durante l'apertura dei file");
        exit(EXIT_FAILURE);
    }

    // Verifica se il giocatore ha una partita in corso e lo scrive nella prima riganel file temp
    if(giocatore.max_mosse != 0){
        fprintf(fileTemp, "%s;%d;%d;%d;%d;%s;%s;%d;%d;%d;\n",
                giocatore.nome,
                giocatore.partite_giocate,
                giocatore.partite_vinte,
                giocatore.miglior_sequenza,
                giocatore.punteggio,
                ottieni_data(),
                matrice_toString(giocatore.matrice_partita),
                giocatore.max_mosse,
                giocatore.mosse_eff,
                giocatore.diff_partita
        );
    } else {
        fprintf(fileTemp, "%s;%d;%d;%d;%d;%s;%s;%d;%d;%d;\n",
                giocatore.nome,
                giocatore.partite_giocate,
                giocatore.partite_vinte,
                giocatore.miglior_sequenza,
                giocatore.punteggio,
                ottieni_data(),
                "",
                0,
                0,
                0
        );
    }

    // Ricopia tutti le righe dal file principale a quello temp tranne (se esiste) quella del giocatore scritto per primo
    while (fgets(riga, MAX_LENGTH, fileIn) != NULL) {
        // Verifica se la riga contiene il dato da rimuovere
        if (strstr(riga, giocatore.nome) == NULL) {
            // Se non contiene il dato, scrive la riga nel file temporaneo
            fputs(riga, fileTemp);
        }
    }

    // Chiusura dei file
    fclose(fileIn);
    fclose(fileTemp);

    // Rimuove il file originale e rinomina il file temporaneo
    remove(nomeFile);
    rename("temp.txt", nomeFile);
    remove("temp.txt");
}

// Trasforma la matrice del Giocatore in una stringa da scrivere e salvare su file
char *matrice_toString(int matrice[ROW][COL]){
    char *stringa = (char *)malloc(ROW*COL * sizeof(char));;
     // Inizializza la stringa risultante come vuota

    int index = 0;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            stringa[index] = matrice[i][j] + '0'; // Converte l'intero in carattere e lo mette nella stringa
            index++;
        }
    }
    stringa[index] = '\0'; // Aggiunge il terminatore di stringa
    return stringa;
}

// Ritorna la data corrente, viene utilizzata per valorizzare la data dell'ultima partita del giocatore
char *ottieni_data() {
    time_t tempo_attuale;
    struct tm *info_tempo;
    char *data_corrente = (char *)malloc(50 * sizeof(char)); // Dimensione della stringa per contenere la data formattata
    // Ottenimento del tempo attuale
    time(&tempo_attuale);
    info_tempo = localtime(&tempo_attuale);

    // Formattazione della data
    strftime(data_corrente, 20, "%d/%m/%Y %H:%M:%S", info_tempo);
    return data_corrente;
}

// Aggiunge o rimuove mosse alle mosse massime in base:
// alle mosse massime (se presente), alla difficolta precedente (se presente) e alla difficolta nuova
int impostaDifficolta(int max_mosse, int difficolta_old, int difficolta_new) {
    if(difficolta_old == difficolta_new) return 0;
    if(max_mosse == 0){ //Verifica se le mosse massime disponibili al giocatore sono già state calcolate
        if(difficolta_new == 1) return 4; //Difficolta facile
        if(difficolta_new == 2) return 2; //Difficolta media
    }
    else {
        if(difficolta_old == 3) { //Se la vecchia difficolta' era 3 (difficile) aggiunge 4 per diff. facile e 2 per diff. media
            if(difficolta_new == 1) return 4; //Difficolta facile
            if(difficolta_new == 2) return 2; //Difficolta media
        }
        if(difficolta_old == 2) { //Se la vecchia difficolta' era 2 (media) aggiunge 2 per diff. facile e ne toglie 2 per diff. difficile
            if(difficolta_new == 1) return 2; //Difficolta facile
            if(difficolta_new == 3) return -2; //Difficolta difficile
        }
        if(difficolta_old == 1) { //Se la vecchia difficolta' era 1 (facile) toglie 2 per diff. media e 4 per diff. difficile
            if(difficolta_new == 2) return -2; //Difficolta media
            if(difficolta_new == 3) return -4; //Difficolta difficile
        }
    }
    return 0;
}

/****************************************************************************************************
 *                                       FUNZIONI DI STAMPA                                         *
 ****************************************************************************************************/


// Stampa la matrice della partita a video
void stampamatrice(int stamp[ROW][COL]) {
    printf("\n");
    printf(" +-----");
    for (int j = 0; j < COL; j++) {
        printf("+-------");
    }
    printf("+\n");
    printf(" | RxC ");
    for (int j = 0; j < COL; j++) {
        printf("|   %d   ", j+1);
    }
    printf("|\n");

    // Stampare separatore per le intestazioni delle colonne
    printf(" +-----");
    for (int j = 0; j < COL; j++) {
        printf("+-------");
    }
    printf("+\n");

    for (int i = 0; i < ROW; i++) {
        // Stampare intestazione delle righe
        printf(" |  %d  |", i+1);

        for (int j = 0; j < COL; j++) {
            if(stamp[i][j] == 0) {
                if(j == COL -1) printf("       |");
                else printf("        ");
            }else{
                if(j == COL -1) printf("  (%d)  |", stamp[i][j]);
                else printf("  (%d)   ", stamp[i][j]);
            }

        }
        printf("\n");
        // Stampare separatore tra le righe
    }

    printf(" +-----");
    for (int j = 0; j < COL; j++) {
        printf("+-------");
    }
    printf("+");
}

// Aggiunge alla matrice della partita le informazioni di mosse massime, mosse effettuate e difficoltà
void stampamatriceUtente(int matrice[ROW][COL],int max_mosse, int mosse, int difficolta) {
    printf("\n******************************************************************************************");
    printf("\n");
    stampamatrice(matrice);
    printf("\n\n******************************************************************************************");
    printf("\n Mosse minime: %d \tMosse effettuate: %d \tDifficolta': %d \t E per tornare al menu'\n",max_mosse, mosse,difficolta);
}

// Stampa a video le statistiche del Giocatore
void stampaStatistiche(Giocatore giocatore) {
    printf("\n******************************************************************************************\n");
    printf(" STATISTICHE DEL GIOCATORE (premi E per tornare al menu')\n\n");
    printf("  Nome: %s\n", giocatore.nome);
    printf("  Data ultima partita: %s\n", giocatore.data_ultima_partita);
    printf("  Partite giocate: %d\n", giocatore.partite_giocate);
    printf("  Partite Perse: %d\n", giocatore.partite_giocate - giocatore.partite_vinte);
    printf("  Partite totali vinte: %d\n", giocatore.partite_vinte);
    printf("  Miglior sequenza: %d\n", giocatore.miglior_sequenza);
    printf("  Miglior punteggio: %d\n", giocatore.punteggio);
    printf("  Percentuale di vittoria: %.2f%%\n", ((float)giocatore.partite_vinte/giocatore.partite_giocate*100));
    printf("\n******************************************************************************************\n");
    char opzione = ' ';
    do {
        scanf(" %c",&opzione);
        opzione = toupper(opzione);
    }while (opzione != 'E');
}

// Stampa a video e chiede all'utente la difficoltà da impostare al gioco
int stampaDifficolta(int diffAtt) {
    printf("\n******************************************************************************************\n");
    printf(" IMPOSTA DIFFICOLTA'");
    if(diffAtt != 0) {
        printf(" (premi E per tornare al menu')\n");
        printf("\n Difficolta' attuale impostata su %d\n", diffAtt);
    }
    else printf("\n");

    printf("\n Seleziona una difficolta' \n  1) Facile (4 mosse extra) \n  2) Media (2 mosse extra) \n  3) Difficile (nessuna mossa extra)\n\n");
    printf("******************************************************************************************\n");
    char input[50];
    int difficolta = 0;
    do{
        fgets(input, sizeof(input), stdin);
        size_t length = strlen(input);
        if (input[length - 1] == '\n') {
            input[length - 1] = '\0';
        }
        if(toupper(*input) == 'E') return diffAtt;
        if (isInteger(input)) {
            difficolta = atoi(input);
        } else {
            printf("\n Input non valido.");
        }
    } while(difficolta != 3 && difficolta != 2 && difficolta != 1);

    return difficolta;
}

// Stampa a video le regole del gioco
void stampaRegole() {
    printf("\n Lo scopo del gioco e' semplice: fai esplodere tutte le bolle prima di terminare i tentativi.\n");
    printf(" Appena partira' il gioco vedrai una matrice con dei numeri al suo interno.\n");
    printf(" I tipi di bolle che puoi trovare sono 3:\n \t 1) Bolla in procinto di esplodere;\n\t 2) Bolla gonfia a meta';\n\t 3) Bolla sgonfia.\n");
    printf(" Quando colpirai una bolla sgonfia (3) la gonfierai a meta' (2). \n Quando colpirai una bolla gonfia a meta' (2) la renderai in procinto di esplodere (1). \n Quando colpirai una bolla in procinto di esplodere (1) la farai esplodere (0).\n");
    printf(" Facendo esplodere una bolla di tipo 1 creerai un'esplosione a catena che coinvolgera' le bolle nelle quattro direzioni rispetto a quella da te colpita!\n");
    printf(" Vinci quando fai esplodere tutte le bolle presenti sulla matrice, perdi se non riesci nei tentativi possibili.\n\n INIZIAMO!");
}

//Pulisce il prompt dei comandi in base al comando definito in CLEAR_COMMAND
void clear() {
    system(CLEAR_COMMAND);
}

// Verifica che l'input passato sia di un INT
bool isInteger(const char *str) {
    int i = 0;
    if (str[0] == '-') {
        if (strlen(str) == 1) {
            return false;
        }
        i = 1;
    }
    for (; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}