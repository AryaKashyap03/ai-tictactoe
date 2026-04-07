import { useState } from 'react'
import './App.css'

function App() {
  const [board, setBoard] = useState(Array(9).fill(' '));
  const [isAITurn, setIsAITurn] = useState(false);

  // 1. The Win/Draw Detection Engine
  const calculateWinner = (squares) => {
    const lines = [
      [0, 1, 2], [3, 4, 5], [6, 7, 8], // Rows
      [0, 3, 6], [1, 4, 7], [2, 5, 8], // Columns
      [0, 4, 8], [2, 4, 6]             // Diagonals
    ];
    for (let i = 0; i < lines.length; i++) {
      const [a, b, c] = lines[i];
      // If the spaces aren't empty and they all match, we have a winner!
      if (squares[a] !== ' ' && squares[a] === squares[b] && squares[a] === squares[c]) {
        return squares[a];
      }
    }
    // If there are no empty spaces left and no one won, it's a draw
    if (!squares.includes(' ')) return 'Draw';
    
    return null; // Game is still going
  }

  // React calculates this automatically every time the board updates
  const gameStatus = calculateWinner(board);

  const handleClick = async (index) => {
    // 2. The Lockout: If the game is over, or that square is taken, ignore clicks
    if (board[index] !== ' ' || isAITurn || gameStatus) return;

    // Human Move
    const newBoard = [...board];
    newBoard[index] = 'X';
    setBoard(newBoard);
    setIsAITurn(true);

    // If the human somehow wins on this move, stop here!
    if (calculateWinner(newBoard)) {
        setIsAITurn(false);
        return;
    }

    const boardString = newBoard.join('');

    try {
      const response = await fetch('http://localhost:8000/get-move', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ board: boardString })
      });

      if (!response.ok) throw new Error("Network response was not ok");

      const data = await response.json();
      const aiMoveIndex = data.move;

      // AI Move
      newBoard[aiMoveIndex] = 'O';
      setBoard(newBoard);
      
    } catch (error) {
      console.error("Error fetching AI move:", error);
    } finally {
      setIsAITurn(false);
    }
  };

  // 3. The Reset Function
  const resetGame = () => {
    setBoard(Array(9).fill(' '));
    setIsAITurn(false);
  };

  return (
    <div className="game-container">
      <h1 style={{color : 'white'}}>Unbeatable AI Tic-Tac-Toe</h1>
      
      {/* 4. The Status Display */}
      <div className="status-board">
        {gameStatus === 'Draw' ? (
            <h2 style={{ color: 'orange' }}>It's a Draw!</h2>
        ) : gameStatus === 'O' ? (
            <h2 style={{ color: 'red' }}>AI Wins!</h2>
        ) : gameStatus === 'X' ? (
            <h2 style={{ color: 'green' }}>You Win! (Wait, that's impossible...)</h2>
        ) : isAITurn ? (
            <h2 style={{color : 'white'}}>AI is thinking...</h2>
        ) : (
            <h2 style={{color : 'white'}}>Your Turn (X)</h2>
        )}
      </div>

      <div className="board">
        {board.map((cell, index) => (
          <div 
            key={index} 
            className="cell" 
            onClick={() => handleClick(index)}
          >
            {cell}
          </div>
        ))}
      </div>

      {/* Show the play again button only if the game is over */}
      {gameStatus && (
        <button className="reset-button" onClick={resetGame} style={{ marginTop: '20px', padding: '10px 20px', fontSize: '1.2rem', cursor: 'pointer', fontFamily : 'sans-serif', fontWeight:"bold"}}>
          Play Again
        </button>
      )}
    </div>
  )
}

export default App