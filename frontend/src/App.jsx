
import { useEffect, useState, useCallback } from "react";

import {
  Activity,
  Thermometer,
  Droplets,
  Zap,
  Gauge,
  Users,
  ShieldCheck,
  ShieldAlert,
  Volume2,
  Wifi,
  WifiOff,
  RefreshCw,
  Cpu,
  Clock,
  Flame,
  Radio,
} from "lucide-react";

import {
  ResponsiveContainer,
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
} from "recharts";

import {
  getSensorHistory,
  getBackendHealth,
} from "./services/api";

import "./index.css";
import "./smart-lab-animations.css";


// ==========================================
// FORMAT HELPERS
// ==========================================

function formatValue(value, decimals = 1) {
  if (value === null || value === undefined || value === "") {
    return "--";
  }

  const number = Number(value);

  return Number.isFinite(number)
    ? number.toFixed(decimals)
    : "--";
}


function formatTime(timestamp) {
  if (!timestamp) return "--";

  const date = new Date(timestamp);

  if (Number.isNaN(date.getTime())) {
    return String(timestamp);
  }

  return date.toLocaleTimeString([], {
    hour: "2-digit",
    minute: "2-digit",
    second: "2-digit",
  });
}


function isActive(value) {
  return Number(value) === 1;
}


function isMissing(value) {
  return value === null || value === undefined || value === "";
}


// ==========================================
// REUSABLE SENSOR CARD
// ==========================================

function SensorCard({
  title,
  value,
  unit,
  icon: Icon,
  color,
  subtitle,
  className = "",
}) {
  return (
    <div className={`sensor-card ${className}`.trim()}>

      <div className="card-heading">
        <span>{title}</span>

        <span className={`icon-box ${color}`}>
          <Icon size={19} />
        </span>
      </div>

      <div className="sensor-reading">
        {value}
        <span>{unit}</span>
      </div>

      <div className="card-subtitle">
        {subtitle}
      </div>

    </div>
  );
}


// ==========================================
// ANIMATED CIRCUIT VISUAL
// ==========================================

function CircuitVisual({
  personDetected,
  personStatusUnknown,
}) {
  const radarClass = personStatusUnknown
    ? "radar-unknown"
    : personDetected
      ? "radar-active"
      : "radar-idle";

  return (
    <div
      className="circuit-visual"
      aria-label="Electronic circuit visualization"
    >

      <div className="circuit-grid" />

      <svg
        className="circuit-svg"
        viewBox="0 0 520 230"
        role="img"
        aria-label="Animated circuit traces"
      >

        <defs>
          <linearGradient
            id="circuitLine"
            x1="0"
            y1="0"
            x2="1"
            y2="1"
          >
            <stop offset="0%" stopColor="#22d3ee" />
            <stop offset="100%" stopColor="#14b8a6" />
          </linearGradient>

          <filter id="circuitGlow">
            <feGaussianBlur
              stdDeviation="3"
              result="blur"
            />
            <feMerge>
              <feMergeNode in="blur" />
              <feMergeNode in="SourceGraphic" />
            </feMerge>
          </filter>
        </defs>

        <g className="circuit-traces" fill="none">
          <path d="M20 45 H130 V90 H210 V115 H260" />
          <path d="M20 180 H100 V145 H210 V115" />
          <path d="M260 115 H330 V55 H410 V25 H500" />
          <path d="M260 115 H345 V175 H430 V205 H500" />
          <path d="M130 90 V25 H205" />
          <path d="M100 145 V215 H205" />
          <path d="M345 175 V220 H390" />
        </g>

        <g className="circuit-nodes">
          <circle cx="130" cy="90" r="5" />
          <circle cx="100" cy="145" r="5" />
          <circle cx="210" cy="115" r="6" />
          <circle cx="345" cy="175" r="5" />
          <circle cx="410" cy="55" r="5" />
          <circle cx="430" cy="205" r="5" />
        </g>

        <g
          className="circuit-core"
          filter="url(#circuitGlow)"
        >
          <circle cx="260" cy="115" r="49" />
          <circle cx="260" cy="115" r="35" />
          <circle cx="260" cy="115" r="22" />
        </g>

        <g className="core-symbol">
          <path d="M248 115 H272 M260 103 V127" />
          <circle cx="260" cy="115" r="7" />
        </g>

      </svg>

      <div className="visual-label">
        <span className="visual-live-dot" />
        ELECTRONIC SYSTEM CORE
      </div>

      <div className="visual-caption">
        <span>SMART LAB</span>
        <span>IoT MONITORING</span>
      </div>

      <div
        className={`radar-visual ${radarClass}`}
        aria-label={
          personStatusUnknown
            ? "Presence status unavailable"
            : personDetected
              ? "Presence detected"
              : "No presence detected"
        }
      >

        <div className="radar-ring ring-one" />
        <div className="radar-ring ring-two" />
        <div className="radar-ring ring-three" />
        <div className="radar-sweep" />
        <div className="radar-center" />

        {personDetected && (
          <div className="radar-target" />
        )}

      </div>

    </div>
  );
}


