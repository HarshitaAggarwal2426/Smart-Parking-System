const express = require("express");
const { spawn } = require("child_process");
const cors = require("cors");

const app = express();

app.use(cors());
app.use(express.json());
app.use(express.static("public"));

app.post("/run", (req, res) => {
    const { input } = req.body;

    // const cppProgram = spawn("parking.exe");
    const cppProgram = spawn("./parking");

    let output = "";
    let maxSize = 10000; // limit output

    cppProgram.stdin.write(input);
    cppProgram.stdin.end();

    cppProgram.stdout.on("data", (data) => {
        if (output.length < maxSize) {
            output += data.toString();
        }
    });

    cppProgram.on("close", () => {
        res.send(output);
    });
});

// app.listen(3000, () => {
//     console.log("Server running on http://localhost:3000");
// });
const PORT = process.env.PORT || 3000;

app.listen(PORT, () => {
    console.log("Server running on port " + PORT);
});