import { sendCommand } from "./api.js";
import { addDownloadButton, displayFileButtons, displayResult, displayTable, displayStats, clearStats, displaySelectedFile, resetSelectedFile } from "./ui.js";
import { parseCSV } from "./parser.js";
import { displayChart, clearChart } from "./graph.js"
import { openChartPanel, closeChartPanel } from "./app.js";

export async function getList() {
    clearStats();
    const data = await sendCommand("getlist");
    displayFileButtons(data, onClick); // specific
}


export async function getFile(file) {
    clearStats();
    const content = await sendCommand("getfile", file);

    if (content.error){
        displayResult(data);
        return;
    }

    parseCSV(content.data);
    return content;
}


export async function startTracer() {
    const interval = document.getElementById("interval").value;
    const data = await sendCommand("starttracer", interval);
    clearStats();
    clearChart();
    resetSelectedFile();
    closeChartPanel();
    displayResult(data);
    const alreadyEnabled = "";
    // tracer enabled
    if (data.status || data.error === alreadyEnabled){
        const statusText = document.getElementById("status-txt");
        statusText.classList.remove("unknown-status", "disabled-status");
        statusText.classList.add("enabled-status");
        statusText.innerText = "🟢 Statut : actif";
    }
}


export async function stopTracer() {
    clearStats();
    const data = await sendCommand("stoptracer");
    clearChart();
    resetSelectedFile();
    closeChartPanel();
    displayResult(data);
    // tracer disabled
    if (data.status){
        const statusText = document.getElementById("status-txt");
        statusText.classList.remove("unknown-status", "enabled-status");
        statusText.classList.add("disabled-status");
        statusText.innerText = "🔴 Statut : inactif";
    }
}


export async function getLogs() {
    clearStats();
    const data = await sendCommand("getlogs");
    clearChart();
    resetSelectedFile();
    closeChartPanel();
    displayResult(data);
}


export async function fetchFileContent(file) {
    clearStats();
    try {
        clearChart();
        const response = await getFile(file);

        if (response.error) {
            displayResult(response);
            return;
        }

        if (!response.data) {
            displayResult({ error: "No data received" });
            return;
        }

        const parsed = parseCSV(response.data);
        displaySelectedFile(file);
        displayTable(parsed);
        addDownloadButton(response.data, file);
        displayStats(parsed);
        displayChart(parsed.data);
        openChartPanel();

    } catch (err) {
        displayResult({ error: err.message || "Erreur réseau" });
    }
}


export async function loadFileList() {
    try {
        clearStats();
        const data = await sendCommand("getlist");

        if (data.data) {
            displayFileButtons(data.data, fetchFileContent);
        } else {
            clearChart();
            displayResult(data);
        }

    } catch (err) {
        displayResult({ error: err.message || "Erreur réseau" });
    }
}
