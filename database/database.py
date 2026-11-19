
import sqlite3
import os

DB_NAME = os.path.join(
    os.path.dirname(os.path.abspath(__file__)),
    "sensor_data.db"
)


def create_database():
    # Connect to SQLite database
    with sqlite3.connect(DB_NAME) as conn:
        cursor = conn.cursor()

        # Create the original table
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS sensor_data (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                temperature REAL NOT NULL,
                humidity REAL NOT NULL,
                timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        """)

        # Additional Smart Lab Monitor fields
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

        # Get existing columns
        cursor.execute("PRAGMA table_info(sensor_data)")
        existing_columns = {
            column[1] for column in cursor.fetchall()
        }

        # Add only missing columns
        for column_name, column_type in new_columns.items():
            if column_name not in existing_columns:
                cursor.execute(
                    f"ALTER TABLE sensor_data ADD COLUMN "
                    f"{column_name} {column_type}"
                )

    print("Smart Lab Monitor database ready!")


if __name__ == "__main__":
    create_database()
