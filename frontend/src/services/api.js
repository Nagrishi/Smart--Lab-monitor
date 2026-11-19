
const API_BASE_URL = "/api";

// Fetch sensor history from Flask
export async function getSensorHistory() {
  const response = await fetch(`${API_BASE_URL}/sensor`);

  if (!response.ok) {
    throw new Error(`API error: ${response.status}`);
  }

  return response.json();
}

// Fetch backend health
export async function getBackendHealth() {
  const response = await fetch(`${API_BASE_URL}/health`);

  if (!response.ok) {
    throw new Error("Backend is unavailable");
  }

  return response.json();
}
