#include <raylib.h> 
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <termios.h>

#define LARGURA_JANELA 800
#define ALTURA_JANELA 600
#define STD_SIZE_X 32
#define STD_SIZE_Y 32

#define TANAX 70
#define TANAY 70

#define LARGURA_BALA 10
#define ALTURA_BALA 15

#define NUM_LINHAS 4
#define NAVES_POR_LINHA 7

#define HEROI_HITBOX_LARGURA 110 //começa do canto esquerdo e expande pra direita
#define HEROI_HITBOX_ALTURA 10  //expande pra baixo
#define HEROI_HITBOX_X 3  // Ajuste lateral da hitbox
#define HEROI_HITBOX_Y 15  // Ajuste vertical da hitbox

// Rectangle pos: posição e tamanho
typedef struct {
Rectangle pos; 
int vida;      
} Barreira;
typedef struct Bala{
Rectangle pos; 
Color color;
int ativa;
int tempo;
int velocidade;
Sound tiro;
}Bala;
typedef struct Nave{
Rectangle pos;
Color color;
Bala bala;
int velocidade;
int direcao;
int vida;
 int atirando;           // 1 = está mostrando a textura de tiro
    double tempoUltimoTiro; // Controla quanto tempo mostra a textura de tiro
}Nave;
typedef struct Heroi{
Rectangle pos;
Color color;
Bala bala;
int direcao;
int velocidade;
int vida;
int atirando;           // 1 = atirando, 0 = normal
double tempoUltimoTiro; 
}Heroi;
typedef struct Bordas{
Rectangle pos;
} Bordas;
typedef struct Assets{
Texture2D naveTexturas[4];
Texture2D naveAtirando;
Texture2D heroiImagem;       //sprite heroi normal
Texture2D heroiImagemTiro;  //sprite heroi atirando
Texture2D imagemfundo;
Texture2D barreira1;
Texture2D barreira2;
Texture2D vida;
Sound tiro;
}Assets;
typedef enum{   //alterna entre as telas ao longo do jogo
placar_inicio,
inserindo_nome,
jogando,
placar_fim
} tela_atual;
typedef struct Jogo{
int vitoria;
int derrota;
int naves_derrotadas;
Nave naves[NUM_LINHAS][NAVES_POR_LINHA]; // matriz 5x7

Heroi heroi;
Bordas bordas[4]; 
Barreira barreiras[5];

Assets assets;
int alturaJanela;
int larguraJanela;
int tempoAnimação;  //controla as animações do jogo
int f2_pressionado;

char apelido[20];
char placar_nomes[5][20];

int posicao_letra;  
int placar[5]; 
int placar_naves[5];
tela_atual estado;
int fase; 
}Jogo;


void IniciaJogo(Jogo *j);
void AtiraBalasHeroi(Jogo *j);
void AtualizaJogo(Jogo *j);
void DesenhaJogo(Jogo *j);
void AtualizaFrameDesenho(Jogo *j);
void AtualizaNavePos(Jogo *j);
void AtualizaHeroiPos(Jogo *j);
void DesenhaNaves(Jogo *j);
void DesenhaHeroi(Jogo *j);
void ColisaoBordas(Jogo *j);
void DesenhaBordas(Jogo *j);
int ColisaoBalas(Jogo *j);
void ColisaoNaveHeroi (Jogo *j);
void DesenhaBordas(Jogo *j);
void AtiraBalas(Jogo *j);
void CarregaImagens(Jogo *j);
void DescarregaImagens(Jogo *j);
void DebugHitbox (Jogo *j);
void AtualizaPlacar(Jogo *j);  
void ExibePlacar(Jogo *j);
void DesenhaBarreiras(Jogo *j);
void ColisaoBalasBarreiras(Jogo *j);
int VerificaVitoria(Jogo *j);
int ColisaoBalasHeroi(Jogo *j);
void CarregaPlacar(Jogo *j);
void DesenhaHeroi(Jogo *j);


