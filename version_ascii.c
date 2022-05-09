
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LONGUEUR_PLATEAU 24
#define LARGEUR_PLATEAU 7
typedef struct chips{ 
int type;
int life;
int line;
int position;
int price;
struct chips* next;
}Chips;

typedef struct virus{ 
int type;
int life;
int line;
int position;
int speed;
int turn;
struct virus* next;
 struct virus* next_line;
  struct virus* prev_line;
}Virus;


typedef struct{ 
    Virus* virus; 
    Chips* chips;
    int turn;
    int money;
}Game;
typedef struct {
    char plateau[LARGEUR_PLATEAU+1][LONGUEUR_PLATEAU+1] ;
} Plateau ;

//affichage du menu des chips:
void affichage_menu_des_choix(){
    printf("What chips do you want to deploy ?\n");
    printf("F : firewall\n");
    printf("P : PMMU\n");
    printf("A : ALU\n");
    printf("R : RAM\n");
    printf("X : Patch\n");
    printf("q : no more\n");
    printf("Your choice ?\n");

}


//initialition des champs de la struct game:
void initialisation(Game *game){
    game->chips = NULL;
    game->virus = NULL;
    game->turn=0;
    game->money=0;
}

//alloction d'un chips: 
Chips* alloue_chips( int line, int position, char type){
        Chips *tmp = (Chips*)malloc(sizeof(Chips));
        if(tmp){
            if (type=='A'){
            tmp->type=1;
            tmp->life=2;
            tmp->price=10;
            
            }
            else if (type=='R'){
                tmp->type=2;
                tmp->life=3;
                tmp->price=15;
            }
            else if (type=='P'){
                tmp->type=3;
                tmp->life=4;
                tmp->price=20;
            }
            else if (type=='X'){
                tmp->type=4;
                tmp->life=5;
                tmp->price=25;
            }
            else if (type=='F'){
                tmp->type=5;
                tmp->life=6;
                tmp->price=30;
            }

        tmp->next=NULL;
        tmp->line = line;
        tmp->position=position;
        }return tmp;
}
//ajout d'un chip dans la liste chainée:
int ajout_chips(Game *game, int line,int position, char type){
    //on crée un nouveau élement:
    Chips* tmp = alloue_chips(line,position, type);
    //si la liste est vide on ajoute seulement l'élément qu'on vient de créer:
    if(game->chips==NULL){
        tmp->next=game->chips;
        game->chips = tmp;
        return 1;
    }
    //si ce n'est pas le cas, on parcourt la liste et on ajoute l'élément à la fin de la liste
    else{
       
        Chips* t = game->chips;
        //parcours de game->chips
        while(t->next){
            t = t->next;
        }
        t->next = tmp;
        return 1;
    }
    
}


//une fonction qui associe chaque type à un char
char type_entier_to_char(Virus* virus){
    char type =' ';
    switch (virus->type)
    {
    case 1:
        type = 'E';
        break;
    case 2:
        type = 'D';
        break;
     case 3:
        type = 'S';
        break;
     case 4:
        type = 'M';
        break;
     case 5:
        type = 'B';
        break;
    
    default:
        break;
    }
    return type;
}
//meme fonction mais pour les chips
char type_entier_to_char_chips(Chips* chips){
    char type =' ';
    switch (chips->type)
    {
    case 1:
        type = 'A';
        break;
    case 2:
        type = 'R';
        break;
     case 3:
        type = 'P';
        break;
     case 4:
        type = 'X';
        break;
     case 5:
        type = 'F';
        break;
    
    default:
        break;
    }
    return type;
}



//allocation d'un virus:
Virus* alloue( int turn, int line, char type){
        Virus *tmp = (Virus*)malloc(sizeof(Virus));
        if(tmp){
            if (type=='E'){
            tmp->type=1;
            tmp->life=3;
            tmp->speed=2;
            
            }
            else if (type=='D'){
                tmp->type=2;
                tmp->life=4;
                tmp->speed=1;
            }
            else if (type=='S'){
                tmp->type=3;
                tmp->life=5;
                tmp->speed=1;
            }
            else if (type=='M'){
                tmp->type=4;
                tmp->life=4;
                tmp->speed=1;
            }
            else if (type=='B'){
                tmp->type=5;
                tmp->life=7;
                tmp->speed=1;
            }

        tmp->next=NULL;
        tmp->next_line=NULL;
        tmp->prev_line=NULL;
        tmp->line = line;
        tmp->turn = turn;
        tmp->position=LONGUEUR_PLATEAU;
        }return tmp;
}

