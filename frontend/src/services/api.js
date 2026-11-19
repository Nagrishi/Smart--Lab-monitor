
const API_BASE_URL =
  import.meta.env.VITE_API_BASE_URL ||
  "https://smart-lab-monitor-api.onrender.com";

export async function getSensorHistory() {
  const response = await fetch(`${API_BASE_URL}/api/sensor`);

  if (!response.ok) {
    throw new Error(`Sensor API error: ${response.status}`);
  }

  return response.json();
}

export async function getBackendHealth() {
  const response = await fetch(`${API_BASE_URL}/api/health`);

  if (!response.ok) {
    throw new Error(`Backend health error: ${response.status}`);
  }

  return response.json();
}
