import { sendCommand } from "./api.js";
import { displayFileButtons, displayResult } from "./ui.js";
import { CLIENT_URL } from "../config/config.js";

export async function getList() {
    const data = await sendCommand("getlist");
    displayFileButtons(data, onClick); // specific
}


export async function getFile() {
    const filename = document.getElementById("filename").value;
    const data = await sendCommand("getfile", filename);
    displayResult(data);
}


export async function startTracer() {
    const interval = document.getElementById("interval").value;
    const data = await sendCommand("starttracer", interval);
    displayResult(data);
}


export async function stopTracer() {
    const goFurther = confirm("Vous êtes sur le point d'arrêter le traceur.\nCliquez OK pour confirmer");
    if (goFurther) {
        const data = await sendCommand("stoptracer");
        displayResult(data);
    }
}


export async function getLogs() {
    const data = await sendCommand("getlogs");
    displayResult(data);
}


export async function fetchFileContent(file) {
    try {
        const response = await fetch(CLIENT_URL, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({
                command: "getfile",
                value: file
            })
        });

        const json = await response.json();
        displayResult(json);

    } catch (err) {
        displayResult({ error: "Erreur réseau" });
    }
}

export async function loadFileList() {
    try {
        const data = await sendCommand("getlist");

        if (data.data) {
            displayFileButtons(data.data, fetchFileContent);
        } else {
            displayResult(data);
        }

    } catch {
        displayResult({ error: "Erreur réseau" });
    }
}