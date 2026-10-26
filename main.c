#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define LARGURA 880
#define ALTURA 600
#define TOTAL 27            // quantidade de estados (26 estados + DF)
#define TOTAL_PERGUNTAS 3   // quantidade de monumentos (perguntas) do jogo
#define PONTOS_POR_ACERTO 10

// Telas do jogo: a variavel "tela" guarda um destes numeros
#define TELA_MENU 0      // tela de entrada
#define TELA_TUTORIAL 1  // como jogar
#define TELA_CREDITOS 2  // creditos
#define TELA_JOGO 3      // perguntas
#define TELA_FIM 4       // resultado final

// Botoes da tela de entrada
#define TOTAL_BOTOES 3
#define BOTAO_L 260                    // largura do botao
#define BOTAO_A 50                     // altura do botao
#define BOTAO_X ((LARGURA - BOTAO_L) / 2)
#define BOTAO_Y 250                    // altura do primeiro botao
#define BOTAO_ESPACO 70                // distancia entre um botao e outro

// Botao VOLTAR (tutorial e creditos)
#define VOLTAR_L 180
#define VOLTAR_A 44
#define VOLTAR_X ((LARGURA - VOLTAR_L) / 2)
#define VOLTAR_Y 520

// ===== MAPA DO BRASIL =====
// O mapa e uma matriz de letras. Cada letra e um estado e cada letra
// vira um quadradinho de CELULA x CELULA pixels na tela. O ponto '.' e o mar.
// A letra de cada estado esta no vetor LETRAS, na mesma posicao do vetor siglas:
//   A=RR  B=AP  C=AM  D=PA  E=MA  F=CE  G=RN  H=AC  I=RO  J=MT  K=TO  L=PI  M=PE  N=PB
//   O=MS  P=GO  Q=DF  R=BA  S=SE  T=AL  U=SP  V=MG  W=ES  X=PR  Y=RJ  Z=SC  #=RS
#define MAPA_LINHAS 46
#define MAPA_COLUNAS 47
#define CELULA 8      // tamanho de cada quadradinho em pixels
#define MAPA_X 412    // canto esquerdo do mapa na tela
#define MAPA_Y 82     // topo do mapa na tela

char LETRAS[TOTAL + 1] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ#";

char *mapa[MAPA_LINHAS] = {
    "................A..............................",
    "..............AAA..............................",
    "............AAAAA.........BB...................",
    "............AAAAA.....D..BBB...................",
    ".....CCC....CCAAAA.DDDDDBBBBB..................",
    ".....CCCCCCCCCAAAADDDDDDDBBBB..................",
    ".....CCCCCCCCCAAACDDDDDDDBBDDD.................",
    ".....CCCCCCCCCACCCCDDDDDDDBDDDDDD..............",
    ".....CCCCCCCCCCCCCCCDDDDDDDDDDDDDEE............",
    ".....CCCCCCCCCCCCCCCCDDDDDDDDDDDDEEEEE.........",
    ".....CCCCCCCCCCCCCCCCDDDDDDDDDDDEEEEEELFFF.....",
    "...CCCCCCCCCCCCCCCCCDDDDDDDDDDDDEEEEELLFFFF....",
    ".CCCCCCCCCCCCCCCCCCCDDDDDDDDDDDEEEEEELLFFFFGGG.",
    ".CCCCCCCCCCCCCCCCCCDDDDDDDDDDDDKEEEEELLLFFFGGGG",
    "CCCCCCCCCCCCCCCCCCCDDDDDDDDDDDKKEEEELLLLFFNNNNN",
    "HHCCCCCCCCCCCCCCCCCDDDDDDDDDDDKKEELLLLLLMMMNNNN",
    ".HHHHCCCCCCCIICCCCCJDDDDDDDDDKKKKELLLLLMMMMMMMM",
    ".HHHHHHCCCCIIIIJJJJJJDDDDDDDDKKKKELLLLRRRRTTTT.",
    "....HHHHH.IIIIIJJJJJJJJJJJJJKKKKKELLRRRRRRSST..",
    "....HHH...IIIIIIJJJJJJJJJJJJKKKKKRRRRRRRRRSS...",
    "...........IIIIIIJJJJJJJJJJJKKKKKRRRRRRRRRRR...",
    ".............IIIIJJJJJJJJJJJKKKKKRRRRRRRRRR....",
    "...............IJJJJJJJJJJJJPPPPPRRRRRRRRR.....",
    "................JJJJJJJJJJJJPPPPPRRVRRRRRR.....",
    "................JJJJJJJJJJJPPPPPPVVVVVRRRR.....",
    ".................JJJJJJJJJPPPPPQQVVVVVVVVR.....",
    "...................JJJJJJPPPPPPQQVVVVVVVRR.....",
    "...................JOOOOJPPPPPPPVVVVVVVVRR.....",
    "...................OOOOOOPPPPVVVVVVVVVVWW......",
    "...................OOOOOOOOPVVVVVVVVVVVWW......",
    "...................OOOOOOOOUUUUUVVVVVVVWW......",
    "...................OOOOOOOOUUUUUVVVVVVYW.......",
    "...................OOOOOOOUUUUUUUVVVVYYY.......",
    "......................OOOXXUUUUUUVUYYY.........",
    "......................OOXXXXXUUUUUU............",
    ".......................XXXXXXXUU...............",
    ".......................XXXXXXXX................",
    "........................XXXXZXZ................",
    "........................ZZZZZZZ................",
    ".......................####ZZZZ................",
    ".....................########Z.................",
    "....................#########..................",
    "....................#########..................",
    "......................#####....................",
    "........................##.....................",
    ".........................#.....................",
};