int main(){
    InitAudioDevice();
    Jogo jogo;  //cria um jogo do tipo jogo (com nave, heroi, bordas e assets)

    jogo.alturaJanela = ALTURA_JANELA;    
    jogo.larguraJanela = LARGURA_JANELA;
    jogo.estado = placar_inicio;

    InitWindow(jogo.larguraJanela, jogo.alturaJanela, "Space Invaders");  //desenha a janela do jogo
    SetTargetFPS(60);  

    CarregaPlacar(&jogo);
    IniciaJogo(&jogo); 
    CarregaImagens(&jogo);

    Music musicaJogo = LoadMusicStream("../assets/musica.ogg");  
    PlayMusicStream(musicaJogo);

    while(!WindowShouldClose()){  //vai rodando o jogo até que a janela seja fechada (esc ou outra tecla definida)
        UpdateMusicStream(musicaJogo);  //vai "atualizando" a musica para que ela toque de forma contínua ao longo do jogo
        if(jogo.estado == placar_inicio){
            ExibePlacar(&jogo);

            if(IsKeyPressed(KEY_ENTER)){
                jogo.estado = inserindo_nome; 
                memset(jogo.apelido, 0, sizeof(jogo.apelido));   // apaga nomes que possam estar armazenado da última partida
                jogo.posicao_letra = 0;   
            }
        }else if (jogo.estado == inserindo_nome){
            //registra teclas pressionadas
            int key = GetCharPressed(); 
            while (key > 0) {
                if (jogo.posicao_letra < 19 && key >= 32 && key <= 126) {
                    jogo.apelido[jogo.posicao_letra++] = (char)key;
                    jogo.apelido[jogo.posicao_letra] = '\0';
                }
                key = GetCharPressed();
            }
            //apaga quando pressiona backspace
            if (IsKeyPressed(KEY_BACKSPACE) && jogo.posicao_letra > 0) {  //
                jogo.apelido[--jogo.posicao_letra] = '\0';
            }
            //finaliza quando pressiona enter
            if (IsKeyPressed(KEY_ENTER) && jogo.posicao_letra > 0) {
                jogo.estado = jogando;
                jogo.naves_derrotadas = 0; // zera o contador
                IniciaJogo(&jogo);
            }
            // desenha a tela de entrada
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("Nickname:", LARGURA_JANELA/2 - 100, 200, 20, WHITE);
            DrawText(jogo.apelido, LARGURA_JANELA/2 - 100, 250, 20, WHITE);
            EndDrawing();
        }

        if (IsKeyPressed(KEY_F2) && jogo.estado != placar_fim) { // verifica se F2 foi pressionado e não está no placar_fim
            jogo.estado = placar_fim;
            AtualizaPlacar(&jogo);
        }
        
        else if(jogo.estado == jogando){
            AtualizaFrameDesenho(&jogo);   // atualiza o jogo e desenha tudo
            if (VerificaVitoria(&jogo)) {
                if (jogo.fase < 5){
                    jogo.fase++;           // avança pra a próxima fase
                    jogo.heroi.vida = 3;     // reseta as vidas do herói
                    IniciaJogo(&jogo);       // reinicia naves, barreiras etc.
                }else{
                    jogo.vitoria = 1;
                    jogo.estado = placar_fim;
                    AtualizaPlacar(&jogo);
                }
            }

            if (jogo.derrota) {     // verifica derrota
                jogo.estado = placar_fim;
                AtualizaPlacar(&jogo);
            }
            
        }else if (jogo.estado ==  placar_fim){
            ExibePlacar(&jogo);
            if (IsKeyPressed(KEY_R)){    
                jogo.vitoria = 0;
                jogo.derrota = 0;
                jogo.estado = jogando;
                jogo.fase = 1; // reinicia pra fase 1
                jogo.naves_derrotadas = 0; // zera o contador de derrotas
                IniciaJogo(&jogo);
            }
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)){
                break;  // sai do loop e fecha o jogo
            }
        }
    }

    UnloadMusicStream(musicaJogo);
    DescarregaImagens(&jogo);
    CloseWindow();
    return 0;
}


