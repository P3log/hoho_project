// Possible fields : error, data, status
export function formatResponse(response) {
    if (!response) return "Empty response";

    if (response.error) {
        return response.error;
    }
    if (response.data) {
        return response.data;
    }
    if (response.status) {
        return response.status;
    }

    return {
        type: "unknown",
        content: JSON.stringify(response, null, 2)
    };
}