// Recebe uma letra do mapa e devolve a posicao do estado (0 a 26).
// Se for mar ('.'), devolve -1.
int estado_da_letra(char letra)
{
    for (int i = 0; i < TOTAL; i++) {
        if (LETRAS[i] == letra) {
            return i;
        }
    }
    return -1;
}

// Recebe uma posicao da tela (x, y), por exemplo onde esta o mouse,
// e devolve qual estado esta ali. Devolve -1 se for mar ou fora do mapa.
int estado_na_tela(int x, int y)
{
    if (x < MAPA_X || y < MAPA_Y) {
        return -1; // a esquerda ou acima do mapa
    }

    // divide pelo tamanho do quadradinho para achar a coluna e a linha da matriz
    int coluna = (x - MAPA_X) / CELULA;
    int linha = (y - MAPA_Y) / CELULA;

    if (coluna >= MAPA_COLUNAS || linha >= MAPA_LINHAS) {
        return -1; // a direita ou abaixo do mapa
    }
    return estado_da_letra(mapa[linha][coluna]);
}

// Escreve um texto MAIOR que o normal, centralizado em x.
// A fonte embutida do Allegro e bem pequena (8 pixels), entao
// "esticamos" o desenho multiplicando o tamanho pela escala (2, 3...).
void escrever_grande(ALLEGRO_FONT *fonte, ALLEGRO_COLOR cor, float x, float y, int escala, char *texto)
{
    ALLEGRO_TRANSFORM t;
    al_identity_transform(&t);
    al_scale_transform(&t, escala, escala); // aumenta o tamanho
    al_use_transform(&t);
    al_draw_text(fonte, cor, x / escala, y / escala, ALLEGRO_ALIGN_CENTRE, texto);
    al_identity_transform(&t);              // volta para o tamanho normal
    al_use_transform(&t);
}

// Devolve 1 se o mouse (mx, my) esta dentro do retangulo, ou 0 se esta fora.
int mouse_dentro(int mx, int my, int x, int y, int largura, int altura)
{
    if (mx >= x && mx <= x + largura && my >= y && my <= y + altura) {
        return 1;
    }
    return 0;
}

// Desenha um botao com o texto no meio.
// Se "destacado" for 1 (mouse em cima), o botao fica verde com borda amarela.
void desenhar_botao(ALLEGRO_FONT *fonte, char *texto, int x, int y, int largura, int altura, int destacado)
{
    ALLEGRO_COLOR cor_fundo = al_map_rgb(32, 46, 88);
    ALLEGRO_COLOR cor_borda = al_map_rgb(80, 100, 160);
    if (destacado) {
        cor_fundo = al_map_rgb(70, 150, 80);
        cor_borda = al_map_rgb(240, 200, 40);
    }
    al_draw_filled_rounded_rectangle(x, y, x + largura, y + altura, 10, 10, cor_fundo);
    al_draw_rounded_rectangle(x, y, x + largura, y + altura, 10, 10, cor_borda, 3);
    escrever_grande(fonte, al_map_rgb(255, 255, 255), x + largura / 2, y + altura / 2 - 8, 2, texto);
}

