#define BLYNK_TEMPLATE_ID "TMPL2RXHEPTJJ"
#define BLYNK_TEMPLATE_NAME "Pele Academia"
#define BLYNK_AUTH_TOKEN "SEU_AUTH_TOKEN"
// Token real somente pelo Serial privado; mantido apenas em RAM.
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>
#include <atomic>

constexpr int DHT_PIN = 15, BUZZER_PIN = 19;
constexpr float LIMITE_TEMPERATURA = 30.0;
constexpr uint32_t INTERVALO_LEITURA = 2000;
DHTesp dht;
Adafruit_SSD1306 display(128, 64, &Wire, -1);
bool oledOk = false;
std::atomic<bool> permitirNuvem{true}, nuvemOnline{false};
struct Amostra { float temperatura, umidade; bool valida, alerta; };
QueueHandle_t fila;
QueueHandle_t filaCredencial;
struct Credencial { char token[65]; };

// Esta tarefa e a unica que acessa WiFi/Blynk. Nenhuma decisao depende dela.
void tarefaNuvem(void*) {
  Credencial credencial{};
  // O token recebido pelo Serial vive apenas na RAM, nunca no projeto publico.
  Serial.println("Blynk aguardando comando AUTH <token> privado. Operacao local ativa.");
  while (xQueueReceive(filaCredencial, &credencial, pdMS_TO_TICKS(100)) != pdTRUE) {}
  Blynk.config(credencial.token);
  WiFi.mode(WIFI_STA);
  WiFi.begin("Wokwi-GUEST", "", 6);
  uint32_t tentativa = millis() - 10000, envio = 0;
  bool suspenso = false, estavaOnline = false;
  for (;;) {
    if (!permitirNuvem.load()) {
      Blynk.disconnect();
      if (!suspenso) WiFi.disconnect();
      suspenso = true;
      nuvemOnline.store(false);
      estavaOnline = false;
    } else {
      if (suspenso) {
        WiFi.begin("Wokwi-GUEST", "", 6);
        suspenso = false;
        tentativa = millis() - 10000;
      }
      if (millis() - tentativa >= 10000) {
        tentativa = millis();
        if (WiFi.status() != WL_CONNECTED) WiFi.reconnect();
        else if (!Blynk.connected()) Blynk.connect(1000);
      }
      if (Blynk.connected()) Blynk.run();
      bool online = Blynk.connected() && WiFi.status() == WL_CONNECTED;
      nuvemOnline.store(online);
      if (online && (!estavaOnline || millis() - envio >= 2000)) {
        Amostra a;
        if (xQueuePeek(fila, &a, 0) == pdTRUE) {
          if (a.valida) {
            Blynk.virtualWrite(V0, a.temperatura);
            Blynk.virtualWrite(V2, a.umidade);
          }
          Blynk.virtualWrite(V1, a.alerta ? 1 : 0);
          Blynk.virtualWrite(V3, !a.valida ? "ERRO SENSOR" : a.alerta ? "ALERTA" : "NORMAL");
          Blynk.virtualWrite(V4, a.valida ? 1 : 0);
          envio = millis();
        }
      }
      estavaOnline = online;
    }
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

void lerSensor() {
  TempAndHumidity leitura = dht.getTempAndHumidity();
  Serial.println(dht.getStatusString());
  Amostra a{leitura.temperature, leitura.humidity, false, false};
  a.valida = !isnan(a.temperatura) && !isnan(a.umidade);
  // Falha de sensor sinaliza erro e ativa buzzer por precaucao.
  a.alerta = !a.valida || a.temperatura > LIMITE_TEMPERATURA;
  if (a.alerta) tone(BUZZER_PIN, 2000);
  else noTone(BUZZER_PIN);
  const char* estado = !a.valida ? "ERRO SENSOR" : a.alerta ? "ALERTA" : "NORMAL";
  if (oledOk) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("PELE ACADEMIA");
    display.println("Temperatura:");
    display.setTextSize(2);
    if (a.valida) { display.print(a.temperatura, 1); display.println(" C"); }
    else display.println("-- C");
    display.setTextSize(1);
    display.print("STATUS: "); display.println(estado);
    display.println("Limite: > 30.0 C");
    display.print("Blynk: ");
    display.println(nuvemOnline.load() ? "ONLINE" : "OFFLINE");
    display.display();
  }
  Serial.printf("t=%lu ms | T=%.1f C | %s | buzzer=%s | Blynk=%s\n",
                millis(), a.temperatura, estado, a.alerta ? "ON" : "OFF",
                nuvemOnline.load() ? "ONLINE" : "OFFLINE");
  if (fila) xQueueOverwrite(fila, &a);
}

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  dht.setup(DHT_PIN, DHTesp::DHT22);
  Wire.begin(21, 22);
  oledOk = display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  if (oledOk) {
    display.clearDisplay(); display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1); display.setCursor(0, 0);
    display.println("PELE ACADEMIA"); display.println("Aguardando sensor...");
    display.display();
  } else Serial.println("ERRO OLED: leitura e buzzer continuam ativos.");
  fila = xQueueCreate(1, sizeof(Amostra));
  filaCredencial = xQueueCreate(1, sizeof(Credencial));
  if (!fila || !filaCredencial || xTaskCreatePinnedToCore(tarefaNuvem, "nuvem", 8192, nullptr, 1, nullptr, 0) != pdPASS)
    Serial.println("Nuvem indisponivel; processamento local ativo.");
  Serial.println("Comandos: AUTH <token>, 0 = suspender nuvem; 1 = reconectar nuvem.");
}

// Leitura de comandos sem bloqueio e sem imprimir a credencial recebida.
void processarComando(const char* comando) {
  if (strcmp(comando, "0") == 0 || strcmp(comando, "1") == 0) {
    permitirNuvem.store(comando[0] == '1');
    Serial.println(comando[0] == '0' ? "Nuvem suspensa pelo teste local." : "Reconexao da nuvem autorizada.");
  } else if (strncmp(comando, "AUTH ", 5) == 0) {
    size_t n = strlen(comando + 5);
    if (n >= 25 && n <= 64 && filaCredencial) {
      Credencial c{};
      memcpy(c.token, comando + 5, n);
      xQueueOverwrite(filaCredencial, &c);
      memset(&c, 0, sizeof(c));
      Serial.println("Credencial recebida em RAM. Nao sera exibida nem gravada.");
    } else Serial.println("Credencial invalida. Confira o token em ambiente privado.");
  }
}

void loop() {
  static uint32_t ultimaLeitura = 0;
  static char comando[96]{};
  static size_t pos = 0;
  static bool excesso = false;
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      if (!excesso) { comando[pos] = 0; processarComando(comando); }
      memset(comando, 0, sizeof(comando)); pos = 0; excesso = false;
    } else if (c != '\r') {
      if (pos < sizeof(comando) - 1) comando[pos++] = c;
      else excesso = true;
    }
  }
  if (millis() - ultimaLeitura >= INTERVALO_LEITURA) {
    ultimaLeitura = millis(); lerSensor();
  }
  delay(5);
}
