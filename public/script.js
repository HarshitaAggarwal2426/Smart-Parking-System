// function sendCommand() {
//     const inputField = document.getElementById("commandInput");
//     const outputDiv = document.getElementById("terminalOutput");

//     const command = inputField.value;

//     fetch("/command", {
//         method: "POST",
//         headers: { "Content-Type": "application/json" },
//         body: JSON.stringify({ command })
//     })
//     .then(res => res.json())
//     .then(data => {
//         outputDiv.innerText += "\n> " + command + "\n";
//         outputDiv.innerText += data.output;
//         outputDiv.scrollTop = outputDiv.scrollHeight;
//     });

//     inputField.value = "";
// }