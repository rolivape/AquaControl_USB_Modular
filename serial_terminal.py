import serial
import threading

def read_from_port(ser):
    while True:
        try:
            data = ser.readline().decode(errors='ignore').strip()
            if data:
                print(data)
        except:
            break

# Cambia esto al puerto correcto de tu ESP32
PORT = '/dev/tty.wchusbserial59700477991' # Usa `ls /dev/tty.*` para encontrarlo
BAUDRATE = 115200

ser = serial.Serial(PORT, BAUDRATE, timeout=1)

# Hilo para lectura asíncrona
reader = threading.Thread(target=read_from_port, args=(ser,))
reader.daemon = True
reader.start()

print("Conectado. Escribe comandos (Ctrl+C para salir):")

try:
    while True:
        cmd = input() + '\n'
        ser.write(cmd.encode())
except KeyboardInterrupt:
    print("\nSaliendo...")
    ser.close()