void IniciaJogo(Jogo *j){
    //começa na fase 1
    if(j->fase < 1) {
        j->fase = 1;
    }

    //dimensoes da barreira
    int largura = 50;
    int altura = 25; 

    // posiciona as barreiras na tela 
    for (int i = 0; i < 4; i++) {
        j->barreiras[i].pos = (Rectangle){ 100 + i * 175, 350, largura, altura }; 
        j->barreiras[i].vida = 16;
    }

    //controla as animações
    j->tempoAnimação = GetTime();
    //inicializa tamanho e posição inicial do heroi
    j->heroi.pos = (Rectangle) {     
        (LARGURA_JANELA/2-40) - STD_SIZE_X/2,   
        (ALTURA_JANELA-10) - STD_SIZE_Y -70,    
        STD_SIZE_X, STD_SIZE_Y             
    };

    j->vitoria = 0;
    j->derrota = 0;
    j->heroi.vida = 3;
    j->heroi.velocidade = 6.5; 
    j->heroi.bala.velocidade = 6.5; 
    j->heroi.bala.ativa = 0;
    j->heroi.bala.tempo = GetTime();

    // espaçamento das naves inimigas (5 linhas x 7 colunas)
    float espacamento_x = 90.0f;
    float espacamento_y = 70.0f;
    float start_x = 50.0f;
    float start_y = 50.0f;

    // cria matriz 5x7, e posiciona as naves conforme o espaçamento definido
    for (int linha = 0; linha < NUM_LINHAS; linha++) {
        for (int coluna = 0; coluna < NAVES_POR_LINHA; coluna++) {
            Nave *n = &j->naves[linha][coluna];
            n->pos = (Rectangle){
                start_x + coluna * espacamento_x,
                start_y + linha * espacamento_y,
                TANAX, TANAY
            };

            //definições das naves
            n->atirando = 0;
            n->tempoUltimoTiro = 0.0;
            n->velocidade = 2;  
            n->direcao = 1;
            n->vida = 1;
            n->bala.velocidade = 2 + (j->fase - 1);   // ajusta a velocidade da bala conforme a fase
            n->bala.ativa = 0;
            n->bala.tempo = GetTime();
            n->bala.tiro = LoadSound("../assets/shoot.wav");
        }
    }

    //borda cima
    j->bordas[0].pos = (Rectangle){0, 0, LARGURA_JANELA, 10};
    //borda baixo
    j->bordas[1].pos = (Rectangle){0, ALTURA_JANELA-10, LARGURA_JANELA, 10};
    //borda esquerda
    j->bordas[2].pos = (Rectangle){0, 0, 10, ALTURA_JANELA};
    //borda direita
    j->bordas[3].pos = (Rectangle){LARGURA_JANELA-10, 0, 10, ALTURA_JANELA};

}

void AtualizaJogo(Jogo *j) {     // atualiza o estado do jogo a cada frame
    AtualizaNavePos(j);
    AtiraBalas(j);
    AtiraBalasHeroi(j);
    AtualizaHeroiPos(j);
    ColisaoBalasBarreiras(j);
    ColisaoBalas(j);
    ColisaoBalasHeroi(j);

    if (VerificaVitoria(j)) {
        j->vitoria = 1;
    }
}

void DesenhaJogo(Jogo *j){
    BeginDrawing();
    //desenha o fundo
    DrawTexture(j->assets.imagemfundo, 0, 0, WHITE);
    //desenha elementos principais
    ClearBackground(BLACK);
    DesenhaNaves(j);
    DesenhaHeroi(j);
    DesenhaBordas(j);
    DesenhaBarreiras(j);

    // desenha todas as balas das naves inimigas 
    for(int linha = 0; linha < NUM_LINHAS; linha++) {
        for(int coluna = 0; coluna < NAVES_POR_LINHA; coluna++) {
            if(j->naves[linha][coluna].bala.ativa) {
                DrawRectangleRec(j->naves[linha][coluna].bala.pos, YELLOW);
            }
        }
    }

    // desenha a bala do herói se for ativada
    if (j->heroi.bala.ativa) {
        DrawRectangleRec(j->heroi.bala.pos, YELLOW);
    }

    //desenha sprites da vida do herói
    for(int i=0; i < j->heroi.vida; i++){  
        DrawTextureEx(
            j->assets.vida,                                  
            (Vector2){LARGURA_JANELA - (i + 1.5) * 40, 10},  //posicionamento
            0.0f,                                           
            0.05f,  //escala                             
            WHITE                                         
        );
    }

    // exibe a fase atual
    char faseText[20];
    sprintf(faseText, "Fase %d", j->fase);
    DrawText(faseText, 10, 10, 20, WHITE);
    DebugHitbox(j);
    //finaliza frame + desenha tudo na tela
    EndDrawing();
}

