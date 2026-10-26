# Explicação do código — "Onde Fica?"

Este documento explica o jogo para a apresentação. Todo o código está no arquivo `main.c`.

---

## 1. Visão geral

O jogo funciona assim:

1. Mostra a foto de um monumento e um mapa do Brasil feito com retângulos.
2. O jogador clica no estado onde acha que o monumento fica.
3. O jogo diz se acertou ou errou e soma os pontos.
4. O jogador aperta `Enter` para ir à próxima pergunta.
5. Depois da 3ª pergunta aparece a tela final com o resultado.
6. Na tela final, `R` ou `Enter` começam tudo de novo.

---

## 2. O que cada parte do código faz

O `main.c` está dividido em blocos, na ordem em que aparecem:

| Parte | O que faz |
|---|---|
| `#include` | Traz as bibliotecas: `stdio.h` (para `printf` e `snprintf`) e os módulos do Allegro (janela, fonte, desenho de formas e imagens). |
| `#define` | Cria constantes: tamanho da janela, número de estados (`TOTAL`), número de perguntas (`TOTAL_PERGUNTAS`), pontos por acerto, o tamanho da matriz do mapa (`MAPA_LINHAS`, `MAPA_COLUNAS`), o tamanho de cada quadradinho (`CELULA`) e a posição do mapa na tela (`MAPA_X`, `MAPA_Y`). Se quiser mudar um valor, muda só aqui. |
| Mapa (`mapa` e `LETRAS`) | O mapa do Brasil escrito como uma matriz de letras. Cada letra é um estado e o `.` é o mar. `LETRAS` diz qual letra é de qual estado. |
| Funções `estado_da_letra` e `estado_na_tela` | A primeira transforma uma letra do mapa na posição do estado (0 a 26). A segunda recebe um ponto da tela (por exemplo, onde o mouse está) e diz qual estado está ali. |
| Função `escrever_grande` | Desenha um texto maior que o normal. A fonte do Allegro é pequena (8 pixels), então a função "estica" o desenho (escala 2, 3 ou 4) e depois volta ao tamanho normal. É usada no título, no nome do monumento e na tela final. |
| Início do `main` | Liga o Allegro (`al_init`), o teclado, o mouse e os addons. Cria a janela, a fila de eventos e a fonte. |
| Vetores dos estados | `siglas` e `nomes` guardam a sigla e o nome dos 27 estados. `rotulo_linha` e `rotulo_coluna` dizem onde escrever a sigla de cada estado no mapa. |
| `PERGUNTAS` | Vetores com os dados de cada pergunta: nome do monumento, arquivo da foto, resposta certa, dica e curiosidade. |
| Carregar fotos | Um laço `for` carrega a foto de cada monumento. Se a foto não existir, o jogo avisa no console e continua. |
| `VARIAVEIS DO JOGO` | Guardam o estado do jogo: pergunta atual, pontos, acertos, estado escolhido, dica, tela final e posição do mouse. |
| `while (rodando)` | O laço principal. Cada volta desenha a tela e espera uma ação do jogador. |
| `CORES USADAS NO JOGO` | Cria as cores (branco, amarelo, verde, vermelho etc.) com `al_map_rgb(vermelho, verde, azul)`. |
| `TELA FINAL` | Desenhada quando `fim_de_jogo` vale 1: pontuação, acertos, resumo de cada pergunta e mensagem final. |
| `TELA DA PERGUNTA` | Barra de cima com placar, foto do monumento, mapa e painel de mensagens embaixo. O mapa é desenhado com dois `for`, um dentro do outro, que passam por todas as posições da matriz. |
| Tratamento de eventos | Depois de desenhar, `al_wait_for_event` espera o jogador. Um `if / else if` decide o que fazer: fechar, tecla apertada, mouse mexeu ou mouse clicou. |
| Final do `main` | Libera da memória as fotos, a fonte, a fila e a janela (`al_destroy_...`). |

---

## 3. Conceitos básicos de C usados

| Conceito | Onde aparece no jogo |
|---|---|
| **Variáveis `int`** | `pontos`, `acertos`, `pergunta`, `escolhido`, `dica`, `fim_de_jogo`, `mouse_x`, `mouse_y`. |
| **Constantes (`#define`)** | `TOTAL`, `TOTAL_PERGUNTAS`, `PONTOS_POR_ACERTO`, `LARGURA`, `ALTURA`. |
| **Vetores** | `siglas[27]`, `nomes[27]`, `rotulo_linha[27]`, `rotulo_coluna[27]`, `monumentos[3]`, `respostas[3]`, `dicas[3]`, `acertou[3]`, `fotos[3]`. |
| **Matriz** | O `mapa`: cada linha é um texto e cada letra é uma coluna. `mapa[linha][coluna]` pega uma letra. |
| **Vetores de texto (`char *`)** | Cada posição guarda um texto, por exemplo `nomes[24]` é `"Rio de Janeiro"`. |
| **Condicionais (`if / else if / else`)** | Verificar acerto/erro, escolher a cor de cada estado, desenhar as fronteiras, escolher a mensagem final, descobrir qual tecla foi apertada. |
| **Laço `while`** | O laço principal do jogo (`while (rodando)`), que só para quando `rodando` vira 0. |
| **Laço `for`** | Dois `for` aninhados percorrem a matriz para desenhar o mapa; outro `for` procura a letra no vetor `LETRAS`; carregar as 3 fotos; zerar o vetor `acertou` ao reiniciar. |
| **Operadores de comparação e lógicos** | `>=`, `<=`, `==`, `!=`, `&&` (e), `\|\|` (ou), `!` (não). |
| **Funções** | `estado_da_letra`, `estado_na_tela` e `escrever_grande`: recebem parâmetros e algumas devolvem um valor com `return`. |
| **Divisão inteira** | `(x - MAPA_X) / CELULA` descobre a coluna da matriz: em C, dividir `int` por `int` descarta a parte decimal. |
| **`continue`** | Ao desenhar o mapa, pula as posições que são mar. |
| **`printf` / `snprintf`** | `printf` escreve aviso no console; `snprintf` monta um texto com números (ex.: `"20 de 30 pontos"`). |
| **Ponteiros (uso simples)** | Os tipos do Allegro (`ALLEGRO_DISPLAY *`, `ALLEGRO_BITMAP *` etc.). Comparamos com `NULL` para saber se a foto carregou. |

