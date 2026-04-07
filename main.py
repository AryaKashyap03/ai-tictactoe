from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
import subprocess
import sys # Added this import!

app = FastAPI()

# This allows your React frontend (usually running on port 3000 or 5173) to securely talk to this API
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

@app.post("/get-move")
async def get_best_move(data: BoardState):
    # Quick validation
    if len(data.board) != 9:
        raise HTTPException(status_code=400, detail="Board string must be exactly 9 characters.")

    try:
        # Detect if the server is Windows or Linux
        executable = ".\game2.exe" if sys.platform == "win32" else "./game2"
        
        result = subprocess.run(
            [executable, data.board], 
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