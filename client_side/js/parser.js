
export function parseCSV(content) {
    const lines = content.trim().split("\n");

    if (lines.length === 0) {
        return { headers: [], data: [] };
    }

    const headers = lines[0].split(";");

    // global variables
    let count = 0;
    let sumHumidity = 0;
    let sumTemperature = 0;

    const data = lines.slice(1)
        .filter(line => line.trim() !== "")
        .map(line => {
            const parts = line.split(";");
            if (parts.length < 4) return null;

            const [date, t, hdt, tmp] = parts;
            const [year, month, day] = date.split("-");

            if (!Number.isNaN(hdt) && !Number.isNaN(tmp)) {
                count++;
                sumHumidity += Number.parseFloat(hdt);
                sumTemperature += Number.parseFloat(tmp);
            }

            return {
                rawDate: date,
                formattedDate: `${day}/${month}/${year}`,
                time: t,
                timestamp: `${year}-${month}-${day}T${t}`,
                humidity: Number.parseFloat(hdt),
                temperature: Number.parseFloat(tmp),
            };
        })
        .filter(x => x !== null);

    const stats = count > 0 ? 
        {
            nbMeasures: count,
            avgHumidity: sumHumidity / count,
            avgTemperature: sumTemperature / count
        } : null;

    return { headers, data, stats };
}