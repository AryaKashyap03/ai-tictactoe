import { useState } from "react";
import "./App.css";

function App() {
  const [boardSize, setBoardSize] = useState(3);
  const [board, setBoard] = useState(Array(9).fill(" "));
  const [isAITurn, setIsAITurn] = useState(false);

  // 1. The Win/Draw Detection Engine
  const calculateWinner = (squares) => {
    // Rows
    for (let i = 0; i < boardSize; i++) {
      const first = i * boardSize;

      if (squares[first] === " ") continue;

      let win = true;

      for (let j = 1; j < boardSize; j++) {
        if (squares[first + j] !== squares[first]) {
          win = false;
          break;
        }
      }

      if (win) return squares[first];
    }

    // Columns

    for (let i = 0; i < boardSize; i++) {
      if (squares[i] === " ") continue;

      let win = true;

      for (let j = 1; j < boardSize; j++) {
        if (squares[i + j * boardSize] !== squares[i]) {
          win = false;
          break;
        }
      }

      if (win) return squares[i];
    }

    // Main diagonal

    if (squares[0] !== " ") {
      let win = true;

      for (let i = 1; i < boardSize; i++) {
        if (squares[i * (boardSize + 1)] !== squares[0]) {
          win = false;
          break;
        }
      }

      if (win) return squares[0];
    }

    // Other diagonal

    const start = boardSize - 1;

    if (squares[start] !== " ") {
      let win = true;

      for (let i = 1; i < boardSize; i++) {
        if (squares[start + i * (boardSize - 1)] !== squares[start]) {
          win = false;
          break;
        }
      }

      if (win) return squares[start];
    }

    if (!squares.includes(" ")) return "Draw";

    return null;
  };

  // React calculates this automatically every time the board updates
  const gameStatus = calculateWinner(board);

  const handleClick = async (index) => {
    // 2. The Lockout: If the game is over, or that square is taken, ignore clicks
    if (board[index] !== " " || isAITurn || gameStatus) return;

    // Human Move
    const newBoard = [...board];
    newBoard[index] = "X";
    setBoard(newBoard);
    setIsAITurn(true);

    // If the human somehow wins on this move, stop here!
    if (calculateWinner(newBoard)) {
      setIsAITurn(false);
      return;
    }

    const boardString = newBoard.join("");

    try {
      const response = await fetch(
        "https://ai-tictactoe-3wxo.onrender.com/get-move",
        {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({
            board: boardString,
            size: boardSize,
          }),
        },
      );

      if (!response.ok) throw new Error("Network response was not ok");

      const data = await response.json();
      const aiMoveIndex = data.move;

      // AI Move
      newBoard[aiMoveIndex] = "O";
      setBoard(newBoard);
    } catch (error) {
      console.error("Error fetching AI move:", error);
    } finally {
      setIsAITurn(false);
    }
  };

  // 3. The Reset Function
  const resetGame = () => {
    setBoard(Array(boardSize * boardSize).fill(" "));
    setIsAITurn(false);
  };
  const changeBoardSize = (size) => {
    setBoardSize(size);
    setBoard(Array(size * size).fill(" "));
    setIsAITurn(false);
  };

  return (
    <div className="game-container">
      <h1 className ="title">Unbeatable AI Tic-Tac-Toe</h1>
      <div style={{ marginBottom: "20px" }}>
        <label style={{ color: "white", marginRight: "10px" }}>
          Board Size
        </label>

        <select
          value={boardSize}
          onChange={(e) => changeBoardSize(Number(e.target.value))}
        >
          <option value={3}>3 x 3</option>
          <option value={4}>4 x 4</option>
          <option value={5}>5 x 5</option>
        </select>
      </div>

      {/* 4. The Status Display */}
      <div className="status-board">
        {gameStatus === "Draw" ? (
          <h2 style={{ color: "orange" }}>It's a Draw!</h2>
        ) : gameStatus === "O" ? (
          <h2 style={{ color: "red" }}>AI Wins!</h2>
        ) : gameStatus === "X" ? (
          <h2 style={{ color: "green" }}>
            You Win! (Wait, that's impossible...)
          </h2>
        ) : isAITurn ? (
          <h2 style={{ color: "white" }}>AI is thinking...</h2>
        ) : (
          <h2 style={{ color: "white" }}>Your Turn (X)</h2>
        )}
      </div>

      <div
        className="board"
        style={{
          gridTemplateColumns: `repeat(${boardSize},100px)`,
        }}
      >
        {board.map((cell, index) => (
          <div
            key={index}
            className="cell"
            onClick={() => handleClick(index)}
            style={{
              width: boardSize === 3 ? 100 : boardSize === 4 ? 80 : 65,

              height: boardSize === 3 ? 100 : boardSize === 4 ? 80 : 65,

              fontSize:
                boardSize === 3 ? "3rem" : boardSize === 4 ? "2.5rem" : "2rem",
            }}
          >
            {cell}
          </div>
        ))}
      </div>

      {/* Show the play again button only if the game is over */}
      {gameStatus && (
        <button
          className="reset-button"
          onClick={resetGame}
          style={{
            marginTop: "20px",
            padding: "10px 20px",
            fontSize: "1.2rem",
            cursor: "pointer",
            fontFamily: "sans-serif",
            fontWeight: "bold",
          }}
        >
          Play Again
        </button>
      )}
    </div>
  );
}

export default App;
