# Sprint 4 de Edge Computing e Computer Systems

## Protótipo de monitoramento da Pelé Academia

O protótipo monitora a temperatura de um espaço de treinamento com ESP32 e DHT22. O próprio ESP32 compara a leitura com o limite didático de 30 °C, atualiza o display OLED e aciona o buzzer quando a temperatura ultrapassa esse limite. O Blynk apresenta as informações remotamente.

## Operações realizadas na borda

A cada 2 segundos, o ESP32 lê temperatura e umidade. Temperatura menor ou igual a 30 °C resulta no estado NORMAL e buzzer desligado; acima de 30 °C resulta em ALERTA e buzzer a 2000 Hz. O OLED mostra a temperatura, o estado, o limite e a situação da conexão. Uma falha de leitura produz ERRO SENSOR e sinalização local.

Essas operações são executadas no loop local. A comunicação utiliza uma tarefa separada e uma fila com a última amostra, de modo que espera por Wi-Fi, autenticação e reconexão não fazem parte do caminho da decisão. Não há comando do Blynk para controlar o buzzer.

## Operações que dependem da nuvem

A autenticação do dispositivo, a transmissão dos Virtual Pins e a exibição remota no painel dependem de conexão com Blynk. O envio inclui temperatura em V0, alarme em V1, umidade em V2, estado textual em V3 e validade do sensor em V4. Durante uma interrupção, o painel deixa de receber atualizações; as operações locais continuam. Na reconexão, o dispositivo envia o valor atual, sem recuperar histórico offline.

## Hardware simulado

| Item | Ligação |
|---|---|
| DHT22 DATA | GPIO 15 |
| OLED SDA e SCL | GPIO 21 e 22 |
| Buzzer positivo | GPIO 19 |
| Alimentação do DHT22 e OLED | 3V3 e GND |
| Buzzer negativo | GND |

O OLED SSD1306 é de 128 × 64 pixels e utiliza o endereço I2C 0x3C. O volume do buzzer no simulador foi reduzido a 0.01.

## Links da solução

- Wokwi: https://wokwi.com/projects/479357102690024449
- Template Blynk: https://blynk.cloud/dashboard/359336/templates/787381/dashboard
- Dispositivo Blynk: https://blynk.cloud/dashboard/359336/global/devices/126785/organization/359336/devices/1091498/dashboard
- Repositório GitHub: https://github.com/caiovfaria/Sprint4-Edge

O link Blynk exige conta e permissão de acesso à organização/dispositivo; não é um link público de visualização. A concessão de acesso ao professor deve ser verificada antes da entrega.

## Evidências e reprodução

As imagens e os resultados efetivamente observados estão em evidencias/ e TESTES.md. Para iniciar o projeto, abra o Wokwi e pressione Play. O sistema local funciona sem credencial. Para autenticar na sessão, envie AUTH seguido de um espaço e do token no Monitor Serial. A credencial permanece somente na RAM e deve ser informada novamente após reiniciar a simulação.

Envie 0 e Enter para suspender Wi-Fi e Blynk. Alterne a temperatura entre 25 e 35 °C e confira a resposta do OLED e do buzzer. Envie 1 e Enter para permitir reconexão e acompanhe o valor atual no Blynk. Não inclua o token em código, capturas, commits ou vídeos.

## Documentação de entrega

O enunciado da Sprint 4, páginas 13 e 14 do documento Challenge PeleAcademia Sprints3e4 RJ, solicita código público no GitHub, link Wokwi, link Blynk acessível ao professor, captura da IHM e do painel, e breve descrição de borda e nuvem. Todos esses itens devem estar no GitHub. A fotografia física é pertinente quando houver montagem física. A seção consultada não exige vídeo nem termo separado para esta disciplina.

Este material se refere à Sprint 4 da disciplina Edge Computing e Computer Systems. Não substitui entregas das demais disciplinas ou a Sprint 3.
