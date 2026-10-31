# Configuração Blynk da Pelé Academia

Template Pele Academia, ID TMPL2RXHEPTJJ. Dispositivo Pele Academia ESP32, ID 1091498.

| Pin | Dado | Tipo e faixa |
|---|---|---|
| V0 | Temperatura | Double, -40 a 80 °C |
| V1 | Alarme | Integer, 0 a 1 |
| V2 | Umidade | Double, 0 a 100 |
| V3 | Estado | String |
| V4 | Sensor valido | Integer, 0 a 1 |

Painel: dois medidores, LED de alarme, estado textual e validade da leitura. A prévia do Template mostra exemplos; os dados reais ficam no painel do dispositivo.

## Autenticação sem token no código

Copie o Auth Token na sua conta Blynk e envie no Monitor Serial privado: AUTH seguido de um espaço e do token. Pressione Enter. O token fica em RAM até a simulação reiniciar e não é impresso pelo firmware. Não o publique em arquivos, capturas ou commits. O placeholder BLYNK_AUTH_TOKEN permanece no sketch público e não é usado para autenticar; a autenticação usa a credencial recebida em memória.

Envie 0 e Enter para suspender Wi-Fi/Blynk; envie 1 e Enter para permitir reconexão. A leitura, o OLED e a decisão local continuam em ambos os estados.

## Links e acesso do professor

- [Painel do dispositivo](https://blynk.cloud/dashboard/359336/global/devices/126785/organization/359336/devices/1091498/dashboard)
- [Template](https://blynk.cloud/dashboard/359336/templates/787381/dashboard)

Esses links exigem login e autorização na organização/dispositivo. As capturas neste repositório são públicas, mas não substituem a permissão para abrir o painel na plataforma. Foi tentado o convite de um segundo usuário, mas o Blynk bloqueou a ação porque o plano Free desta conta inclui somente um usuário. Nenhum convite foi enviado. Para permitir a visualização da execução sem login, o README apresenta as capturas reais de NORMAL, ALERTA, operação offline e reconexão. A aceitação dessa alternativa ao acesso direto depende do professor. O fluxo documentado do Blynk utiliza usuários e papéis autorizados, não um link anônimo do dispositivo. Não compartilhe o Auth Token para contornar essa limitação.
