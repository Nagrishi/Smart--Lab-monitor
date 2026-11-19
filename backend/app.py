
from flask import Flask, request, jsonify
import sqlite3
import os

app = Flask(__name__)

# ==========================================
# DATABASE CONFIGURATION
# ==========================================

BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DB_PATH = os.path.join(BASE_DIR, "database", "sensor_data.db")


def get_db_connection():
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    return conn


# ==========================================
# DATABASE INITIALIZATION
# ==========================================

def initialize_database():

    os.makedirs(os.path.dirname(DB_PATH), exist_ok=True)

    conn = get_db_connection()

    try:
        cursor = conn.cursor()

        cursor.execute("""
            CREATE TABLE IF NOT EXISTS sensor_data (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                temperature REAL NOT NULL,
                humidity REAL NOT NULL,
                timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        """)

        new_columns = {
            "voltage": "REAL",
            "current": "REAL",
            "power": "REAL",
            "energy": "REAL",
            "frequency": "REAL",
            "power_factor": "REAL",
            "mq2_adc": "INTEGER",
            "mq2_voltage": "REAL",
            "mq2_digital": "INTEGER",
            "mmwave_person_detected": "INTEGER",
            "gas_triggered": "INTEGER",
            "temperature_warning": "INTEGER",
            "buzzer_on": "INTEGER"
        }

        cursor.execute("PRAGMA table_info(sensor_data)")

        existing_columns = {
            row["name"] for row in cursor.fetchall()
        }

        for column, column_type in new_columns.items():

            if column not in existing_columns:

                cursor.execute(
                    f"ALTER TABLE sensor_data ADD COLUMN "
                    f"{column} {column_type}"
                )

        conn.commit()

    finally:
        conn.close()


# ==========================================
# HELPER FUNCTIONS
# ==========================================

def to_number(value):

    if value is None or isinstance(value, bool):
        return None

    try:
        return float(value)

    except (ValueError, TypeError):
        return None


def to_integer(value):

    if value is None:
        return None

    if isinstance(value, bool):
        return int(value)

    if isinstance(value, str):

        value = value.strip().lower()

        if value in ("true", "high", "yes", "on"):
            return 1

        if value in ("false", "low", "no", "off"):
            return 0

    try:
        number = float(value)

        if number in (0, 1):
            return int(number)

    except (ValueError, TypeError):
        pass

    return None


# ==========================================
# HOME
# ==========================================

@app.route("/")
def home():

    return "Smart Lab Monitor Backend is running!"


# ==========================================
# HEALTH CHECK
# ==========================================

@app.route("/api/health", methods=["GET"])
def health():

    try:
        conn = get_db_connection()

        try:
            conn.execute("SELECT 1")

        finally:
            conn.close()

        return jsonify({
            "status": "online",
            "database": "connected"
        })

    except sqlite3.Error as error:

        return jsonify({
            "status": "error",
            "message": str(error)
        }), 500


# ==========================================
# RECEIVE SENSOR DATA
# ==========================================

