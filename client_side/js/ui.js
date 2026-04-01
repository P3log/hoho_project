import { formatResponse } from "./response.js";

export function displayResult(data) {
    const output = document.getElementById("output");
    output.textContent = formatResponse(data);
}


export function displayFileButtons(files, onClick) {
    const container = document.getElementById("fileList");
    container.innerHTML = "";

    if (!Array.isArray(files) || files.length === 0) {
        container.textContent = "Aucun fichier disponible";
        return;
    }

    files.forEach(file => {
        const btn = document.createElement("button");

        btn.textContent = file;
        btn.style.display = "block";
        btn.style.margin = "5px 0";
        btn.style.cursor = "pointer";

        btn.addEventListener("click", function () {
            withLoading(btn, () => onClick(file));
        });

        container.appendChild(btn);
    });
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
