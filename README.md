# 🚗 Robot cu Control Bluetooth - Arduino

Acest proiect Arduino permite controlul unui robot cu două motoare și turelă servo, folosind comenzi trimise prin Bluetooth. Este ideal pentru experimente educaționale, prototipuri și aplicații de robotică simplă.

## 🔧 Componente necesare

- Arduino Uno / Nano
- DRV8833 - Driver motoare
- 2 x motoare DC
- 2 x servomotoare (SG90 recomandat)
- Buzzer pasiv
- Modul Bluetooth (ex. HC-05)
- Fire jumper, breadboard / PCB
- Sursă alimentare (baterii sau alimentator)

## ⚙️ Conexiuni hardware

| Componentă        | Pin Arduino |
|-------------------|-------------|
| Motor IN1         | D2          |
| Motor IN2         | D3          |
| Motor IN3         | D4          |
| Motor IN4         | D5          |
| PWM Motor A       | D9 (OCR1A)  |
| PWM Motor B       | D10 (OCR1B) |
| Buzzer            | D6 (OC0A)   |
| Servo înclinare   | D12         |
| Servo rotație     | D13         |
| Bluetooth RX      | D7          |
| Bluetooth TX      | D8          |

## 📲 Comenzi Bluetooth

Trimite caractere individuale către robot (ex. din aplicația Serial Bluetooth Terminal):

| Comandă | Acțiune                   |
|---------|---------------------------|
| `W`     | Merge înainte             |
| `S`     | Merge înapoi              |
| `A`     | Viraj la stânga           |
| `D`     | Viraj la dreapta          |
| `Q`     | Scade viteza              |
| `E`     | Crește viteza             |
| `X`     | Frânează brusc            |
| `I`     | Ridică turelă (servo)     |
| `K`     | Coboară turelă (servo)    |
| `J`     | Rotește turelă stânga     |
| `L`     | Rotește turelă dreapta    |

🔁 Mișcare diagonală: Combină `W`/`S` cu `A`/`D` pentru diagonale inteligente.

## 🧠 Funcții principale în cod

- `setupPWM()` – Inițializează PWM pentru controlul motoarelor
- `handleCommand()` – Gestionează comenzile primite prin Bluetooth sau USB
- `brakeMotors()` – Activează frâna motorului și semnal sonor
- `stopMotors()` – Oprește complet motoarele
- Timeout automat: dacă nu se mai primesc comenzi după 200 ms, motoarele/servo se opresc

## 🛠️ Cum se folosește

1. Conectează componentele conform tabelului.
2. Încarcă codul pe Arduino din fișierul `.ino`.
3. Deschide aplicația Bluetooth pe telefon și conectează-te la modulul HC-05.
4. Trimite comenzile din secțiunea de mai sus.
5. Bucură-te de controlul complet al robotului!

## 📝 Licență

Acest proiect este licențiat sub [MIT License](LICENSE).

---

Realizat cu ❤️ de [numele tău aici, opțional].