//ajout d'un virus dans la liste chainée, dans cette fonction on gère que les chainages simples
int ajout_virus(Game *game, int turn, int line ,char type){
    //on crée un nouveau élement:
    Virus* tmp = alloue(turn,line ,type);
       
    //si la liste est vide on ajoute seulement l'élément qu'on vient de créer:
    if(game->virus==NULL){
        //tmp->next=game->virus;
        game->virus = tmp;
        return 1;
    }
    //si ce n'est pas le cas, on parcourt la liste et on ajoute l'élément à la fin de la liste
    else{
        Virus* t = game->virus;
        while(t->next){
            t = t->next;
        }
        t->next = tmp;
        return 1;
    }
    
    return 0;
}

//c'est ici qu'on gère le chainage double ( chainage selon la ligne)
void chainage_double(Game game){
    Virus* tmp;
    //on parcourt la liste des virus
    for(; game.virus!=NULL; game.virus= game.virus->next){
        //on fait le meme parcours sauf que cette fois on le fait avec le next du virus de la première boucle
        tmp=game.virus->next;
        for(; tmp!=NULL; tmp=tmp->next){
            //à chaque fois on regarde si les deux virus on la meme ligne
            if(game.virus->line==tmp->line){
                //si oui le next_line du virus sera sont next
                game.virus->next_line = tmp;
                //et le prev_line du next_line du game_virus sera game_virus lui meme
                tmp->prev_line=game.virus;
                break;
            }
        }
    }
}

//cette fonction controle la vitesse, on autorise pas le dépassement d'un  virus par un autre
void controle(Game *game){
    if(game==NULL){
        return;
    }
    
    Virus* tmp = game->virus;
    //on parcours la liste
    while(tmp->next_line){
        //si la vitesse du virus next_line du virus tmp est plus grande, on change sa vitesse (la vitesse de next line) et ca devient la vitesse du virus qui lui précède
        if(tmp->speed < tmp->next_line->speed){
            tmp->next_line->speed = tmp->next_line->prev_line->speed;
        }
        tmp = tmp->next;
    }
    

}



/*******plateau du jeu***********/
//une fonction qui inistialise notre plateau de jeu par des espaces pour pouvoir le remplir après
void initialiser_plateau(Plateau* p){
    int i, j;
    for(i=1;i<LARGEUR_PLATEAU+1;i++){
        for(j=1;j<LONGUEUR_PLATEAU+1; j++){
            p->plateau[i][j]=' ';
        }
    }
}

//on remplie le plateau par les tourelles, on ajoute la tourelle à la liste chainée et on remplie la case line-colonne par le choix
void poser_tourelle(Game *game, Plateau* p, int line, int position, char choix){
    if(ajout_chips(game, line, position, choix)){
        p->plateau[line][position]=choix;
   
    }

}





/**** vague ****/
//on initialise la vague par des espaces, le nbturn dans les paramètres on va le recuperer apres car c'est modifiable en fonction du level
void initialiser_vague(int nbturn, char plateau[LARGEUR_PLATEAU+1][nbturn+1]){
    int i, j;
    
    for(i=1;i<LARGEUR_PLATEAU+1;i++){
        for(j=1;j<nbturn+1; j++){
            plateau[i][j]=' ';
        }
    }
}

//on parcourt la liste chainée des virus et on remplie chaque case ( line-turn ) par un char qu'on a trouvé grace a notre fonction type_entier_to_char(virus* virus)
void remplir_vague(Game game,int turn, int line, char type,int nbturn, char plateau[LARGEUR_PLATEAU+1][nbturn+1]){
            if(game.virus == NULL){return;}
            Virus* tmp = game.virus;
            while(tmp->next){
                plateau[tmp->line][tmp->turn] = type_entier_to_char(tmp);
                tmp = tmp->next;
            }
}

