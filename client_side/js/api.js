import { CLIENT_URL } from "../config/config.js";

export async function sendCommand(command, value = "") {
    const response = await fetch(CLIENT_URL, {
        method: "POST",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify({ command, value })
    });

    return response.json();
}