void AtualizaFrameDesenho(Jogo *j){
    AtualizaJogo(j);
    DesenhaJogo(j);
}

void AtualizaNavePos(Jogo *j) { //move as naves inimigas 

    // 1 vai para direita, -1 vai para esquerda
    static int direcao_geral = 1; 
    float velocidade = 2.0f;
    bool mudar_direcao = false;

    // verifica se alguma nave atinge a borda esquerda ou direita
    for(int linha = 0; linha < NUM_LINHAS; linha++) {
        for(int coluna = 0; coluna < NAVES_POR_LINHA; coluna++) {
            Nave *n = &j->naves[linha][coluna];
            float proximoX = n->pos.x + velocidade * direcao_geral;
            if((proximoX + n->pos.width >= LARGURA_JANELA - 10) || (proximoX <= 10)) {
                mudar_direcao = true;
                break;
            }
        }
        if(mudar_direcao) break;  //evita verificações desnecessárias
    }

    //inverte a direção de todas as naves
    if(mudar_direcao) {
        direcao_geral *= -1;
    }

    // atualiza posição horizontal de todas as naves (move as naves)
    for(int linha = 0; linha < NUM_LINHAS; linha++) {
        for(int coluna = 0; coluna < NAVES_POR_LINHA; coluna++) {
            Nave *n = &j->naves[linha][coluna];
            n->pos.x += velocidade * direcao_geral;
            
        }
    }
}

void AtualizaHeroiPos(Jogo *j) {
    Rectangle hitboxHeroi = {
        j->heroi.pos.x + HEROI_HITBOX_X,
        j->heroi.pos.y + HEROI_HITBOX_Y,
        HEROI_HITBOX_LARGURA,
        HEROI_HITBOX_ALTURA
    };

    // movimento para DIREITA até chegar na borda
    if (IsKeyDown(KEY_RIGHT) && (hitboxHeroi.x + hitboxHeroi.width < 790)) {
        j->heroi.pos.x += j->heroi.velocidade;
    }

    // movimento para ESQUERDA até chegar na borda
    if (IsKeyDown(KEY_LEFT) && (hitboxHeroi.x > 10)) {
        j->heroi.pos.x -= j->heroi.velocidade;
    }
}

void CarregaImagens(Jogo *j){
    //j->assets.naveVerde = LoadTexture("assets/GreenAnimation.png");  
    j->assets.naveTexturas[0] = LoadTexture("../assets/img1.png");  
    j->assets.naveTexturas[1] = LoadTexture("../assets/img2.png");
    j->assets.naveTexturas[2] = LoadTexture("../assets/img3.png");
    j->assets.naveTexturas[3] = LoadTexture("../assets/img4.png");
    j->assets.naveAtirando = LoadTexture("../assets/img5.png");


    j->assets.heroiImagem = LoadTexture("../assets/elverton2.png");       
    j->assets.heroiImagemTiro = LoadTexture("../assets/elverton_tiro.png");
    j->assets.imagemfundo = LoadTexture("../assets/imagemfundo10.png");     
    j->assets.vida = LoadTexture("../assets/coracao.png");
    j->assets.barreira1 = LoadTexture("../assets/barreira1.png"); 
    j->assets.barreira2 = LoadTexture("../assets/barreira2.png");  
    j->heroi.bala.tiro = LoadSound("../assets/shoot.wav");
}

void DescarregaImagens(Jogo *j){
    //UnloadTexture(j->assets.naveVerde);
    for (int i = 0; i < 4; i++) {
        UnloadTexture(j->assets.naveTexturas[i]);
    }
        UnloadTexture(j->assets.naveAtirando);

    UnloadTexture(j->assets.heroiImagem);
    UnloadTexture(j->assets.heroiImagemTiro);  
    UnloadTexture(j->assets.imagemfundo);
    UnloadTexture(j->assets.vida);  
    UnloadTexture(j->assets.barreira1);
    UnloadTexture(j->assets.barreira2);
}

