
// Display the concerned field content and not the JSON
export function displayResult(data) {
    const output = document.getElementById("output");
    
    output.classList.remove("error", "success");

    if (data.error){
        output.classList.add("error");
        output.textContent = data.error;
        return;
    }
    if (data.status) {
        output.textContent = data.status;
        return;
    }
    if (data.data){
        output.textContent = data.data;
        return;
    }

    // fallback
    output.textContent = JSON.stringify(data, null, 2);
}


export function displayFileButtons(files, onClick) {
    const container = document.getElementById("fileList");
    container.innerHTML = "";

    if (!Array.isArray(files) || files.length === 0) {
        container.textContent = "Aucun fichier disponible";
        return;
    }
    const ul = document.createElement("ul");
    files.forEach(file => {
        const li = document.createElement("li");
        const filename = document.createElement("span");
        filename.innerText = file;

        const btn = document.createElement("button");
        btn.textContent = "Lire";
        btn.style.display = "block";
        btn.style.margin = "5px 0";
        btn.style.cursor = "pointer";
        
        btn.addEventListener("click", function () {
            withLoading(btn, () => onClick(file));
        });
        btn.classList.add("btn", "btn-file");

        li.appendChild(filename);
        li.appendChild(btn);
        ul.appendChild(li);
        
    });
    container.append(ul);
}


export function highlightSelection(listContainer, selectedElement) {
    listContainer.querySelectorAll("li").forEach(el => {
        el.classList.remove("selected");
    });
    selectedElement.classList.add("selected");
}


export async function withLoading(button, asyncAction) {
    const originalContent = button.innerHTML;

    // Retrieve the dimensions
    const width = button.offsetWidth;
    const height = button.offsetHeight;

    button.style.width = width + "px";
    button.style.height = height + "px";

    button.disabled = true;
    button.classList.add("loading");

    // Center loader
    button.innerHTML = `<span class="loader"></span>`;

    try {
        await asyncAction();
    } finally {
        button.disabled = false;
        button.classList.remove("loading");
        button.innerHTML = originalContent;

        // free dimensions
        button.style.width = "";
        button.style.height = "";
    }
}


export function downloadData(data, filename = "mesures.txt") {
    if (!data) {
        alert("Aucune donnée à sauvegarder");
        return;
    }

    const blob = new Blob([data], { type: "text/plain" });
    const url = URL.createObjectURL(blob);

    const a = document.createElement("a");
    a.href = url;
    a.download = filename;

    document.body.appendChild(a);
    a.click();

    document.body.removeChild(a);
    URL.revokeObjectURL(url);
}


export function displayTable(parsed) {
    const output = document.getElementById("output");
    output.innerHTML = "";
    const { headers, data } = parsed
    const table = document.createElement("table");
    table.setAttribute("id", "dataTable");
    const thead = document.createElement("thead");
    const trHead  = document.createElement("tr")

    headers.forEach(e => {
        const th = document.createElement("th");
        th.append(e);
        trHead.appendChild(th);
    });
    thead.appendChild(trHead);
    table.appendChild(thead);

    const tbody = document.createElement("tbody");
    
    data.forEach((row, index) => {
        const tr = document.createElement("tr");
        tr.setAttribute("data-index", index);
        const td1 = document.createElement("td");
        const td2 = document.createElement("td");
        const td3 = document.createElement("td");
        const td4 = document.createElement("td");

        td1.append(row.formattedDate)
        td2.append(row.time)
        td3.append(row.humidity)
        td4.append(row.temperature)

        tr.appendChild(td1);
        tr.appendChild(td2);
        tr.appendChild(td3);
        tr.appendChild(td4);
        tbody.appendChild(tr);
    });
    table.appendChild(tbody);
    output.append(table);
}


export function addDownloadButton(data, file) {
    const btn = document.createElement("button");
    btn.textContent = "Télécharger";
    btn.onclick = () => downloadData(data, file);
    btn.classList.add("btn", "floating-btn");
    btn.setAttribute("id", "floatingBtn");
    document.getElementById("output").appendChild(btn);
}


export function displayStats(parsed) {
    const { header, data, stats } = parsed;
    const statistics = document.getElementById("results");
    
    const myDiv = document.createElement("div");
    myDiv.setAttribute("id", "stats");
    const table = document.createElement("table");
    table.classList.add("fit");
    table.setAttribute("id", "dataTable");
    const tr1 = document.createElement("tr");
    const th1 = document.createElement("th");
    const td1 = document.createElement("td");
    th1.append("Nombre de mesures: ");
    td1.append(stats.nbMeasures);
    tr1.appendChild(th1);
    tr1.appendChild(td1);
    table.appendChild(tr1);

    const tr2 = document.createElement("tr");
    const th2 = document.createElement("th");
    const td2 = document.createElement("td");
    th2.append("Taux moyen d'humidité: ");
    td2.append(`${stats.avgHumidity.toFixed(2)} %`);
    tr2.appendChild(th2);
    tr2.appendChild(td2);
    table.appendChild(tr2);

    const tr3 = document.createElement("tr");
    const th3 = document.createElement("th");
    const td3 = document.createElement("td");
    th3.append("Température moyenne");
    td3.append(`${stats.avgTemperature.toFixed(2)} °C`);
    tr3.appendChild(th3);
    tr3.appendChild(td3);
    table.appendChild(tr3);

    myDiv.appendChild(table);
    statistics.appendChild(myDiv);

}

export function clearStats(){
    const stats = document.getElementById("stats");
    if (stats) {
        stats.remove();
    }
}

export function resetSelectedFile(){
    const title = document.getElementById("selected-file");
    title.innerText = "";
}

export function displaySelectedFile (filename){
    const title = document.getElementById("selected-file");
    title.innerText = filename;
}