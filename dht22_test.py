import time
import board
import adafruit_dht

dht = adafruit_dht.DHT22(board.D17, use_pulseio=False)

while True:
    try:
        print(f"Temp: {dht.temperature:.1f}°C  Humidity: {dht.humidity:.1f}%")
    except RuntimeError as e:
        print("Retrying:", e)

    time.sleep(3)