void CarregaPlacar(Jogo *j) {
    FILE *placar = fopen("placar.txt", "r");
    int qtdd = 0;

    if (placar) {
        //exibe apenas últimos 5 placares
        while (qtdd < 5 && fscanf(placar, "%19s %d %d", 
                j->placar_nomes[qtdd], 
                &j->placar[qtdd], 
                &j->placar_naves[qtdd]) == 3) {
            qtdd++;
        }
        fclose(placar);
    }
}


void DesenhaNaves(Jogo *j) {
    for(int linha = 0; linha < NUM_LINHAS; linha++) {
        for(int coluna = 0; coluna < NAVES_POR_LINHA; coluna++) {
            Nave *n = &j->naves[linha][coluna];
            if(n->vida > 0) {
                Texture2D tex;
                
                // Verifica se deve usar a textura de tiro
                if(n->atirando && (GetTime() - n->tempoUltimoTiro < 0.6)) { // Mostra por 0.6 segundos
                    tex = j->assets.naveAtirando;
                } else {
                    tex = j->assets.naveTexturas[linha];
                    n->atirando = 0; // Reseta o estado
                }
                
                DrawTexturePro(
                    tex,
                    (Rectangle){0, 0, tex.width, tex.height},
                    (Rectangle){n->pos.x, n->pos.y, TANAX, TANAY},
                    (Vector2){0, 0}, 
                    0.0f, 
                    WHITE
                );
            }
        }
    }
}

void DesenhaHeroi(Jogo *j) {
    // se passou mais de 0.3 segundos desde o tiro, volta para a sprite normal
    if (j->heroi.atirando && GetTime() - j->heroi.tempoUltimoTiro > 0.3) {
        j->heroi.atirando = 0;
    }

    // escolhe o sprite certo dependendo se o herói está atirando
    Texture2D spriteHeroi = j->heroi.atirando ? j->assets.heroiImagemTiro : j->assets.heroiImagem;

    // desenha o sprite correspondente
    DrawTexturePro(spriteHeroi, 
        (Rectangle){0, 0, spriteHeroi.width, spriteHeroi.height}, 
        (Rectangle){j->heroi.pos.x, j->heroi.pos.y, 120, 120}, 
        (Vector2){0, 0}, 
        0.0f, 
        WHITE);
}

void DesenhaBarreiras(Jogo *j) {
    //checa e desenha todas as barreiras vivas
    for (int i = 0; i < 4; i++) {
        if (j->barreiras[i].vida > 0) {  
            Texture2D spriteBarreira;  
            
            //alterna as sprites da barreira dependendo da vida
            if (j->barreiras[i].vida > 8) {
                spriteBarreira = j->assets.barreira1;  
            } else {
                spriteBarreira = j->assets.barreira2;  
            }

            float escala = 1.5f;  // ajusta escala da sprite da barreira

            //desenha as barreiras na tela
            DrawTexturePro(
                spriteBarreira,  
                (Rectangle){0, 0, spriteBarreira.width, spriteBarreira.height}, 
                (Rectangle){j->barreiras[i].pos.x, j->barreiras[i].pos.y, (j->barreiras[i].pos.width * escala), (j->barreiras[i].pos.height * escala)}, 
                (Vector2){0, 0}, 0.0f, WHITE);
        }
    }
}

void DesenhaBordas(Jogo *j){
    for(int i = 0; i < 4; i++){
        DrawRectangleRec(j->bordas[i].pos, LIGHTGRAY);
    }
}

void DesenhaBalas(Jogo *j) {
    // desenha balas de todas as naves inimigas
    for(int linha = 0; linha < NUM_LINHAS; linha++) {
        for(int coluna = 0; coluna < NAVES_POR_LINHA; coluna++) {
            if(j->naves[linha][coluna].bala.ativa) {
                DrawRectangleRec(j->naves[linha][coluna].bala.pos, YELLOW);
            }
        }
    }
}

