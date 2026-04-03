let chartInstance = null;

export function displayChart(data) {
    const ctx = document.getElementById("chart").getContext("2d");

    // const labels = data.map(d => `${d.formattedDate} ${d.time}`);

    const humidityData = data.map(d => ({
        x:  d.timestamp,
        y: d.humidity
    }));
    const temperatureData = data.map(d => ({
        x: d.timestamp,
        y: d.temperature
    }));

    chartInstance = new Chart(ctx, {
        type: "line",
        data: {
            datasets: [
                {
                    label: "Température",
                    data: temperatureData,
                    yAxisID: "y1",
                    borderColor: "#ff6d8c"
                },
                {
                    label: "Humidité",
                    data: humidityData,
                    yAxisID: "y2",
                    borderColor: "#36a2eb"
                }
            ]
        },
        options: {
            responsive: true,
            interaction: {
                mode: "nearest",
                intersect: true
            },
            scales: {
                x: {
                    type: "time",
                    time: {
                        unit: "minute", // or "hour"
                        displayFormats: {
                            minute: "HH:mm",
                            hour: "HH:mm"
                        },
                    tooltipFormat: "HH:mm"
                    },
                    title: {
                        display: true,
                        text: "Temps"
                    }
                },
                y1: {
                    type: "linear",
                    position: "left",
                    title: {
                        display: true,
                        text: "Température (°C)"
                    }
                },
                y2: {
                    type: "linear",
                    position: "right",
                    title: {
                        display: true,
                        text: "Humidité (%)"
                    }
                }
            }
        }
    });
}


export function resizeChart() {
    if (chartInstance) {
        chartInstance.resize();
    }
}


export function clearChart() {
    if (chartInstance) {
        chartInstance.destroy();
        chartInstance = null;
    }
}