int main()
{
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_font_addon();
    al_init_primitives_addon();
    al_init_image_addon();

    ALLEGRO_DISPLAY *janela = al_create_display(LARGURA, ALTURA);
    ALLEGRO_EVENT_QUEUE *fila = al_create_event_queue();
    ALLEGRO_FONT *fonte = al_create_builtin_font();

    al_set_window_title(janela, "Onde Fica?");

    al_register_event_source(fila, al_get_display_event_source(janela));
    al_register_event_source(fila, al_get_keyboard_event_source());
    al_register_event_source(fila, al_get_mouse_event_source());

    char *siglas[TOTAL] = {
        "RR", "AP", "AM", "PA", "MA", "CE", "RN",
        "AC", "RO", "MT", "TO", "PI", "PE", "PB",
        "MS", "GO", "DF", "BA", "SE", "AL",
        "SP", "MG", "ES", "PR", "RJ", "SC", "RS"
    };
    char *nomes[TOTAL] = {
        "Roraima", "Amapa", "Amazonas", "Para", "Maranhao", "Ceara", "Rio Grande do Norte",
        "Acre", "Rondonia", "Mato Grosso", "Tocantins", "Piaui", "Pernambuco", "Paraiba",
        "Mato Grosso do Sul", "Goias", "Distrito Federal", "Bahia", "Sergipe", "Alagoas",
        "Sao Paulo", "Minas Gerais", "Espirito Santo", "Parana", "Rio de Janeiro",
        "Santa Catarina", "Rio Grande do Sul"
    };

    // linha e coluna da matriz onde a sigla de cada estado e escrita.
    // Nos estados pequenos do litoral a sigla fica no mar, ao lado do estado.
    int rotulo_linha[TOTAL] = {
        3, 4, 11, 11, 12, 12, 12,
        17, 19, 21, 18, 15, 16, 14,
        30, 27, 25, 21, 19, 17,
        32, 28, 29, 35, 33, 38, 41
    };
    int rotulo_coluna[TOTAL] = {
        15, 26, 11, 25, 34, 40, 48,
        4, 13, 21, 30, 37, 48, 48,
        22, 26, 31, 38, 45, 47,
        30, 35, 43, 26, 40, 28, 24
    };

    // ===== PERGUNTAS =====
    // Cada pergunta usa a MESMA posicao em todos os vetores abaixo.
    // Exemplo: posicao 0 = Cristo Redentor, foto cristo.png, resposta 24 (RJ).
    char *monumentos[TOTAL_PERGUNTAS] = {
        "Cristo Redentor",
        "Teatro Amazonas",
        "Congresso Nacional"
    };
    char *arquivos[TOTAL_PERGUNTAS] = {
        "cristo.png",
        "teatro.png",
        "congresso.png"
    };
    // posicao do estado certo nos vetores siglas/nomes
    int respostas[TOTAL_PERGUNTAS] = {
        24, // RJ
        2,  // AM
        16  // DF
    };
    char *dicas[TOTAL_PERGUNTAS] = {
        "DICA: fica na regiao Sudeste e tem litoral.",
        "DICA: fica na regiao Norte e e o maior estado do Brasil.",
        "DICA: fica no Centro-Oeste e e onde esta a capital do pais."
    };
    char *curiosidades[TOTAL_PERGUNTAS] = {
        "Inaugurado em 1931, fica no alto do morro do Corcovado.",
        "Inaugurado em 1896 em Manaus, na epoca do ciclo da borracha.",
        "Projetado por Oscar Niemeyer e inaugurado em 1960 em Brasilia."
    };

    // carrega a foto de cada monumento
    ALLEGRO_BITMAP *fotos[TOTAL_PERGUNTAS];
    for (int p = 0; p < TOTAL_PERGUNTAS; p++) {
        fotos[p] = al_load_bitmap(arquivos[p]);
        if (fotos[p] == NULL) {
            // o jogo continua, mas mostra um quadro no lugar da foto
            printf("Aviso: nao achei o arquivo %s\n", arquivos[p]);
        }
    }

    // ===== TEXTOS DAS TELAS DE ENTRADA =====
    char *botoes[TOTAL_BOTOES] = { "JOGAR", "COMO JOGAR", "CREDITOS" };

    char *tutorial[6] = {
        "1. Olhe a foto e o nome do monumento.",
        "2. Clique no estado do mapa onde voce acha que ele fica.",
        "3. Acertou? Voce ganha 10 pontos.",
        "4. Errou? O estado certo aparece em amarelo no mapa.",
        "5. Aperte ENTER para ir para a proxima pergunta.",
        "6. Sao 3 perguntas. No final aparece a sua pontuacao."
    };

    char *desenvolvedores[3] = { "Arthur Pimentel", "Christopher Nithigale", "Pedro Queiroz" };

    // ===== VARIAVEIS DO JOGO =====
    int pergunta = 0;   // qual pergunta esta na tela (0, 1 ou 2)
    int pontos = 0;     // pontuacao: +10 por acerto
    int acertos = 0;    // quantas perguntas o jogador acertou
    int acertou[TOTAL_PERGUNTAS] = { 0 }; // 1 = acertou aquela pergunta, 0 = errou
    int escolhido = -1; // -1 = ainda nao clicou
    int dica = 0;       // 1 = mostrar a dica
    int tela = TELA_MENU; // o jogo comeca na tela de entrada
    int novo_jogo = 0;  // 1 = zerar tudo e comecar as perguntas
    int mouse_x = 0;    // posicao atual do mouse (para destacar o estado)
    int mouse_y = 0;
    int rodando = 1;

    while (rodando) {
        // ===== CORES USADAS NO JOGO =====
        ALLEGRO_COLOR branco = al_map_rgb(255, 255, 255);
        ALLEGRO_COLOR preto = al_map_rgb(0, 0, 0);
        ALLEGRO_COLOR amarelo = al_map_rgb(240, 200, 40);
        ALLEGRO_COLOR verde_claro = al_map_rgb(120, 230, 120);
        ALLEGRO_COLOR vermelho_claro = al_map_rgb(240, 110, 100);
        ALLEGRO_COLOR cinza = al_map_rgb(170, 180, 200);
        ALLEGRO_COLOR fundo = al_map_rgb(20, 30, 60);
        ALLEGRO_COLOR painel = al_map_rgb(32, 46, 88);
        ALLEGRO_COLOR borda = al_map_rgb(80, 100, 160);

        al_clear_to_color(fundo);

        if (tela == TELA_MENU) {
            // ===== TELA DE ENTRADA (MENU) =====
            escrever_grande(fonte, amarelo, LARGURA / 2, 80, 7, "ONDE FICA?");
            al_draw_text(fonte, branco, LARGURA / 2, 160, ALLEGRO_ALIGN_CENTRE,
                "Descubra em qual estado ficam os monumentos do Brasil!");
            al_draw_line(LARGURA / 2 - 200, 190, LARGURA / 2 + 200, 190, borda, 2);

            // desenha os 3 botoes, um embaixo do outro
            for (int b = 0; b < TOTAL_BOTOES; b++) {
                int y = BOTAO_Y + b * BOTAO_ESPACO;
                int destacado = mouse_dentro(mouse_x, mouse_y, BOTAO_X, y, BOTAO_L, BOTAO_A);
                desenhar_botao(fonte, botoes[b], BOTAO_X, y, BOTAO_L, BOTAO_A, destacado);
            }

            al_draw_text(fonte, cinza, LARGURA / 2, 540, ALLEGRO_ALIGN_CENTRE, "Clique em um botao. ESC = sair");
            al_draw_text(fonte, cinza, LARGURA / 2, 565, ALLEGRO_ALIGN_CENTRE, "Projeto Integrador");
        }
        else if (tela == TELA_TUTORIAL) {
            // ===== TELA COMO JOGAR =====
            al_draw_filled_rounded_rectangle(80, 30, LARGURA - 80, 500, 12, 12, painel);
            al_draw_rounded_rectangle(80, 30, LARGURA - 80, 500, 12, 12, borda, 3);
            escrever_grande(fonte, amarelo, LARGURA / 2, 55, 4, "COMO JOGAR");

            // escreve cada linha do vetor tutorial
            for (int i = 0; i < 6; i++) {
                al_draw_text(fonte, branco, 130, 120 + i * 28, 0, tutorial[i]);
            }

            // legenda das cores do mapa
            al_draw_text(fonte, amarelo, 130, 305, 0, "CORES DO MAPA");
            al_draw_filled_rectangle(130, 325, 146, 341, al_map_rgb(130, 210, 140));
            al_draw_text(fonte, branco, 160, 330, 0, "mouse em cima do estado");
            al_draw_filled_rectangle(130, 350, 146, 366, amarelo);
            al_draw_text(fonte, branco, 160, 355, 0, "resposta certa");
            al_draw_filled_rectangle(130, 375, 146, 391, al_map_rgb(210, 60, 50));
            al_draw_text(fonte, branco, 160, 380, 0, "escolha errada");

            // teclas
            al_draw_text(fonte, amarelo, 470, 305, 0, "TECLAS");
            al_draw_text(fonte, branco, 470, 330, 0, "D     = mostrar uma dica");
            al_draw_text(fonte, branco, 470, 355, 0, "ENTER = proxima pergunta");
            al_draw_text(fonte, branco, 470, 380, 0, "R     = reiniciar o jogo");
            al_draw_text(fonte, branco, 470, 405, 0, "ESC   = voltar ao menu");

            al_draw_text(fonte, cinza, LARGURA / 2, 455, ALLEGRO_ALIGN_CENTRE,
                "Dica: os estados pequenos do litoral tem a sigla escrita no mar.");

            desenhar_botao(fonte, "VOLTAR", VOLTAR_X, VOLTAR_Y, VOLTAR_L, VOLTAR_A,
                mouse_dentro(mouse_x, mouse_y, VOLTAR_X, VOLTAR_Y, VOLTAR_L, VOLTAR_A));
        }
        else if (tela == TELA_CREDITOS) {
            // ===== TELA DE CREDITOS =====
            al_draw_filled_rounded_rectangle(160, 30, LARGURA - 160, 500, 12, 12, painel);
            al_draw_rounded_rectangle(160, 30, LARGURA - 160, 500, 12, 12, borda, 3);
            escrever_grande(fonte, amarelo, LARGURA / 2, 55, 4, "CREDITOS");

            al_draw_text(fonte, amarelo, LARGURA / 2, 125, ALLEGRO_ALIGN_CENTRE, "DESENVOLVIDO POR");
            for (int i = 0; i < 3; i++) {
                escrever_grande(fonte, branco, LARGURA / 2, 145 + i * 28, 2, desenvolvedores[i]);
            }

            al_draw_text(fonte, amarelo, LARGURA / 2, 250, ALLEGRO_ALIGN_CENTRE, "ARTE VISUAL");
            escrever_grande(fonte, branco, LARGURA / 2, 270, 2, "Christopher e Arthur");

            al_draw_text(fonte, amarelo, LARGURA / 2, 330, ALLEGRO_ALIGN_CENTRE, "FOTO DO CRISTO REDENTOR");
            al_draw_text(fonte, branco, LARGURA / 2, 350, ALLEGRO_ALIGN_CENTRE,
                "Artyominc - Wikimedia Commons (CC BY-SA 3.0)");

            al_draw_line(LARGURA / 2 - 150, 400, LARGURA / 2 + 150, 400, borda, 2);
            al_draw_text(fonte, cinza, LARGURA / 2, 420, ALLEGRO_ALIGN_CENTRE, "Projeto Integrador");
            al_draw_text(fonte, cinza, LARGURA / 2, 440, ALLEGRO_ALIGN_CENTRE, "Feito em C com Allegro 5");

            desenhar_botao(fonte, "VOLTAR", VOLTAR_X, VOLTAR_Y, VOLTAR_L, VOLTAR_A,
                mouse_dentro(mouse_x, mouse_y, VOLTAR_X, VOLTAR_Y, VOLTAR_L, VOLTAR_A));
        }
        else if (tela == TELA_FIM) {
            // ===== TELA FINAL =====
            char texto[60]; // guarda um texto montado com numeros

            al_draw_filled_rounded_rectangle(140, 60, 740, 540, 12, 12, painel);
            al_draw_rounded_rectangle(140, 60, 740, 540, 12, 12, borda, 3);

            escrever_grande(fonte, amarelo, LARGURA / 2, 95, 4, "FIM DE JOGO!");

            snprintf(texto, sizeof(texto), "%d de %d pontos", pontos, TOTAL_PERGUNTAS * PONTOS_POR_ACERTO);
            escrever_grande(fonte, branco, LARGURA / 2, 160, 3, texto);

            snprintf(texto, sizeof(texto), "Acertos: %d de %d", acertos, TOTAL_PERGUNTAS);
            escrever_grande(fonte, branco, LARGURA / 2, 205, 2, texto);

            // linha separadora
            al_draw_line(200, 245, 680, 245, borda, 2);

            // resumo de cada pergunta
            for (int p = 0; p < TOTAL_PERGUNTAS; p++) {
                int y = 270 + p * 30;
                if (acertou[p]) {
                    al_draw_textf(fonte, verde_claro, LARGURA / 2, y, ALLEGRO_ALIGN_CENTRE,
                        "[OK] %s - %s", monumentos[p], nomes[respostas[p]]);
                }
                else {
                    al_draw_textf(fonte, vermelho_claro, LARGURA / 2, y, ALLEGRO_ALIGN_CENTRE,
                        "[X]  %s - %s", monumentos[p], nomes[respostas[p]]);
                }
            }

            al_draw_line(200, 365, 680, 365, borda, 2);

            // mensagem de acordo com a quantidade de acertos
            if (acertos == TOTAL_PERGUNTAS) {
                escrever_grande(fonte, amarelo, LARGURA / 2, 390, 2, "Excelente! Gabaritou!");
                al_draw_text(fonte, branco, LARGURA / 2, 425, ALLEGRO_ALIGN_CENTRE,
                    "Voce conhece muito bem o Brasil.");
            }
            else if (acertos > 0) {
                escrever_grande(fonte, amarelo, LARGURA / 2, 390, 2, "Bom trabalho!");
                al_draw_text(fonte, branco, LARGURA / 2, 425, ALLEGRO_ALIGN_CENTRE,
                    "Jogue de novo para tentar acertar todos.");
            }
            else {
                escrever_grande(fonte, amarelo, LARGURA / 2, 390, 2, "Nao foi dessa vez!");
                al_draw_text(fonte, branco, LARGURA / 2, 425, ALLEGRO_ALIGN_CENTRE,
                    "Use a dica (tecla D) e tente de novo.");
            }

            al_draw_text(fonte, cinza, LARGURA / 2, 500, ALLEGRO_ALIGN_CENTRE,
                "R ou ENTER = jogar de novo     ESC = menu");
        }
        else {
            // ===== TELA DA PERGUNTA =====

            // barra de cima: titulo, teclas e placar
            al_draw_filled_rectangle(0, 0, LARGURA, 56, painel);
            al_draw_line(0, 56, LARGURA, 56, borda, 2);
            escrever_grande(fonte, amarelo, 140, 16, 3, "ONDE FICA?");
            al_draw_text(fonte, cinza, 440, 24, ALLEGRO_ALIGN_CENTRE, "D = dica   R = reiniciar   ESC = menu");
            al_draw_textf(fonte, branco, LARGURA - 20, 14, ALLEGRO_ALIGN_RIGHT,
                "Pergunta %d de %d", pergunta + 1, TOTAL_PERGUNTAS);
            al_draw_textf(fonte, amarelo, LARGURA - 20, 34, ALLEGRO_ALIGN_RIGHT,
                "Pontos: %d   Acertos: %d", pontos, acertos);

            // nome e foto do monumento da pergunta atual
            escrever_grande(fonte, branco, 170, 66, 2, monumentos[pergunta]);
            if (fotos[pergunta] != NULL) {
                al_draw_bitmap(fotos[pergunta], 20, 90, 0);
            }
            else {
                // quadro no lugar da foto que nao foi encontrada
                al_draw_filled_rectangle(20, 90, 320, 450, painel);
                al_draw_text(fonte, branco, 170, 260, ALLEGRO_ALIGN_CENTRE, "Imagem nao encontrada:");
                al_draw_text(fonte, amarelo, 170, 280, ALLEGRO_ALIGN_CENTRE, arquivos[pergunta]);
                al_draw_text(fonte, cinza, 170, 310, ALLEGRO_ALIGN_CENTRE, "(coloque a foto na pasta do projeto)");
            }
            al_draw_rectangle(20, 90, 320, 450, branco, 3);

            // mapa
            al_draw_text(fonte, cinza, 600, 66, ALLEGRO_ALIGN_CENTRE,
                "Clique no estado onde fica o monumento:");
            // qual estado esta embaixo do mouse agora (-1 = nenhum)
            int estado_mouse = estado_na_tela(mouse_x, mouse_y);

            // percorre a matriz linha por linha, coluna por coluna
            for (int lin = 0; lin < MAPA_LINHAS; lin++) {
                for (int col = 0; col < MAPA_COLUNAS; col++) {
                    char letra = mapa[lin][col];
                    if (letra == '.') {
                        continue; // e mar: nao desenha nada
                    }

                    int i = estado_da_letra(letra);
                    int x = MAPA_X + col * CELULA;
                    int y = MAPA_Y + lin * CELULA;

                    ALLEGRO_COLOR cor = al_map_rgb(70, 150, 80); // verde normal
                    if (escolhido != -1 && i == respostas[pergunta]) {
                        cor = amarelo;                           // resposta certa
                    }
                    else if (i == escolhido) {
                        cor = al_map_rgb(210, 60, 50);           // escolha errada
                    }
                    else if (escolhido == -1 && i == estado_mouse) {
                        cor = al_map_rgb(130, 210, 140);         // verde claro (mouse em cima)
                    }
                    al_draw_filled_rectangle(x, y, x + CELULA, y + CELULA, cor);

                    // fronteira: desenha uma linha escura onde o vizinho
                    // e outro estado ou o mar (ou onde a matriz acaba)
                    if (lin == 0 || mapa[lin - 1][col] != letra) {
                        al_draw_line(x, y, x + CELULA, y, preto, 1);                     // em cima
                    }
                    if (lin == MAPA_LINHAS - 1 || mapa[lin + 1][col] != letra) {
                        al_draw_line(x, y + CELULA, x + CELULA, y + CELULA, preto, 1);   // embaixo
                    }
                    if (col == 0 || mapa[lin][col - 1] != letra) {
                        al_draw_line(x, y, x, y + CELULA, preto, 1);                     // esquerda
                    }
                    if (col == MAPA_COLUNAS - 1 || mapa[lin][col + 1] != letra) {
                        al_draw_line(x + CELULA, y, x + CELULA, y + CELULA, preto, 1);   // direita
                    }
                }
            }

            // escreve a sigla de cada estado
            for (int i = 0; i < TOTAL; i++) {
                int x = MAPA_X + rotulo_coluna[i] * CELULA + CELULA / 2;
                int y = MAPA_Y + rotulo_linha[i] * CELULA;
                // sigla em cima do estado = preta; sigla no mar = branca
                ALLEGRO_COLOR cor_sigla = preto;
                if (rotulo_coluna[i] >= MAPA_COLUNAS || mapa[rotulo_linha[i]][rotulo_coluna[i]] == '.') {
                    cor_sigla = branco;
                }
                al_draw_text(fonte, cor_sigla, x, y, ALLEGRO_ALIGN_CENTRE, siglas[i]);
            }

            // painel de mensagens embaixo
            al_draw_filled_rounded_rectangle(20, 465, LARGURA - 20, ALTURA - 10, 8, 8, painel);
            al_draw_rounded_rectangle(20, 465, LARGURA - 20, ALTURA - 10, 8, 8, borda, 2);

            if (escolhido == -1) {
                al_draw_text(fonte, branco, 40, 480, 0, "Clique em um estado no mapa...");
                if (dica) {
                    al_draw_text(fonte, amarelo, 40, 505, 0, dicas[pergunta]);
                }
                else {
                    al_draw_text(fonte, cinza, 40, 505, 0, "Precisa de ajuda? Aperte D para ver uma dica.");
                }
            }
            else {
                if (escolhido == respostas[pergunta]) {
                    char texto[40];
                    snprintf(texto, sizeof(texto), "ACERTOU! +%d pontos", PONTOS_POR_ACERTO);
                    escrever_grande(fonte, verde_claro, LARGURA / 2, 475, 2, texto);
                    al_draw_textf(fonte, branco, LARGURA / 2, 503, ALLEGRO_ALIGN_CENTRE,
                        "Parabens! Voce marcou %s corretamente.", nomes[respostas[pergunta]]);
                }
                else {
                    escrever_grande(fonte, vermelho_claro, LARGURA / 2, 475, 2, "QUASE!");
                    al_draw_textf(fonte, branco, LARGURA / 2, 503, ALLEGRO_ALIGN_CENTRE,
                        "Voce escolheu %s, mas a resposta certa era %s (em amarelo).",
                        nomes[escolhido], nomes[respostas[pergunta]]);
                }
                al_draw_text(fonte, cinza, LARGURA / 2, 523, ALLEGRO_ALIGN_CENTRE, curiosidades[pergunta]);

                if (pergunta < TOTAL_PERGUNTAS - 1) {
                    al_draw_text(fonte, amarelo, LARGURA / 2, 555, ALLEGRO_ALIGN_CENTRE,
                        ">> Aperte ENTER para a proxima pergunta <<");
                }
                else {
                    al_draw_text(fonte, amarelo, LARGURA / 2, 555, ALLEGRO_ALIGN_CENTRE,
                        ">> Aperte ENTER para ver o resultado <<");
                }
            }
        }

        al_flip_display();

        // espera o jogador fazer alguma coisa
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            rodando = 0;
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            int tecla = evento.keyboard.keycode;

            int enter = (tecla == ALLEGRO_KEY_ENTER || tecla == ALLEGRO_KEY_PAD_ENTER);

            if (tecla == ALLEGRO_KEY_ESCAPE) {
                if (tela == TELA_MENU) {
                    rodando = 0;      // no menu, ESC fecha o jogo
                }
                else {
                    tela = TELA_MENU; // nas outras telas, ESC volta ao menu
                }
            }
            else if (tecla == ALLEGRO_KEY_D && tela == TELA_JOGO) {
                dica = 1;
            }
            else if ((tecla == ALLEGRO_KEY_R && (tela == TELA_JOGO || tela == TELA_FIM)) ||
                     (enter && tela == TELA_FIM)) {
                novo_jogo = 1;        // reinicia o jogo inteiro
            }
            else if (enter && tela == TELA_JOGO && escolhido != -1) {
                // so passa de pergunta depois que o jogador ja respondeu
                if (pergunta < TOTAL_PERGUNTAS - 1) {
                    pergunta++;      // vai para a proxima pergunta
                    escolhido = -1;
                    dica = 0;
                }
                else {
                    tela = TELA_FIM; // era a ultima: mostra a tela final
                }
            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_AXES) {
            // o mouse se mexeu: guarda a posicao para pintar o estado embaixo dele
            mouse_x = evento.mouse.x;
            mouse_y = evento.mouse.y;
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && tela == TELA_MENU) {
            // ve em qual botao do menu o jogador clicou
            for (int b = 0; b < TOTAL_BOTOES; b++) {
                int y = BOTAO_Y + b * BOTAO_ESPACO;
                if (mouse_dentro(evento.mouse.x, evento.mouse.y, BOTAO_X, y, BOTAO_L, BOTAO_A)) {
                    if (b == 0) {
                        novo_jogo = 1;          // JOGAR
                    }
                    else if (b == 1) {
                        tela = TELA_TUTORIAL;   // COMO JOGAR
                    }
                    else {
                        tela = TELA_CREDITOS;   // CREDITOS
                    }
                }
            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN &&
                 (tela == TELA_TUTORIAL || tela == TELA_CREDITOS)) {
            // botao VOLTAR
            if (mouse_dentro(evento.mouse.x, evento.mouse.y, VOLTAR_X, VOLTAR_Y, VOLTAR_L, VOLTAR_A)) {
                tela = TELA_MENU;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && tela == TELA_JOGO && escolhido == -1) {
            // ve em qual estado o mouse clicou (-1 se clicou no mar ou fora do mapa)
            escolhido = estado_na_tela(evento.mouse.x, evento.mouse.y);

            // se clicou em um estado, confere a resposta e soma os pontos
            if (escolhido != -1 && escolhido == respostas[pergunta]) {
                pontos = pontos + PONTOS_POR_ACERTO;
                acertos++;
                acertou[pergunta] = 1;
            }
        }

        // comeca um jogo novo: volta para a primeira pergunta e zera tudo
        if (novo_jogo) {
            pergunta = 0;
            pontos = 0;
            acertos = 0;
            for (int p = 0; p < TOTAL_PERGUNTAS; p++) {
                acertou[p] = 0;
            }
            escolhido = -1;
            dica = 0;
            tela = TELA_JOGO;
            novo_jogo = 0;
        }
    }

    for (int p = 0; p < TOTAL_PERGUNTAS; p++) {
        if (fotos[p] != NULL) {
            al_destroy_bitmap(fotos[p]);
        }
    }
    al_destroy_font(fonte);
    al_destroy_event_queue(fila);
    al_destroy_display(janela);
    return 0;
}
