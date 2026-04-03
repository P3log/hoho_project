import { getFile, startTracer, stopTracer, getLogs, loadFileList } from "./actions.js";
import { withLoading, displayTable, displayStats, clearStats, displaySelectedFile, resetSelectedFile } from "./ui.js";
import { parseCSV } from "./parser.js";
import { clearChart, displayChart, resizeChart } from "./graph.js";


window.getFile = getFile;
window.startTracer = startTracer;
window.getLogs = getLogs;
window.openChartPanel = openChartPanel; // open the chart by default


// List files
document.getElementById("btnList").addEventListener("click", function () {
    withLoading(this, loadFileList);
});

// stop tracer
document.getElementById("btnStop").addEventListener("click", function () {
    const goFurther = confirm("Vous êtes sur le point d'arrêter le traceur.\nCliquez OK pour confirmer");
    if (goFurther) {
        withLoading(this, stopTracer);
    }
    resetSelectedFile();
});

// Upload a file
document.getElementById("fileInput").addEventListener("change", function (event) {
    const file = event.target.files[0];
    if (!file) return;

    const reader = new FileReader();

    reader.onload = function (e) {
        const content = e.target.result;
        
        // clear display
        output.classList.remove("error");

        // Simulate a response from the server
        clearStats();
        clearChart();
        const parsed = parseCSV(content);
        displaySelectedFile(file.name); // todo assess
        displayTable(parsed);
        displayStats(parsed);
        displayChart(parsed.data);
        openChartPanel();
    };

    reader.readAsText(file);
});


// Button to display / hide the graph (accordion)
const btn = document.getElementById("toggleChartBtn");
const container = document.getElementById("chartContainer");

btn.addEventListener("click", () => {
    const isExpanded = container.classList.toggle("expanded");

    btn.textContent = isExpanded
        ? "Masquer le graphique "
        : "Afficher le graphique ";

    if (isExpanded) {
        setTimeout(() => {
            resizeChart();
        }, 300); // wait for the animation to end
    }
});


export function openChartPanel() {
    const container = document.getElementById("chartContainer");
    const btn = document.getElementById("toggleChartBtn");

    if (!container.classList.contains("expanded")) {
        container.classList.add("expanded");
        btn.textContent = "Masquer le graphique";

        // wait CSS animation
        setTimeout(() => {
            resizeChart();
        }, 300);
    } else {
        // already open → resize only
        resizeChart();
    }
}

export function closeChartPanel() {
    const container = document.getElementById("chartContainer");
    const btn = document.getElementById("toggleChartBtn");

    if (container.classList.contains("expanded")) {
        container.classList.remove("expanded");

        btn.textContent = "Afficher le graphique";
    }
}