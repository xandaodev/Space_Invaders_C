# Space Invaders (C + Raylib)

Uma recriação do clássico Space Invaders desenvolvida em C utilizando a biblioteca gráfica Raylib. O projeto foi desenvolvido como trabalho prático do primeiro período de Ciência da Computação na UFSJ.

## 🎮 Demonstração

![Gameplay](assets/preview.gif)

## 💻 Sobre o Projeto

Este jogo implementa as mecânicas clássicas de Space Invaders, incluindo:
- Movimentação de inimigos em bloco.
- Sistema de colisão (AABB).
- Barreiras destrutíveis.
- Animação de sprites.
- Sistema de pontuação e High Score persistente (arquivo txt).
- Áudio (efeitos e música de fundo).

**Tecnologias:**
- Linguagem C
- [Raylib](https://www.raylib.com/) (Gráficos e Áudio)

## 🚀 Como Rodar

Este jogo foi desenvolvido focado em Linux (ou WSL no Windows).

### Pré-requisitos
Você precisará do compilador GCC e da biblioteca Raylib instalada no seu sistema.

**No Ubuntu/Debian/WSL:**
```bash
# 1. Instale as ferramentas de compilação
sudo apt update
sudo apt install build-essential git

# 2. Instale a Raylib (se ainda não tiver)
sudo apt install libraylib-dev