//un affichage basique d'un tab[][]....
void afficher_vague(int nbturn, char plateau[LARGEUR_PLATEAU+1][nbturn+1]){
    int i, j;
    printf("Here is the wave of problems approaching...\n");
    for(i=1;i<LARGEUR_PLATEAU+1;i++){
         printf("|%d ", i);
        for(j=1;j<nbturn+1; j++){
            printf(" %c", plateau[i][j]);
        }printf("\n");
    }
}





////interaction clavier///////

//avant de d'ajouter la tourelle on doit verifier si la case que l'utilisateur a choisit est dans le plateau
int case_dans_plateau(int line, int position){
    if(line>=1 && line<LARGEUR_PLATEAU+1 && position>=1 && position<LONGUEUR_PLATEAU+1){
        return 1;
    }
    else 
        return 0;

}
//pareil, ici on verifie si la case est vide
int case_vide(Game game, int line, int position){
    if(game.chips==NULL){return 1;}
    Chips* tmp = game.chips;
    while(tmp){
        if(tmp->line == line && tmp->position == position ){ return 0;}
        tmp = tmp->next;
    }return 1;
   
}


//on demande les coordonnées, tant qu'ils sont pas valide on redemande..
void coordonnees(Game *game,Plateau p, int *line, int *position, char choice){
    printf("Rentrez la ligne et la position de votre tourelle: ");
    scanf("%d %d", line, position);
    while(!case_dans_plateau(*line, *position) || !case_vide(*game, *line, *position) ){
            printf("Rentrez la ligne et la position de votre tourelle: ");
            scanf("%d %d", line, position); 
    }
}

//on demande le choix si le choix n'est pas valide  on redemande
void choix(char *choice){
    scanf(" %c", choice);
    while(*choice != 'A' && *choice != 'R' && *choice != 'P'  && *choice !='q'){
        printf("Votre choix n'est pas valide! Réessayer ");
        scanf(" %c", choice);
        
    }
}


//fonction qui nous donne le prix de chaque tourelle, Pourquoi ? pareceque avant d'ajouter  la tourelle a la liste on doit verifier que l'utilisateur peut l'acheter ou pas.
int price(char choice){
    int prix=0;
    switch (choice)
    {
    case 'A':
        prix = 100;
        break;
    case 'R':
        prix = 150;
        break;
    case 'P':
        prix = 300;
        break;
    case 'X':
        prix = 50;
        break;
    case 'F':
        prix = 125;
        break;
    
    default:
        break;
    }
    return prix;
}


//on remplie le plateau par les virus a partir de la liste
void parcours(Game* game,Plateau* p, int *life){
    Virus* tmp = game->virus;
    if(game->virus==NULL){return;}
    while((tmp->turn)<=(game->turn) && (tmp->next)!=NULL){
        p->plateau[tmp->line][tmp->position]=type_entier_to_char(tmp);
        tmp = tmp->next;
    }
}
//on fait pareil pour les chips
void parcours_chips(Game* game,Plateau* p){
    Chips* tmp = game->chips;
    while((tmp)!=NULL){
        p->plateau[tmp->line][tmp->position]=type_entier_to_char_chips(tmp);
        tmp = tmp->next;
    }
}

//une fonction qui nous donne les pts de vies du virus dans une cas précise 
int life(Game game,  int line, int position){
    if(game.virus == NULL){return 0;}
    Virus* tmp = game.virus;
    while(tmp){
        if(tmp->line == line && tmp->position==position){return tmp->life;}
        tmp = tmp->next;
    }
    return 0;

}

// la meme fonction pour les chips, le but de ces deux fontions c'est avoir un affichage correct vu qu'on parcourt pas la liste chainée pour afficher le plateau
int life_chips(Game game,  int line, int position){
    if(game.chips == NULL){return 0;}
    Chips* tmp = game.chips;
    while(tmp){
        if(tmp->line == line && tmp->position==position){return tmp->life;}
        tmp = tmp->next;
    }
    return 0;

}
//affichage basique ...
void afficher_plateau(Game game,Plateau* p){
    int i, j;
    printf("Game turn: %d  \n",game.turn);
    for(i=1;i<LARGEUR_PLATEAU+1;i++){
        printf("%d| ", i);
        for(j=1;j<LONGUEUR_PLATEAU+1; j++){
                if(p->plateau[i][j]=='E' || p->plateau[i][j]=='D' || p->plateau[i][j]=='S' || p->plateau[i][j]=='M'|| p->plateau[i][j]=='B')
                    printf(" %2d%c",life(game, i, j), p->plateau[i][j]);
                else if(p->plateau[i][j]=='A' || p->plateau[i][j]=='F' || p->plateau[i][j]=='P' || p->plateau[i][j]=='R' || p->plateau[i][j]=='X' )
                    printf(" %2c%d", p->plateau[i][j],life_chips(game,i, j));
                else{
                    printf("   %c", p->plateau[i][j]);
                }
        }printf("\n");
            
    }
        
}