void AtiraBalas(Jogo *j) {  //controla os tiros das naves
    
    //só permite que naves vivas atirem
    for (int coluna = 0; coluna < NAVES_POR_LINHA; coluna++) {
        int linhaAtiradora = -1;
        for (int linha = NUM_LINHAS - 1; linha >= 0; linha--) {
            if (j->naves[linha][coluna].vida > 0) {
                linhaAtiradora = linha;
                break;
            }
        }
        
        //verifica se a nave pode atirar baseado no tempo da última bala disparada e da probabilidade definida
        if (linhaAtiradora != -1) {
            Nave *n = &j->naves[linhaAtiradora][coluna];
            double tempoAtual = GetTime();

            // define a probabilidade e o cooldown com base na fase
            int probTiro = 3 + (j->fase - 1) * 1.6;  // aumenta em 2% a chance por fase
            double cooldown = 6.5 - (j->fase - 1) * 0.3; // diminui o tempo de cooldown a cada fase
            if(cooldown < 2.0) cooldown = 2.0; // não permitir cooldown menor que 2 segundos

            //se atender aos requisitos, atira a bala 
            if (n->vida > 0 && !n->bala.ativa &&(rand() % 100 < probTiro) &&(tempoAtual - n->bala.tempo > cooldown)) {
                 n->atirando = 1; // Ativa o estado de tiro
        n->tempoUltimoTiro = tempoAtual; // Registra o momento do tiro
                n->bala.ativa = 1;
                //posiciona a bala corretamente
                n->bala.pos = (Rectangle){
                    n->pos.x + n->pos.width / 2 - LARGURA_BALA / 2,
                    n->pos.y + n->pos.height,
                    LARGURA_BALA,
                    ALTURA_BALA
                };
                n->bala.tempo = tempoAtual;
                PlaySound(n->bala.tiro);
            }
        }
    }

    // atualiza a posição das balas até saírem da tela (movimenta a bala)
    for (int linha = 0; linha < NUM_LINHAS; linha++) {
        for (int coluna = 0; coluna < NAVES_POR_LINHA; coluna++) {
            Nave *n = &j->naves[linha][coluna];
            if (n->bala.ativa) {
                n->bala.pos.y += n->bala.velocidade;
                if (n->bala.pos.y > ALTURA_JANELA) {
                    n->bala.ativa = 0;
                }
            }
        }
    }
}

void AtiraBalasHeroi(Jogo *j) {
    if (IsKeyPressed(KEY_SPACE) && j->heroi.bala.ativa == 0 && (GetTime() - j->heroi.bala.tempo > 0.3)) {
        // posiciona a bala em relação ao herói
        j->heroi.bala.pos = (Rectangle){
            j->heroi.pos.x + (j->heroi.pos.width+25) - (LARGURA_BALA/2), 
            j->heroi.pos.y - ALTURA_BALA, LARGURA_BALA, ALTURA_BALA
        };

        //ativa o tiro
        j->heroi.bala.ativa = 1;
        j->heroi.bala.tempo = GetTime();
        j->heroi.atirando = 1;  
        j->heroi.tempoUltimoTiro = GetTime(); 
        PlaySound(j->heroi.bala.tiro);
    }

    //atualiza a posição das balas até saírem da tela (movimenta a bala)
    if (j->heroi.bala.ativa) {
        j->heroi.bala.pos.y -= j->heroi.bala.velocidade;
        if (j->heroi.bala.pos.y + ALTURA_BALA < 0) {
            j->heroi.bala.ativa = 0;
        }
    }
}

void ColisaoBordas(Jogo *j) {
    // verifica se alguma nave da primeira ou última coluna tocou as bordas
    bool colidiuEsquerda = false;
    bool colidiuDireita = false;

    // verifica apenas as naves das extremidades 
    for(int linha = 0; linha < NUM_LINHAS; linha++) {
        // coluna (direita)
        Nave *naveEsquerda = &j->naves[linha][0];
        if(naveEsquerda->vida > 0 && CheckCollisionRecs(naveEsquerda->pos, j->bordas[2].pos)) {
            colidiuEsquerda = true;
            break;
        }

        // coluna (direita)
        Nave *naveDireita = &j->naves[linha][NAVES_POR_LINHA - 1];
        if(naveDireita->vida > 0 && CheckCollisionRecs(naveDireita->pos, j->bordas[3].pos)) {
            colidiuDireita = true;
            break;
        }
    }

    // altera a direção de todas as naves se houver colisão
    if(colidiuEsquerda || colidiuDireita) {
        for(int linha = 0; linha < NUM_LINHAS; linha++) {
            for(int coluna = 0; coluna < NAVES_POR_LINHA; coluna++) {
                j->naves[linha][coluna].direcao *= -1; // Inverte a direção
            }
        }
    }
}