---

## 4. Como funciona a pontuação

São duas variáveis começando em zero:

```c
int pontos = 0;  // +10 por acerto
int acertos = 0; // quantas perguntas acertou
```

Quando o jogador clica em um estado, o jogo compara com a resposta da pergunta atual:

```c
if (escolhido != -1 && escolhido == respostas[pergunta]) {
    pontos = pontos + PONTOS_POR_ACERTO; // soma 10
    acertos++;                            // soma 1
    acertou[pergunta] = 1;                // marca que acertou esta pergunta
}
```

- `escolhido != -1` garante que o clique foi em cima de um estado (e não no fundo da tela).
- Cada pergunta só pode ser respondida **uma vez**, porque o clique só é aceito quando `escolhido == -1`. Assim não dá para ganhar pontos clicando várias vezes.
- A pontuação máxima é `TOTAL_PERGUNTAS * PONTOS_POR_ACERTO` = 3 × 10 = **30**.
- O vetor `acertou` serve para a tela final mostrar `[OK]` ou `[X]` em cada monumento.

---

## 5. Como funciona a troca de perguntas

A variável `pergunta` guarda a **posição** da pergunta atual nos vetores (0, 1 ou 2). Como todos os vetores usam a mesma posição, basta mudar `pergunta` para trocar tudo de uma vez:

| `pergunta` | `monumentos[...]` | `arquivos[...]` | `respostas[...]` |
|---|---|---|---|
| 0 | Cristo Redentor | cristo.png | 24 (RJ) |
| 1 | Teatro Amazonas | teatro.png | 2 (AM) |
| 2 | Congresso Nacional | congresso.png | 16 (DF) |

Quando o jogador aperta `Enter` **depois de responder**:

```c
if (pergunta < TOTAL_PERGUNTAS - 1) {
    pergunta++;      // vai para a próxima pergunta
    escolhido = -1;  // limpa a escolha
    dica = 0;        // esconde a dica
}
else {
    fim_de_jogo = 1; // era a última: mostra a tela final
}
```

Para **reiniciar**, o jogo volta todas as variáveis ao valor inicial: `pergunta = 0`, `pontos = 0`, `acertos = 0`, `escolhido = -1`, `dica = 0`, `fim_de_jogo = 0`, e um `for` zera o vetor `acertou`.

**Para adicionar uma 4ª pergunta:** mudar `TOTAL_PERGUNTAS` para 4 e acrescentar um item em cada vetor de perguntas (monumento, arquivo, resposta, dica, curiosidade).

---

## 6. Como o mouse identifica o estado escolhido

O mapa é uma **matriz de letras**. Cada letra vira um quadradinho de 8 × 8 pixels (`CELULA`) na tela. Um pedaço do mapa (Sudeste) fica assim:

```
OOUUUUUUUVVVVYYY
OXXUUUUUUVUYYY..
```

`U` é São Paulo, `V` é Minas Gerais, `Y` é Rio de Janeiro e `.` é o mar. Por isso o desenho tem o formato do Brasil.

Para saber qual estado está embaixo do mouse, é só fazer a conta ao contrário: dividir a posição do mouse pelo tamanho do quadradinho.

```c
int coluna = (x - MAPA_X) / CELULA;
int linha = (y - MAPA_Y) / CELULA;
return estado_da_letra(mapa[linha][coluna]);
```

- `MAPA_X` e `MAPA_Y` são o canto do mapa na tela, por isso são subtraídos primeiro.
- A divisão é inteira, então o resultado é o número da coluna e da linha da matriz.
- Com a linha e a coluna, `mapa[linha][coluna]` dá a letra, e `estado_da_letra` transforma a letra na posição do estado.

Exemplo com um clique no Rio de Janeiro, no ponto (712, 342):
`coluna = (712 − 412) / 8 = 37` e `linha = (342 − 82) / 8 = 32`. A letra em `mapa[32][37]` é `Y`, que é o RJ.

Antes da conta, a função confere se o clique está dentro do mapa. Se o clique for no mar ou fora do mapa, ela devolve `-1`, `escolhido` continua `-1` e nada acontece.

O **destaque verde-claro** usa a mesma função: quando o mouse se mexe (evento `ALLEGRO_EVENT_MOUSE_AXES`), o jogo guarda a posição em `mouse_x` e `mouse_y`, descobre o estado embaixo dele e pinta todos os quadradinhos daquele estado.

As **fronteiras** são linhas pretas desenhadas onde a letra do vizinho (em cima, embaixo, à esquerda ou à direita) é diferente.