//fonction pour faire avancer les virus en verifiant qu'il peut pas depasser une tourelle et il s'arrete
void diminue(Game* game){
    Virus * vir = game->virus;
    for(;vir !=NULL && vir->turn <= game->turn; vir = vir->next){
        Chips* chi = game->chips;
        if(game->chips ==NULL){vir->position = vir->position - vir->speed;}
        else{
            for(;chi !=NULL; chi = chi->next){
                if(vir->position - chi->position  <= vir->speed  && vir->line == chi->line){
                    vir->speed=0;
                    break;
                }
                else{
                    vir->position = vir->position - vir->speed;
                    break;
                }
            }

        }
    }
}

//fonction pour supprimer un virus
void delete(Virus** vir, Virus* virus){
    Virus* tmp = *vir;
    if( *vir == NULL){return;}

    if(*vir == virus ){
        *vir = (*vir)->next;
        if(virus->next_line !=NULL){
            virus->next_line->prev_line = NULL;
        }
        free(virus);
        
    
    }
    else{
        while(tmp->next != NULL && tmp->next != virus)
            tmp =tmp->next;
            if(tmp->next ){
                tmp->next = virus->next;
                if(virus->next_line){
                    virus->next_line->prev_line = virus->prev_line;
                }
                if(virus->prev_line){
                    virus->prev_line->next_line = virus->next_line;
                }
                
                free(virus);
                
            }        
    }
}

void action_chips_sur_virus(Game* game){
    Chips* chi=game->chips;
    for(;chi !=NULL; chi = chi->next){
        Virus* vir=game->virus;
        for(;vir !=NULL; vir = vir->next){
            //action de la tourelle A elle attaque de loin et elle fait diminuer les pts de vies 
            if(chi->type == 1 && chi->line == vir->line && vir->position < 24 ){
                vir->life -=1;
                if(vir->life <= 0){
                    delete(&game->virus, vir);
                }
                break;
            }
            //action de la tourelle R elle ralentie les virus
            if(chi->type == 2 && chi->line == vir->line && vir->position < 24 ){
                vir->speed = 1;
                break;
            }

            //action de la tourelle P elle tue directement au contact seulement
            if(chi->type == 3 && chi->line == vir->line && vir->position == chi->position + 1 ){
                delete(&game->virus, vir);
                break;
            }

        }
    }
}

//supprimer un chip
int delete_chips(Game* game, int line, int position){
    if(game->chips == NULL){return 0;}
    Chips* tmp ;
    if(game->chips->line == line && game->chips->position == position){
        tmp = game->chips;
        game->chips = game->chips->next;
        free(tmp);
    }
    else{
        Chips* tmp2 = game->chips;
        while(tmp2->next){
            if(tmp2->next->line == line && tmp2->next->position == position){
                tmp = tmp2->next;
                tmp2->next = tmp2->next->next;
                free(tmp);
                break;
            }
            else{ tmp2 =tmp2->next;}
        }
    }return 0;
}


void action_virus_sur_chips(Game* game){
    Virus * vir = game->virus;
    for(;vir !=NULL; vir = vir->next){
        Chips* chi = game->chips;
        for(;chi !=NULL; chi = chi->next){
            //action du virus E il attaque au contact et il fait diminuer les pts de vies des chips
            if(vir->type == 1 && chi->line == vir->line && vir->position == chi->position + 1 ){
                chi->life -=1;
                if(chi->life <= 0){
                    delete_chips(game, chi->line, chi->position);
                }
                break;
            }

        }
    }
}


int life_virus_initial(int type){
    int life=0;
    switch (type)
    {
    case 1:
        life = 3;
        break;
    case 2:
        life = 4;
        break;
    case 3:
        life = 5;
        break;
    case 4:
        life = 6;
        break;
    
    default:
        break;
    }
    return life;
}

