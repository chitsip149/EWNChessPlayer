# **EWN AI Player Using Monte Carlo Tree Search and Star0 Algorithm**

## **Overview**  
This project implements an **AI player for Einstein Würfelt Nicht! (EWN)**, a two-player board game that involves both strategy and chance. The AI is built using two algorithms:  

1. **Monte Carlo Tree Search (MCTS)** – Uses **random simulations** and the **Upper Confidence Bound (UCB)** formula to make decisions.  
2. **Min-max with Alpha-beta Pruning (Star0 Algorithm)** – Incorporates **chance nodes** to account for **dice roll probabilities**, improving strategic play.  

Each AI was tested against a **random move generator** (which selects moves randomly every turn) and against each other to evaluate their performance.

---

## **Game Rules**  
EWN is played on a **5x5 grid**, where each player has **six pieces (numbered 1 to 6)**. The goal is to:  

- Move a piece to the opponent’s **starting corner**, or  
- Capture all of the opponent’s pieces.  

Each turn, a player rolls a **six-sided die**, determining which piece they must move. If the piece has been captured, they can move any remaining piece. Pieces move diagonally toward the opponent's side and can capture opponent pieces by landing on them.

---

## **Installation and Compilation**  
Clone the repository and navigate to the project directory:  

```bash
git clone https://github.com/chitsip149/EWNChessPlayer.git
```