void DebugHitbox(Jogo *j) {
    // criando uma hitbox pra facilitar as checagens
    Rectangle hitboxHeroi = {  
        j->heroi.pos.x +HEROI_HITBOX_X,     
        j->heroi.pos.y +HEROI_HITBOX_Y,     
        HEROI_HITBOX_LARGURA , 
        HEROI_HITBOX_ALTURA  
    };
}

int VerificaVitoria(Jogo *j) {
    //verifica se todas as naves inimigas foram destruídas
    for(int linha = 0; linha < NUM_LINHAS; linha++) {
        for(int coluna = 0; coluna < NAVES_POR_LINHA; coluna++) {

            if(j->naves[linha][coluna].vida > 0) {
                return 0; // ainda existem naves vivas
            }
        }
    }
    return 1; // todas as naves destruídas (vitória)
}

void AtualizaPlacar(Jogo *j) {  
    FILE *placar = fopen("placar.txt", "r");

    if (placar == NULL) {
        printf("Erro ao abrir placar.txt para leitura\n");
        return; // Evita travamento do jogo
    }

    char nomes[5][20] = {0};
    int resultados[5] = {0}, naves[5] = {0}, qtdd = 0;

    //lê o arquivo
    if (placar) {
        while (qtdd < 5 && fscanf(placar, "%19s %d %d", nomes[qtdd], &resultados[qtdd], &naves[qtdd]) == 3) {
            qtdd++;
        }
        fclose(placar);
    }

    //remove o registro mais antigo
    if (qtdd == 5) {
        for (int i = 0; i < 4; i++) {
            strcpy(nomes[i], nomes[i+1]);
            resultados[i] = resultados[i+1];
            naves[i] = naves[i+1];
        }
        qtdd = 4;
    }

    //add novo resultado
    strcpy(nomes[qtdd], j->apelido);
    resultados[qtdd] = j->vitoria ? 1 : 0;
    naves[qtdd] = j->naves_derrotadas;
    qtdd++;

    //atualiza arquivo
    placar = fopen("placar.txt", "w");
    for (int i = 0; i < qtdd; i++) {
        fprintf(placar, "%s %d %d\n", nomes[i], resultados[i], naves[i]);
        strcpy(j->placar_nomes[i], nomes[i]);
        j->placar[i] = resultados[i];
        j->placar_naves[i] = naves[i];
    }
    fclose(placar);
}

void ExibePlacar(Jogo *j) {
    //desenha informações e placar na tela
    BeginDrawing();
    ClearBackground(BLACK);

    DrawText("SPACE INVADERS", LARGURA_JANELA / 2 - 140, 100, 30, GREEN);
    DrawText("Pressione ENTER para começar\n    Pressione R para reiniciar", LARGURA_JANELA / 2 - 165, 300, 20, YELLOW);

    // mensagem de vitória/derrota aparece apenas na tela final do placar
    if (j->estado == placar_fim) {  
        if (j->vitoria) {
            DrawText("Você Ganhou!", LARGURA_JANELA / 2 - 70, ALTURA_JANELA - 350, 20, GREEN);
        } else if (j->derrota) {
            DrawText("Você Perdeu!", LARGURA_JANELA / 2 - 70, ALTURA_JANELA - 350, 20, RED);
        }
    }

    // exibe os últimos 5 placares 
    for(int i = 0; i < 5; i++) {
        if (strlen(j->placar_nomes[i])) { // só exibe se o nome não estiver vazio
            char resultado_txt[50];
            sprintf(resultado_txt, "%s: %s - %d naves", 
                    j->placar_nomes[i],
                    j->placar[i] == 1 ? "VITÓRIA" : "DERROTA", 
                    j->placar_naves[i]);
            DrawText(resultado_txt, LARGURA_JANELA / 2 - 200, 380 + i * 30, 20, WHITE);
        }
    }
    EndDrawing();
}