void action_virus_sur_virus(Game *game){
    Virus * vir = game->virus;
    for(;vir !=NULL; vir = vir->next){
        //action virus M il soigne avant et après 
        if(vir->type == 4 && vir->next_line && vir->prev_line){
            
                vir->prev_line +=1;
            
            if(vir->next_line->turn <= game->turn && vir->next_line->life < life_virus_initial(vir->next_line->type) ){
                vir->next_line += 1;
            }
            break;
        }
    }
}
//liberation de memoire
void liberer_virus(Game *game){
    Virus* next;
    while(game->virus){
        next = game->virus->next;
        free(game->virus);
        game->virus = next;
    }
}


void liberer_chips(Game *game){
    Chips* next;
    while(game->chips){
        next = game->chips->next;
        free(game->chips);
        game->chips = next;
    }
}
//condition du fin de jeu, un virus atteint l'ordinateur central
int game_over(Game game){
    Virus* tmp_virus = game.virus;
    if(game.virus){
        while(tmp_virus->next){
            if(tmp_virus->position+tmp_virus->speed<=0){
                    printf("GAME OVER\n");
                return 1;
            }
            tmp_virus = tmp_virus->next;
        } 
    }
    return 0;
}


//condition de victoire
int victoire(Game game){
    if (game.virus==NULL){
        printf("You Won!");
        return 1;
    }
    return 0;
}

//fonction qui calcule le nb des lignes dans le fichier txt et le turn max 
int donnees(Game *game, char nom[], int *max){
    int c = 0;
    FILE *f = fopen(nom, "r");
    if (f == NULL) {
        exit(1);
    }
    int turn, line;
    char type;
    int argent;
    fscanf(f, "%d", &(argent));
    while (fscanf(f, "%d %d %c", &turn, &line, &type) != EOF) {
        c +=1;
        if (turn > *max)
            *max = turn;
    }

    fclose(f);
    return c;
}

int main(int argc, char *argv[]){

    
    int turn, line, argent, life;
    Plateau plateau;
    char type;
    char choice;
    int ligne, position;
    Game game;
    int max;
   
    initialisation(&game);
    donnees(&game, argv[1], &max);
    char vague[8][max+ 1];
    initialiser_vague(max, vague);
    game.turn = 1;
    initialiser_plateau(&plateau);
    FILE * f = fopen(argv[1], "r");
    int i;
    fscanf(f, "%d \n",&argent);
    for(i=1; i<=donnees(&game, argv[1], &max)+ 1; i++){
        fscanf(f, "%d %d %c", &turn, &line, &type);
        ajout_virus(&game, turn, line, type);
        remplir_vague(game, turn, line, type, max, vague);
    }
    
    fclose(f);
    chainage_double(game) ;
    afficher_vague(max,vague);
    affichage_menu_des_choix();
    choix(&choice);
    int prix = price(choice);

    if(argent >= prix && choice != 'q'){
        argent = argent - prix;
        coordonnees(&game,plateau, &ligne, &position, choice);
        poser_tourelle(&game, &plateau, ligne, position, choice);
        printf("Money %d\n", argent);
        afficher_plateau(game, &plateau);
    }
    
    while(argent >= prix && choice !='q'){
        printf("Your choice ? \n");
        argent = argent - prix;
        choix(&choice);
        if(choice != 'q'){
            coordonnees(&game,plateau, &ligne, &position, choice);
            poser_tourelle(&game, &plateau, ligne, position, choice);
            printf("Money %d\n", argent);
            afficher_plateau(game, &plateau);
            prix = price(choice);
        }
    
    }  

while(!game_over(game)&& !victoire(game)){

        parcours(&game,&plateau, &life);
        afficher_plateau(game,&plateau);
        diminue(&game);
        controle(&game);
        action_chips_sur_virus(&game);
        action_virus_sur_chips(&game);
        action_virus_sur_virus(&game);
        game.turn++;
        initialiser_plateau(&plateau);
        parcours(&game,&plateau, &life);
        parcours_chips(&game, &plateau);
        sleep(2);
        
    }
liberer_virus(&game);
liberer_chips(&game);


}
    



