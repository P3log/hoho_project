import { getFile, startTracer, stopTracer, getLogs, loadFileList } from "./actions.js";
import { withLoading } from "./ui.js";

window.getList = loadFileList;
window.getFile = getFile;
window.startTracer = startTracer;
window.stopTracer = stopTracer;
window.getLogs = getLogs;


document.getElementById("btnList").addEventListener("click", function () {
    withLoading(this, loadFileList);
});