int ColisaoBalas(Jogo *j) {
    //define hitbox para checagem
    Rectangle hitboxHeroi = {
        j->heroi.pos.x + HEROI_HITBOX_X,
        j->heroi.pos.y + HEROI_HITBOX_Y,
        HEROI_HITBOX_LARGURA,
        HEROI_HITBOX_ALTURA
    };

    // verifica se a bala do herói atingiu alguma nave inimiga
    if (j->heroi.bala.ativa) {
        for (int linha = 0; linha < NUM_LINHAS; linha++) {
            for (int coluna = 0; coluna < NAVES_POR_LINHA; coluna++) {
                Nave *n = &j->naves[linha][coluna];
                if (n->vida > 0 && CheckCollisionRecs(n->pos, j->heroi.bala.pos)) {  
                    // atingiu a nave
                    n->vida = 0;  
                    j->heroi.bala.ativa = 0;  
                    j->naves_derrotadas++;  
                    return 1;  // sai do loop imediatamente 
                }
            }
        }
    }

    // verifica se o herói foi atingido
    for (int linha = 0; linha < NUM_LINHAS; linha++) {
        for (int coluna = 0; coluna < NAVES_POR_LINHA; coluna++) {
            Nave *n = &j->naves[linha][coluna];
            if (n->bala.ativa && CheckCollisionRecs(hitboxHeroi, n->bala.pos)) {
                //herói atingido
                j->heroi.vida--;  
                n->bala.ativa = 0; 
                if (j->heroi.vida < 1) {  // se o herói fica sem vida --> derrota
                    j->derrota = 1;
                }
                return 1;  // sai do loop imediatamente
            }
        }
    }
    return 0;
}

int ColisaoBalasHeroi(Jogo *j) {
    //verifica se houve colisão entre a bala do herói e alguma nave
    for (int linha = 0; linha < NUM_LINHAS; linha++) {
        for (int coluna = 0; coluna < NAVES_POR_LINHA; coluna++) {
            Nave *n = &j->naves[linha][coluna];

            if (j->heroi.bala.ativa && n->vida > 0) {
                if (CheckCollisionRecs(n->pos, j->heroi.bala.pos)) {

                    //atualiza os dados 
                    n->vida = 0;
                    j->heroi.bala.ativa = 0;
                    j->naves_derrotadas++;

                    //se todas as naves foram destruídas, vitória
                    if (VerificaVitoria(j)) {
                        j->vitoria = 1;

                    }
                    return 1;  //após colisão
                } 
            }
        }
    }
    return 0;  //se não houve colisão
}

void ColisaoBalasBarreiras(Jogo *j) {
    //verifica colisões entre balas e barreiras

    //define hitbox barreiras para checagem
    for(int i = 0; i < 4; i++) {
        if(j->barreiras[i].vida > 0) {
            Rectangle hitboxBarreira = {
                j->barreiras[i].pos.x, 
                j->barreiras[i].pos.y, 
                j->barreiras[i].pos.width + 12, // ajusta a hitbox para a direita
                j->barreiras[i].pos.height
            };

            //verifica se bala herói atingiu alguma barreira
            if(j->heroi.bala.ativa && CheckCollisionRecs(j->heroi.bala.pos, hitboxBarreira)) { 
                j->heroi.bala.ativa = 0;
                j->barreiras[i].vida--;
            }

            //verifica se bala da nave atingiu alguma barreira
            for(int linha = 0; linha < NUM_LINHAS; linha++) {
                for(int coluna = 0; coluna < NAVES_POR_LINHA; coluna++) {
                    Nave *n = &j->naves[linha][coluna];
                    if(n->bala.ativa && CheckCollisionRecs(n->bala.pos, hitboxBarreira)) {
                        n->bala.ativa = 0;
                        j->barreiras[i].vida--;
                    }
                }
            }
        }   
    }
}