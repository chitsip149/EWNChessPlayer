# **EWN AI Players Using Monte Carlo Tree Search and Star0 Algorithms**

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

For more information on the game rule, please refer to [this link.](https://en.wikipedia.org/wiki/EinStein_w%C3%BCrfelt_nicht!)

---

## **Installation and Compilation**  

1. **Clone the repository:**  

```bash
git clone https://github.com/chitsip149/EWNChessPlayer.git
```

2. **Compile the AI players:**

```bash
cd EWN_random
g++ -o MyAI main.cpp MyAI.cpp -std=c++11
cd ..

cd EWN_MCTS
g++ -o MyAI main.cpp MyAI.cpp -std=c++11
cd ..

cd EWN_star0
g++ -o MyAI main.cpp MyAI.cpp -std=c++11
```

3. **Test the AI using the EWN game platform:**

The AI players can be tested using the official **[game platform](http://120.126.151.216/download.html)**.  
Follow the installation instructions on the platform's website to set it up and run the EWN AI players on your local device.  

After installing the platform and specifying the AI paths (paths to the `.exe` files generated after compiling the source code), the game interface should look like this:  

https://github.com/user-attachments/assets/373a7642-9199-4d7b-a01c-ef5b688c61de

## **Algorithms Implemented**

This project was developed as an **assignment for the course "Theory of Computer Games"** at  
**National Dong Hwa University, Department of Computer Science and Information Engineering**.

For more information on how the algorithms work, please refer to **[./report.pdf](./report.pdf)**.

### **1. Monte Carlo Tree Search (MCTS)**
- Uses **random rollouts** to estimate the best move.  
- Selects moves using the **Upper Confidence Bound (UCB)** formula.  
- Learns optimal strategies by **iteratively improving move selection**.  

### **2. Star0 Algorithm (Min-max with Alpha-beta Pruning)**
- Extends the **Min-max algorithm** to handle **chance nodes**.  
- Accounts for **probabilities of dice rolls** in decision-making.  
- Uses **Alpha-beta pruning** to reduce the number of states evaluated.  

---

## **Performance Evaluation**

Each AI was tested against a **random move generator** (which selects a legal move at random every turn) and against each other.

### **Test Results**
| AI Matchup        | Wins | Losses |
|-------------------|------|--------|
| MCTS vs Random   | 58   | 42     |
| Star0 vs Random  | 70   | 30     |
| MCTS vs Star0    | 44   | 56     |

Star0 performed better than MCTS due to its **strategic depth** in handling dice rolls.

---

## **Possible Enhancements**
- **Heuristic-based simulations** for MCTS instead of pure randomness.  
- **Parallelized MCTS** to speed up search and improve accuracy.  
- **Reinforcement Learning** for training an AI model to replace hand-tuned evaluations.  

---

## **References**
- Wang, Z., & Zhang, Y. (2013). Research of Einstein würfelt nicht! Monte Carlo on board game AI. *IEEE Conference on Computational Intelligence and Games (CIG)*. [Paper Link](https://www.semanticscholar.org/paper/Research-of-Einstein-w%C3%BCrfelt-nicht!-Monte-Carlo-on-Wang-Zhang/68b228d112094bd48932932bd85b2b22e1b47d2f).  