// ==========================================
// STATUS ROW
// ==========================================

function StatusRow({
  title,
  active,
  activeText,
  inactiveText,
  unknown = false,
}) {
  return (
    <div className="status-row">

      <span>{title}</span>

      {unknown ? (
        <span className="status-pill neutral">
          Unknown
        </span>
      ) : (
        <span
          className={`status-pill ${
            active ? "danger" : "normal"
          }`}
        >
          <span className="status-dot" />
          {active ? activeText : inactiveText}
        </span>
      )}

    </div>
  );
}


// ==========================================
// MAIN DASHBOARD
// ==========================================

export default function App() {

  const [latest, setLatest] = useState(null);

  const [history, setHistory] = useState([]);

  const [backendOnline, setBackendOnline] = useState(false);

  const [loading, setLoading] = useState(true);

  const [refreshing, setRefreshing] = useState(false);

  const [error, setError] = useState("");

  const [lastUpdated, setLastUpdated] = useState(null);


  // ========================================
  // FETCH CLOUD DATA
  // ========================================

  const loadDashboard = useCallback(async (showLoading = false) => {

    if (showLoading) {
      setRefreshing(true);
    }

    try {

      const [records, health] = await Promise.all([
        getSensorHistory(),
        getBackendHealth(),
      ]);
  
      // DEBUGGING LOGS
      console.log("Sensor records:", records);
      console.log("Records is array:", Array.isArray(records));
      console.log("Health response:", health);
      console.log("Latest record:", records[0]);

      if (!Array.isArray(records)) {
        throw new Error("Invalid sensor history response");
      }

      setBackendOnline(true);
      console.log("Backend health:", health);

      // API returns newest record first.
      const newest = records[0] || null;

      setLatest(newest);

      // Prepare chronological chart data.
      const chartData = records
        .slice(0, 30)
        .reverse()
        .map((record) => ({
          time: formatTime(record.timestamp),

          temperature: isMissing(record.temperature)
            ? null
            : Number(record.temperature),

          humidity: isMissing(record.humidity)
            ? null
            : Number(record.humidity),

          power: isMissing(record.power)
            ? null
            : Number(record.power),

          mq2_adc: isMissing(record.mq2_adc)
            ? null
            : Number(record.mq2_adc),
        }));

      setHistory(chartData);

      setLastUpdated(new Date());

      setError("");

    } catch (err) {
  console.error("Dashboard API error:", err);

  setError(
    err.message || "Unable to load dashboard data"
  );
} finally {

      setLoading(false);

      setRefreshing(false);
    }

  }, []);


  // ========================================
  // AUTOMATIC DATA REFRESH
  // ========================================

  useEffect(() => {

    let mounted = true;

    const refresh = async () => {

      if (!mounted) return;

      await loadDashboard();
    };

    refresh();

    // Refresh sensor data every 5 seconds.
    const timer = setInterval(refresh, 5000);

    return () => {

      mounted = false;

      clearInterval(timer);
    };

  }, [loadDashboard]);


  // ========================================
  // SENSOR STATUS CALCULATIONS
  // ========================================

  const hasRecord = latest !== null;

  const temperatureWarning = isActive(
    latest?.temperature_warning
  );

  const buzzerOn = isActive(
    latest?.buzzer_on
  );

  const personDetected = isActive(
    latest?.mmwave_person_detected
  );

  const personStatusUnknown = isMissing(
    latest?.mmwave_person_detected
  );


  // ========================================
  // MQ-2 STATUS
  // ========================================

  const mq2Available =
    latest !== null &&
    (
      !isMissing(latest.mq2_adc) ||
      !isMissing(latest.mq2_voltage) ||
      !isMissing(latest.mq2_digital)
    );

  const mq2Adc = latest?.mq2_adc;

  const mq2Voltage = latest?.mq2_voltage;

  const mq2Digital = latest?.mq2_digital;

  const gasAlertUnknown = isMissing(
    latest?.gas_triggered
  );

  const gasTriggered = isActive(
    latest?.gas_triggered
  );

  const mq2DigitalUnknown = isMissing(
    mq2Digital
  );


  // ========================================
  // DASHBOARD UI
  // ========================================

  return (

    <div className="app-shell">

      {/* ================================== */}
      {/* SIDEBAR */}
      {/* ================================== */}

      <aside className="sidebar">

        <div className="brand">

          <div className="brand-icon">
            <Cpu size={25} />
          </div>

          <div>
            <h2>SMART LAB</h2>
            <span>MONITORING SYSTEM</span>
          </div>

        </div>

        <div className="sidebar-section">
          MAIN MENU
        </div>

        <nav>

          <a className="nav-item active" href="#overview">
            <Activity size={19} />
            Overview
          </a>

          <a className="nav-item" href="#environment">
            <Thermometer size={19} />
            Environment
          </a>

          <a className="nav-item" href="#gas">
            <Flame size={19} />
            Gas Monitoring
          </a>

          <a className="nav-item" href="#energy">
            <Zap size={19} />
            Energy
          </a>

          <a className="nav-item" href="#security">
            <ShieldCheck size={19} />
            Security
          </a>

        </nav>

        <div className="sidebar-footer">

          <div className="device-indicator">

            <span
              className={`status-dot ${
                backendOnline ? "green" : "red"
              }`}
            />

            <span>
              {backendOnline
                ? "Backend connected"
                : "Backend offline"}
            </span>

          </div>

          <p>Smart Lab Monitor v1.0</p>

        </div>

      </aside>


      {/* ================================== */}
      {/* MAIN CONTENT */}
      {/* ================================== */}

      <main className="main-content">

        {/* HEADER */}

        <header className="topbar">

          <div className="topbar-heading">

            <p className="eyebrow">
              IOT MONITORING PLATFORM
            </p>

            <h1>Dashboard Overview</h1>

            <p className="header-description">
              Monitor your laboratory environment, gas levels
              and electrical parameters.
            </p>

          </div>

          <CircuitVisual
            personDetected={personDetected}
            personStatusUnknown={personStatusUnknown}
          />

          <div
            className={`connection-badge ${
              backendOnline
                ? "connected"
                : "disconnected"
            }`}
          >

            {backendOnline
              ? <Wifi size={17} />
              : <WifiOff size={17} />}

            {backendOnline
              ? "SYSTEM ONLINE"
              : "BACKEND OFFLINE"}

          </div>

        </header>


        {/* ================================== */}
        {/* CONNECTION ERROR */}
        {/* ================================== */}

        {error && (

          <div className="error-banner">

            <WifiOff size={18} />

            {error}. Check your Render backend connection.

          </div>

        )}


        {/* ================================== */}
        {/* LOADING STATUS */}
        {/* ================================== */}

        {loading && (

          <div className="loading-message">

            <RefreshCw size={18} className="spin" />

            Connecting to cloud database...

          </div>

        )}


        {/* ================================== */}
        {/* MANUAL REFRESH */}
        {/* ================================== */}

        <div className="section-heading">

          <div>
            <h2>Live Sensor Dashboard</h2>
            <p>Cloud sensor data from Render and Supabase</p>
          </div>

          <button
            type="button"
            className="refresh-button"
            onClick={() => loadDashboard(true)}
            disabled={refreshing}
          >

            <RefreshCw
              size={16}
              className={refreshing ? "spin" : ""}
            />

            {refreshing ? "Refreshing..." : "Refresh"}

          </button>

        </div>


        {/* ================================== */}
        {/* ENVIRONMENTAL MONITORING */}
        {/* ================================== */}

        <section id="environment">

          <div className="section-heading">

            <div>
              <h2>Environmental Monitoring</h2>
              <p>Real-time readings from DHT11</p>
            </div>

            <span className="section-label">
              DHT11
            </span>

          </div>

          <div className="sensor-grid">

            <SensorCard
              title="Temperature"
              value={formatValue(latest?.temperature)}
              unit="°C"
              icon={Thermometer}
              color="cyan"
              subtitle="Ambient temperature"
            />

            <SensorCard
              title="Humidity"
              value={formatValue(latest?.humidity)}
              unit="%"
              icon={Droplets}
              color="blue"
              subtitle="Relative humidity"
            />

          </div>

        </section>


        {/* ================================== */}
        {/* MQ-2 GAS MONITORING */}
        {/* ================================== */}

        <section id="gas">

          <div className="section-heading">

            <div>
              <h2>Gas & Smoke Monitoring</h2>

              <p>
                MQ-2 analog, digital and gas alert monitoring
              </p>
            </div>

            <span className="section-label">
              MQ-2
            </span>

          </div>

          <div className="sensor-grid">

            <SensorCard
              title="Gas Sensor ADC"
              value={formatValue(mq2Adc, 0)}
              unit=""
              icon={Activity}
              color="orange"
              subtitle={
                mq2Available
                  ? "Raw analog reading"
                  : "Waiting for MQ-2 sensor"
              }
            />

            <SensorCard
              title="Sensor Voltage"
              value={formatValue(mq2Voltage, 2)}
              unit="V"
              icon={Zap}
              color="yellow"
              subtitle={
                mq2Available
                  ? "Analog output voltage"
                  : "No voltage reading available"
              }
            />

            <SensorCard
              title="Digital Output"
              value={
                mq2DigitalUnknown
                  ? "--"
                  : Number(mq2Digital) === 1
                    ? "HIGH"
                    : "LOW"
              }
              unit=""
              icon={Radio}
              color="purple"
              subtitle="MQ-2 comparator output"
            />

          </div>


          {/* GAS ALERT STATUS */}

          <div className="security-card gas-status-card">

            <div
              className={`security-icon ${
                gasAlertUnknown
                  ? "orange"
                  : gasTriggered
                    ? "red-icon"
                    : "green"
              }`}
            >
              <Flame size={23} />
            </div>

            <div>

              <p>Gas Detection Status</p>

              <h3>
                {gasAlertUnknown
                  ? "WAITING FOR SENSOR"
                  : gasTriggered
                    ? "GAS ALERT"
                    : "NORMAL"}
              </h3>

              <span>
                {gasAlertUnknown
                  ? "MQ-2 alert data has not been received"
                  : gasTriggered
                    ? "Gas alert condition reported by firmware"
                    : "No gas alert reported by firmware"}
              </span>

            </div>

            <span
              className={`status-pill ${
                gasAlertUnknown
                  ? "neutral"
                  : gasTriggered
                    ? "danger"
                    : "normal"
              }`}
            >

              {gasAlertUnknown
                ? "NO DATA"
                : gasTriggered
                  ? "ALERT"
                  : "SAFE"}

            </span>

          </div>

          {!mq2Available && (

            <div className="loading-message">

              <Flame size={18} />

              MQ-2 hardware integration is pending.
              Connect the sensor to receive actual readings.

            </div>

          )}

        </section>


        {/* ================================== */}
        {/* ELECTRICAL MONITORING */}
        {/* ================================== */}

        <section id="energy">

          <div className="section-heading">

            <div>
              <h2>Electrical Monitoring</h2>

              <p>
                Real-time electrical measurements from PZEM-004T
              </p>
            </div>

            <span className="section-label">
              PZEM-004T
            </span>

          </div>

          <div className="sensor-grid four-columns">

            <SensorCard
              title="Voltage"
              value={formatValue(latest?.voltage)}
              unit="V"
              icon={Zap}
              color="yellow"
              subtitle="Supply voltage"
            />

            <SensorCard
              title="Current"
              value={formatValue(latest?.current, 3)}
              unit="A"
              icon={Activity}
              color="purple"
              subtitle="Load current"
            />

            <SensorCard
              title="Power"
              value={formatValue(latest?.power)}
              unit="W"
              icon={Gauge}
              color="green"
              subtitle="Active power"
              className="power-card"
            />

            <SensorCard
              title="Energy"
              value={formatValue(latest?.energy, 3)}
              unit="kWh"
              icon={Zap}
              color="orange"
              subtitle="Accumulated energy"
            />

          </div>

          <div className="sensor-grid">

            <SensorCard
              title="Frequency"
              value={formatValue(latest?.frequency, 2)}
              unit="Hz"
              icon={Activity}
              color="cyan"
              subtitle="Supply frequency"
            />

            <SensorCard
              title="Power Factor"
              value={formatValue(latest?.power_factor, 2)}
              unit=""
              icon={Gauge}
              color="blue"
              subtitle="Electrical power factor"
            />

          </div>

        </section>


        {/* ================================== */}
        {/* SECURITY & ALARM MONITORING */}
        {/* ================================== */}

        <section id="security">

          <div className="section-heading">

            <div>
              <h2>Security & Alarm Monitoring</h2>
              <p>mmWave presence and alarm states</p>
            </div>

            <span className="section-label">
              SAFETY
            </span>

          </div>

          <div className="security-grid">

            {/* HUMAN PRESENCE */}

            <div className="security-card">

              <div className="security-icon cyan">
                <Users size={23} />
              </div>

              <div>

                <p>Human Presence</p>

                <h3>
                  {personStatusUnknown
                    ? "Unknown"
                    : personDetected
                      ? "Detected"
                      : "Not detected"}
                </h3>

                <span>mmWave sensor</span>

              </div>

              <span
                className={`status-pill ${
                  personStatusUnknown
                    ? "neutral"
                    : personDetected
                      ? "normal"
                      : "danger"
                }`}
              >

                {personStatusUnknown
                  ? "NO DATA"
                  : personDetected
                    ? "PRESENT"
                    : "ABSENT"}

              </span>

            </div>


            {/* TEMPERATURE WARNING */}

            <div className="security-card">

              <div
                className={`security-icon ${
                  temperatureWarning
                    ? "red-icon"
                    : "green"
                }`}
              >

                {temperatureWarning
                  ? <ShieldAlert size={23} />
                  : <ShieldCheck size={23} />}

              </div>

              <div>

                <p>Temperature Warning</p>

                <h3>
                  {temperatureWarning
                    ? "ALERT"
                    : "NORMAL"}
                </h3>

                <span>Threshold: 45°C</span>

              </div>

              <span
                className={`status-pill ${
                  temperatureWarning
                    ? "danger"
                    : "normal"
                }`}
              >

                {temperatureWarning
                  ? "WARNING"
                  : "SAFE"}

              </span>

            </div>


            {/* BUZZER */}

            <div className="security-card">

              <div
                className={`security-icon ${
                  buzzerOn
                    ? "red-icon"
                    : "green"
                }`}
              >
                <Volume2 size={23} />
              </div>

              <div>

                <p>Buzzer Status</p>

                <h3>
                  {buzzerOn ? "ACTIVE" : "OFF"}
                </h3>

                <span>Alarm output</span>

              </div>

              <span
                className={`status-pill ${
                  buzzerOn
                    ? "danger"
                    : "normal"
                }`}
              >

                {buzzerOn ? "ON" : "OFF"}

              </span>

            </div>

          </div>

        </section>


        {/* ================================== */}
        {/* SENSOR HISTORY */}
        {/* ================================== */}

        <section>

          <div className="section-heading">

            <div>
              <h2>Sensor History</h2>
              <p>Last 30 available database records</p>
            </div>

            <span className="section-label">
              HISTORY
            </span>

          </div>

          <div className="charts-grid">

            {/* TEMPERATURE CHART */}

            <div className="chart-card">

              <div className="chart-heading">

                <div>
                  <h3>Temperature Trend</h3>
                  <p>Historical temperature readings</p>
                </div>

                <Thermometer size={20} />

              </div>

              <ResponsiveContainer width="100%" height={250}>

                <LineChart data={history}>

                  <CartesianGrid
                    stroke="#263548"
                    strokeDasharray="4 4"
                  />

                  <XAxis
                    dataKey="time"
                    stroke="#8393a8"
                    tick={{ fontSize: 11 }}
                  />

                  <YAxis
                    stroke="#8393a8"
                    tick={{ fontSize: 11 }}
                    domain={["auto", "auto"]}
                  />

                  <Tooltip
                    contentStyle={{
                      background: "#111c2b",
                      border: "1px solid #334155",
                      borderRadius: "10px",
                    }}
                  />

                  <Line
                    type="monotone"
                    dataKey="temperature"
                    name="Temperature"
                    stroke="#38bdf8"
                    strokeWidth={2.5}
                    dot={false}
                    connectNulls={false}
                  />

                </LineChart>

              </ResponsiveContainer>

            </div>


            {/* POWER CHART */}

            <div className="chart-card">

              <div className="chart-heading">

                <div>
                  <h3>Power Trend</h3>
                  <p>Historical active power readings</p>
                </div>

                <Zap size={20} />

              </div>

              <ResponsiveContainer width="100%" height={250}>

                <LineChart data={history}>

                  <CartesianGrid
                    stroke="#263548"
                    strokeDasharray="4 4"
                  />

                  <XAxis
                    dataKey="time"
                    stroke="#8393a8"
                    tick={{ fontSize: 11 }}
                  />

                  <YAxis
                    stroke="#8393a8"
                    tick={{ fontSize: 11 }}
                  />

                  <Tooltip
                    contentStyle={{
                      background: "#111c2b",
                      border: "1px solid #334155",
                      borderRadius: "10px",
                    }}
                  />

                  <Line
                    type="monotone"
                    dataKey="power"
                    name="Power"
                    stroke="#34d399"
                    strokeWidth={2.5}
                    dot={false}
                    connectNulls={false}
                  />

                </LineChart>

              </ResponsiveContainer>

            </div>


            {/* MQ-2 GAS CHART */}

            <div className="chart-card">

              <div className="chart-heading">

                <div>
                  <h3>MQ-2 Gas Sensor Trend</h3>
                  <p>Historical raw ADC readings</p>
                </div>

                <Flame size={20} />

              </div>

              {history.some(
                (record) => record.mq2_adc !== null
              ) ? (

                <ResponsiveContainer width="100%" height={250}>

                  <LineChart data={history}>

                    <CartesianGrid
                      stroke="#263548"
                      strokeDasharray="4 4"
                    />

                    <XAxis
                      dataKey="time"
                      stroke="#8393a8"
                      tick={{ fontSize: 11 }}
                    />

                    <YAxis
                      stroke="#8393a8"
                      tick={{ fontSize: 11 }}
                    />

                    <Tooltip
                      contentStyle={{
                        background: "#111c2b",
                        border: "1px solid #334155",
                        borderRadius: "10px",
                      }}
                    />

                    <Line
                      type="monotone"
                      dataKey="mq2_adc"
                      name="MQ-2 ADC"
                      stroke="#f59e0b"
                      strokeWidth={2.5}
                      dot={false}
                      connectNulls={false}
                    />

                  </LineChart>

                </ResponsiveContainer>

              ) : (

                <div className="loading-message">
                  <Flame size={18} />
                  Waiting for actual MQ-2 readings.
                  The chart will appear when sensor data is received.
                </div>

              )}

            </div>

          </div>

        </section>


        {/* ================================== */}
        {/* FOOTER */}
        {/* ================================== */}

        <footer className="dashboard-footer">

          <div>
            <Cpu size={16} />
            Smart Lab Monitor
          </div>

          <div>
            <Clock size={15} />

            Last updated:{" "}

            {lastUpdated
              ? lastUpdated.toLocaleTimeString()
              : "--"}
          </div>

          <div>

            <span
              className={`status-dot ${
                backendOnline
                  ? "green"
                  : "red"
              }`}
            />

            {hasRecord
              ? `Record #${latest.id}`
              : "No sensor records"}

          </div>

        </footer>

      </main>

    </div>
  );
}