@app.route("/api/sensor", methods=["POST"])
def receive_sensor_data():

    data = request.get_json(silent=True)

    if not isinstance(data, dict):

        return jsonify({
            "status": "error",
            "message": "A valid JSON object is required"
        }), 400

    # Support flat and nested JSON
    pzem = data.get("pzem") or {}
    dht11 = data.get("dht11") or {}
    mq2 = data.get("mq2") or {}
    mmwave = data.get("mmwave") or {}
    alerts = data.get("alerts") or {}

    sections = [pzem, dht11, mq2, mmwave, alerts]

    if not all(isinstance(section, dict) for section in sections):

        return jsonify({
            "status": "error",
            "message": "Sensor sections must be JSON objects"
        }), 400

    # --------------------------------------
    # DHT11
    # --------------------------------------

    temperature = to_number(
        dht11.get("temperature", data.get("temperature"))
    )

    humidity = to_number(
        dht11.get("humidity", data.get("humidity"))
    )

    if temperature is None or humidity is None:

        return jsonify({
            "status": "error",
            "message": "Temperature and humidity are required"
        }), 400

    # --------------------------------------
    # PZEM-004T
    # --------------------------------------

    voltage = to_number(
        pzem.get("voltage", data.get("voltage"))
    )

    current = to_number(
        pzem.get("current", data.get("current"))
    )

    power = to_number(
        pzem.get("power", data.get("power"))
    )

    energy = to_number(
        pzem.get("energy", data.get("energy"))
    )

    frequency = to_number(
        pzem.get("frequency", data.get("frequency"))
    )

    power_factor = to_number(
        pzem.get("power_factor", data.get("power_factor"))
    )

    # --------------------------------------
    # MQ-2 GAS SENSOR
    # --------------------------------------

    mq2_adc = to_integer(
        mq2.get("adc", data.get("mq2_adc"))
    )

    mq2_voltage = to_number(
        mq2.get("voltage", data.get("mq2_voltage"))
    )

    mq2_digital = to_integer(
        mq2.get("digital", data.get("mq2_digital"))
    )

    # --------------------------------------
    # MMWAVE PRESENCE SENSOR
    # --------------------------------------

    # Supports:
    # mmwave.person_detected
    # mmwave_person_detected
    # person_detected

    person_detected = mmwave.get(
        "person_detected",
        data.get(
            "mmwave_person_detected",
            data.get("person_detected")
        )
    )

    person_detected = to_integer(person_detected)

    # --------------------------------------
    # ALERT STATES
    # --------------------------------------

    gas_triggered = to_integer(
        alerts.get(
            "gas_triggered",
            data.get("gas_triggered")
        )
    )

    temperature_warning = to_integer(
        alerts.get(
            "temperature_warning",
            data.get("temperature_warning")
        )
    )

    buzzer_on = to_integer(
        alerts.get(
            "buzzer_on",
            data.get("buzzer_on")
        )
    )

    # ======================================
    # SAVE DATA TO SQLITE
    # ======================================

    conn = None

    try:

        conn = get_db_connection()
        cursor = conn.cursor()

        cursor.execute("""
            INSERT INTO sensor_data (
                temperature,
                humidity,
                voltage,
                current,
                power,
                energy,
                frequency,
                power_factor,
                mq2_adc,
                mq2_voltage,
                mq2_digital,
                mmwave_person_detected,
                gas_triggered,
                temperature_warning,
                buzzer_on
            )
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        """, (
            temperature,
            humidity,
            voltage,
            current,
            power,
            energy,
            frequency,
            power_factor,
            mq2_adc,
            mq2_voltage,
            mq2_digital,
            person_detected,
            gas_triggered,
            temperature_warning,
            buzzer_on
        ))

        record_id = cursor.lastrowid

        conn.commit()

        # ----------------------------------
        # TERMINAL OUTPUT
        # ----------------------------------

        print("\n========== SMART LAB MONITOR ==========")

        print(f"Temperature: {temperature} C")
        print(f"Humidity: {humidity} %")

        print(f"Voltage: {voltage} V")
        print(f"Current: {current} A")
        print(f"Power: {power} W")
        print(f"Energy: {energy} kWh")

        print(f"MQ2 ADC: {mq2_adc}")
        print(f"MQ2 Voltage: {mq2_voltage} V")
        print(f"MQ2 Digital: {mq2_digital}")

        print(f"Person detected: {person_detected}")

        print(f"Gas triggered: {gas_triggered}")
        print(f"Temperature warning: {temperature_warning}")
        print(f"Buzzer: {buzzer_on}")

        print(f"Saved record ID: {record_id}")

        print("=======================================")

        return jsonify({
            "status": "success",
            "message": "Sensor data saved",
            "record_id": record_id
        }), 201

    except sqlite3.Error as error:

        if conn:
            conn.rollback()

        return jsonify({
            "status": "error",
            "message": str(error)
        }), 500

    finally:

        if conn:
            conn.close()


# ==========================================
# GET SENSOR HISTORY
# ==========================================

@app.route("/api/sensor", methods=["GET"])
def get_sensor_data():

    conn = None

    try:

        conn = get_db_connection()

        rows = conn.execute("""
            SELECT *
            FROM sensor_data
            ORDER BY id DESC
            LIMIT 1000
        """).fetchall()

        return jsonify([dict(row) for row in rows])

    except sqlite3.Error as error:

        return jsonify({
            "status": "error",
            "message": str(error)
        }), 500

    finally:

        if conn:
            conn.close()


# ==========================================
# GET LATEST SENSOR READING
# ==========================================

@app.route("/api/sensor/latest", methods=["GET"])
def get_latest_sensor_data():

    conn = None

    try:

        conn = get_db_connection()

        row = conn.execute("""
            SELECT *
            FROM sensor_data
            ORDER BY id DESC
            LIMIT 1
        """).fetchone()

        if row is None:

            return jsonify({
                "status": "empty",
                "message": "No sensor readings available"
            }), 404

        return jsonify(dict(row))

    except sqlite3.Error as error:

        return jsonify({
            "status": "error",
            "message": str(error)
        }), 500

    finally:

        if conn:
            conn.close()


# ==========================================
# START SERVER
# ==========================================

if __name__ == "__main__":

    initialize_database()

    print("Smart Lab Monitor Backend Starting...")
    print(f"Database: {DB_PATH}")

    app.run(
        host="0.0.0.0",
        port=5000,
        debug=False
    )
