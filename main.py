from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
import subprocess
import sys

app = FastAPI()


app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"], 
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Define the expected JSON payload
class BoardState(BaseModel):
    board: str
    size: int

@app.post("/get-move")
async def get_best_move(data: BoardState):
    # Quick validation
    if len(data.board) != data.size * data.size:
        raise HTTPException(status_code=400,detail="Invalid board size.")

    try:
        # Detect if the server is Windows or Linux
        executable = ".\\game_ai.exe" if sys.platform == "win32" else "./game_ai"
        result = subprocess.run(
            [executable, data.board,str(data.size)], 
            capture_output=True, 
            text=True, 
            check=True
        )
        
        # Grab the printed number from the C++ output and strip any invisible newline characters
        move_index = int(result.stdout.strip())
        
        # Return it as clean JSON
        return {"move": move_index}

    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))
