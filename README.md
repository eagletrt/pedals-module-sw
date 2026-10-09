# Pedals module

Firmware della scheda pedali E-Agle su STM32C092, con clock a 48 MHz. Acquisisce i sensori dell'acceleratore e del circuito frenante, calcola la corsa dell'acceleratore e il relativo stato di plausibilità e pubblica misure e diagnostica sulla rete CAN primaria. Espone inoltre log su UART e riconosce la richiesta CAN che provoca il riavvio nel bootloader.

Questa guida presuppone una conoscenza di base di STM32 e HAL: si concentra sul codice applicativo in `Core/Src/pedals/` e `Core/Inc/pedals/` e sul suo collegamento alle periferiche. Descrive il comportamento attualmente implementato, comprese le parti provvisorie. Il modulo applicativo `pedals/bootloader` riconosce le richieste e monitora il traffico di flashing; il progetto separato `pedals-module-bootloader-sw/` esegue la programmazione della Flash.

## Percorso di lettura

1. [main.c](Core/Src/main.c): inizializzazione e collegamento delle callback hardware.
2. [fsm.c](Core/Src/pedals/fsm/fsm.c): ordine delle operazioni nel ciclo principale.
3. [adc.c](Core/Src/adc.c) e [adc_conversion.h](Core/Inc/adc_conversion.h): provenienza, conversione e distribuzione delle misure.
4. Il modulo da modificare, usando la mappa seguente.
5. [Stato attuale e punti da completare](#stato-attuale-e-punti-da-completare), prima di assumere che una funzione dichiarata o descritta nei commenti sia già operativa.

| Modulo | Implementazione | Interfacce e tipi | Responsabilità |
| --- | --- | --- | --- |
| FSM | [fsm.c](Core/Src/pedals/fsm/fsm.c) | [fsm.h](Core/Inc/pedals/fsm/fsm.h) | Coordina avvio, aggiornamenti, comunicazioni e reset. |
| POST | [post-api.c](Core/Src/pedals/post/post-api.c) | [post-api.h](Core/Inc/pedals/post/post-api.h), [post.h](Core/Inc/pedals/post/post.h) | Inizializza timebase, scheduler dei watchdog, bootloader, throttle e CAN attraverso le callback fornite da `main`. |
| Timebase | [timebase.c](Core/Src/pedals/timebase/timebase.c) | [timebase-api.h](Core/Inc/pedals/timebase/timebase-api.h), [timebase.h](Core/Inc/pedals/timebase/timebase.h) | Espone a tutta la scheda il tempo incrementato da TIM3, indipendente da HAL SysTick. |
| Watchdog software | [watchdogs.c](Core/Src/pedals/watchdogs/watchdogs.c) | [watchdogs-api.h](Core/Inc/pedals/watchdogs/watchdogs-api.h) | Gestisce un unico scheduler condiviso e i watchdog registrati dai diversi moduli. |
| Bootloader applicativo | [bootloader.c](Core/Src/pedals/bootloader/bootloader.c) | [bootloader-api.h](Core/Inc/pedals/bootloader/bootloader-api.h), [bootloader.h](Core/Inc/pedals/bootloader/bootloader.h) | Riconosce CONNECT locale e attività di flashing sul bus; possiede soltanto il proprio watchdog di inattività. |
| Throttle | [throttle-api.c](Core/Src/pedals/throttle/throttle-api.c) | [throttle-api.h](Core/Inc/pedals/throttle/throttle-api.h), [throttle.h](Core/Inc/pedals/throttle/throttle.h) | Combina gli APPS, controlla la plausibilità e prepara la telemetria acceleratore. |
| Brake | [brake-api.c](Core/Src/pedals/brake/brake-api.c) | [brake-api.h](Core/Inc/pedals/brake/brake-api.h), [brake.h](Core/Inc/pedals/brake/brake.h) | Conserva corsa e pressioni freno e prepara la telemetria freno/BOTS. |
| BOTS | [bots-api.c](Core/Src/pedals/bots/bots-api.c) | [bots-api.h](Core/Inc/pedals/bots/bots-api.h) | Conserva la tensione del Brake Over-Travel Switch e la confronta con una soglia. |
| CAN | [can-communications-api.c](Core/Src/pedals/can-communication/can-communications-api.c) | [can-communications-api.h](Core/Inc/pedals/can-communication/can-communications-api.h), [can-communications.h](Core/Inc/pedals/can-communication/can-communications.h) | Gestisce le code RX/TX e le callback di trasporto. |
| Router CAN | [can-communications-router-api.c](Core/Src/pedals/can-communication/can-communications-router-api.c) | [can-communications-router-api.h](Core/Inc/pedals/can-communication/can-communications-router-api.h) | Inoltra la ricezione al modulo bootloader; ospita il futuro dispatch dei comandi applicativi. |
| Identity | [identity-api.c](Core/Src/pedals/identity/identity-api.c) | [identity-api.h](Core/Inc/pedals/identity/identity-api.h), [identity.h](Core/Inc/pedals/identity/identity.h) | Pubblica versioni, metadati e stato della FSM. |
| Logger | [logger-api.c](Core/Src/pedals/logger/logger-api.c) | [logger-api.h](Core/Inc/pedals/logger/logger-api.h), [logger.h](Core/Inc/pedals/logger/logger.h) | Formatta messaggi e li inoltra alla UART attraverso PAL. |

Di norma `*-api.h` espone le funzioni pubbliche e il corrispondente `.h` contiene tipi, costanti e struttura dello stato interno. Gli handler sono definiti nei `.c` con `EAGLETRT_STATIC`: nel firmware sono privati al modulo; nei test la macro viene svuotata per permettere l'ispezione dello stato. Il codice applicativo usa C23 (`-std=gnu23`), anche se alcuni costrutti, come `constexpr`, possono ricordare C++.

## Esecuzione e flusso dei dati

Non c'è un RTOS: `main()` esegue continuamente `run_state()`. Le periferiche acquisiscono o segnalano eventi tramite interrupt; il ciclo principale aggiorna la logica e processa le code CAN.

```text
TIM3 TRGO, ogni 1 ms
  -> ADC1: sequenza di 8 canali -> DMA circolare
  -> callback di conversione: copia campioni e calcola tensioni
  -> FSM, ogni almeno 3 ms: adc_update_modules()
       -> throttle: APPS normalizzati
       -> brake: pressioni anteriore/posteriore
       -> bots: tensione
  -> throttle, ogni almeno 5 ms: corsa combinata e plausibilità
  -> messaggi periodici -> serializzazione libcan -> coda TX -> FDCAN1

FDCAN1 RX -> callback HAL -> coda RX -> router -> modulo bootloader
  -> XCP CONNECT per pedals -> flag persistente -> reset MCU
  -> frame nel range flashing -> STATE_FLASH, TX sospesa e RX attiva
       -> altri frame nel range: rinnovo del timeout
       -> 500 ms senza attività: STATE_IDLE, ripresa della telemetria

TIM1 scaduto -> flag di timeout -> errore throttle al successivo aggiornamento
```

### Avvio in `main.c`

La sequenza è rilevante perché i moduli dipendono da callback e code già inizializzate:

1. `VectorBase_Config()` imposta `SCB->VTOR` sulla tabella vettori dell'immagine corrente, anche quando l'applicazione è collocata dopo il bootloader.
2. Inizializza HAL, clock, GPIO, DMA, USART1, FDCAN1, ADC1, TIM3 e TIM1.
3. Crea l'arena e il contesto PAL del logger, quindi chiama `logger_api_init()`.
4. `adc_init()` calibra l'ADC e avvia il DMA; viene avviato anche FDCAN1.
5. Costruisce `PostInit` e chiama una volta `run_state(STATE_INIT, &init_struct)`: il POST inizializza la timebase globale, lo scheduler globale dei watchdog e poi i moduli che vi registrano i propri watchdog.
6. Avvia TIM3 con interrupt: lo stesso update abilita i trigger ADC e incrementa la timebase globale ogni millisecondo.
7. Abilita le notifiche RX delle FIFO CAN, dopo l'inizializzazione delle code.
8. Costruisce `FsmData` e avvia il ciclo infinito.

Le dipendenze hardware passate da `main` sono:

| Callback | Implementazione collegata | Uso |
| --- | --- | --- |
| `PostInit.start_timer` / `stop_timer` | `tim_start_timer_throttle()` / `tim_stop_timer_throttle()` | Finestra di plausibilità dell'acceleratore. |
| Timebase globale | TIM3, tramite `timebase_tick()` | Tick indipendente da HAL SysTick, riutilizzabile da watchdog e futuri scheduler di task. |
| `PostInit.config.send` | `fdcan_send_primary()` | Invio effettivo dei frame. |
| `PostInit.config.on_receive` | `can_communications_router_api_receive_primary()` | Interpretazione dei frame fuori dall'interrupt. |
| `PostInit.config.cs_enter` / `cs_exit` | `__disable_irq()` / `__enable_irq()` | Protezione delle code condivise fra ISR e main loop. |
| `FsmData.get_tick` | `HAL_GetTick()` | Temporizzazioni software in millisecondi. |
| `FsmData.update_module` | `adc_update_modules()` | Trasferimento delle misure ai moduli. |
| `FsmData.system_reset` | `HAL_NVIC_SystemReset()` | Riavvio a seguito della richiesta di bootloader. |

### FSM e POST

`run_state()` chiama la funzione dello stato corrente tramite `state_table`. Se la funzione restituisce `NO_CHANGE`, mantiene lo stato precedente. Il parametro `state_data_t` è un alias di `void`: in `STATE_INIT` deve puntare a `PostInit`, nel normale funzionamento a `FsmData`.

| Stato | Comportamento attuale |
| --- | --- |
| `STATE_INIT` | `post_api_init()` verifica le dipendenze e inizializza timebase, watchdog, bootloader, throttle e CAN; la FSM passa a `STATE_IDLE` oppure `STATE_ERROR`. |
| `STATE_IDLE` | Processa RX e dà priorità al reset locale. Se rileva flashing passa a `STATE_FLASH` senza produrre nuova telemetria; altrimenti aggiorna i sensori e invia i messaggi periodici. |
| `STATE_ERROR` | Rimane nello stato senza eseguire recupero o invii periodici. |
| `STATE_FLASH` | Continua RX, sensori e plausibilità senza produrre telemetria. Un CONNECT locale resetta la MCU; il timeout senza traffico di flashing riporta la FSM in `STATE_IDLE`. |

Il POST è quindi un'inizializzazione dei moduli, non un collaudo dei sensori. Logger, ADC e periferiche sono inizializzati separatamente da `main`.

La timebase e il pool dei watchdog sono risorse della scheda, non del bootloader. L'ordine richiesto è `timebase_init()` → `watchdogs_init()` → inizializzazione dei moduli consumer. `watchdogs_init()` passa alla libreria la funzione `timebase_get_current_tick` come callback: lo scheduler la conserva e legge autonomamente il tempo quando serve. Ogni consumer conserva il proprio `struct Watchdog`, lo inizializza indicando un timeout in millisecondi e usa `start`, `restart`, `pet`, `stop` o `reset`, senza fornire un tick o una callback temporale. Il `main` chiama `watchdogs_update()` una sola volta per iterazione, indipendentemente dallo stato FSM. Le callback scadute vengono eseguite nel main loop, mentre l'interrupt TIM3 si limita a incrementare il tempo; anche le operazioni sui watchdog vanno chiamate dal main loop.

`watchdogs_reset()` riporta un watchdog inizializzato a `NOT_RUNNING`, anche se è scaduto: non lo avvia e non esegue la callback. Se è in esecuzione, lo rimuove dallo scheduler; se è già fermo, la chiamata riesce comunque. `watchdogs_restart()` invece avvia o rinnova il conteggio in qualsiasi stato: resta quindi l'operazione usata dal bootloader a ogni frame di flashing. Il reset di un watchdog software non è il reset della MCU.

Un futuro scheduler globale delle task potrà ricevere la stessa `timebase_get_current_tick` una sola volta in `tasks_api_init()`. Le task non sono ancora integrate negli invii periodici attuali.

In `STATE_IDLE`, le funzioni `send_*()` vengono chiamate a ogni iterazione ma applicano internamente il proprio periodo. `FSM_MODULES_UPDATE_PERIOD_MS` vale 3 ms; `THROTTLE_UPDATE_PEDIOD_MS` vale 5 ms (il nome della macro contiene effettivamente `PEDIOD`). Sono intervalli minimi controllati tramite tick, non task con scadenze garantite: operazioni bloccanti nel ciclo possono ritardarle.

I file della FSM riportano una generazione con `gv_fsm` da `fsm.dot`; tale sorgente non è presente nel repository. Prima di rigenerarli occorre recuperarlo e preservare il codice applicativo inserito nelle funzioni di stato.

## Acquisizione: `adc.c`, `adc.h`, `adc_conversion.h`

Le sigle usate nel codice sono APPS per i sensori di posizione acceleratore, BPPS per il sensore di posizione freno, BSPS per i sensori di pressione del circuito frenante e BOTS per l'interruttore di extracorsa freno.

ADC1 esegue una sequenza a 12 bit con trigger TIM3. DMA1 Channel1 scrive otto `uint16_t` in `adc_buffer` in modalità circolare. `HAL_ADC_ConvCpltCallback()` copia i campioni in `adc_values` e aggiorna `adc_voltages`.

L'ordine di `enum AdcReading` in [adc.h](Core/Inc/adc.h) deve corrispondere ai rank ADC e agli indici dei buffer:

| Indice | Segnale | Pin / canale | Destinazione attuale |
| --- | --- | --- | --- |
| 0 | `SENSE_5V` | PA1 / IN1 | Lettura diagnostica, senza controllo della supply nel ciclo applicativo. |
| 1 | `BSPS_FRONT` | PA2 / IN2 | Pressione anteriore in `brake`. |
| 2 | `BSPS_REAR` | PA3 / IN3 | Pressione posteriore in `brake`. |
| 3 | `BOTS` | PA4 / IN4 | Tensione in `bots`. |
| 4 | `BPPS` | PA5 / IN5 | Tensione acquisita; aggiornamento della corsa freno disabilitato. |
| 5 | `APPS_3` | PA6 / IN6 | Terzo valore passato a `throttle`, con calibrazione ancora incompleta. |
| 6 | `APPS_2` | PA7 / IN7 | Secondo valore passato a `throttle`. |
| 7 | `APPS_1` | PA8 / IN8 | Primo valore passato a `throttle`. |

`adc_read_raw()`, `adc_read_voltage()` e `adc_get_reading_name()` servono per leggere e identificare i canali. Le tensioni sono ricostruite al lato sensore, compensando i partitori: non sono semplicemente la tensione sul pin del microcontrollore.

In [adc_conversion.h](Core/Inc/adc_conversion.h) si trovano:

- Conversione base `raw * 3.3 / 4095` e compensazione del partitore comune `18 / (11.8 + 18)`; per BOTS il rapporto è `47 / (330 + 47)`.
- Correzioni delle pressioni: fattore 1.005 anteriore e 1.006 posteriore.
- Estremi di normalizzazione: APPS1 3.47–5.00 V, APPS2 1.36–3.00 V, BPPS 2.60–3.70 V. La formula è `(V - min) / (max - min)` e non applica autonomamente validazione o saturazione.
- Estremi APPS3 entrambi a 0 V: la normalizzazione attuale divide per zero. Il terzo canale non va considerato calibrato.

`adc_update_modules()` normalizza gli APPS, converte le pressioni da 0.5–4.5 V a 0–100 bar e chiama i setter di throttle, brake e bots. Il mapping delle pressioni usa al momento valori letterali in `adc.c`: cambiare soltanto le macro BSPS del file di conversione non aggiorna questa formula. Non viene applicato qui un controllo di validità delle pressioni.

La conversione BPPS e la chiamata al setter della corsa freno sono commentate. Di conseguenza, nel percorso attuale `BrakeHandler.pedal_travel` resta al valore iniziale zero.

## Acceleratore: `throttle`

Il modulo separa tre operazioni: ricevere le misure con `throttle_api_update_pedal_values()`, elaborarle con `throttle_api_update_internal_status(tick)` e pubblicarle con `throttle_api_send_status(tick)`. I getter espongono corsa combinata, stato e singoli APPS.

Il contratto previsto per gli APPS è una frazione di corsa in `[0, 1]`, oppure `THROTTLE_ERROR_VALUE` (`-1.0F`) per una misura non valida. La corsa non è espressa in percento 0–100.

### Combinazione delle misure

L'algoritmo confronta le tre coppie circolari `(APPS1, APPS2)`, `(APPS2, APPS3)` e `(APPS3, APPS1)`. Una coppia è accettata quando i valori sono utilizzabili e distano al massimo `THROTTLE_MAX_PERCENTAGE_DEVIATION`, pari a 0.1, cioè dieci punti percentuali di corsa.

- Tre coppie valide: media di tutti e tre i sensori.
- Una o due coppie valide: media dell'ultima coppia valida incontrata nel ciclo.
- Nessuna coppia valida: risultato `-1`, che attiva la gestione di implausibilità.

Alla media viene applicata una zona morta: clamp a `[0.05, 0.95]`, poi rimappatura su `[0, 1]`. Il confronto di plausibilità avviene prima di questa rimappatura. L'algoritmo è costruito specificamente per tre sensori; cambiare solo `THROTTLE_ID_COUNT` non basta a estenderlo.

Attualmente un clamp provvisorio a `[0, 1]` viene applicato a ciascun APPS prima del controllo di validità, in attesa della sistemazione dei finecorsa. Questo altera il contratto precedente: anche `-1` diventa `0` e i valori fuori intervallo possono essere trattati come estremi validi. Va tenuto presente durante diagnosi e sviluppo della plausibilità.

### Stati di plausibilità

| Stato | Transizione / uscita |
| --- | --- |
| `THROTTLE_STATUS_OK` | Aggiorna la corsa se le letture sono plausibili. Al primo risultato non plausibile avvia TIM1 e passa a `IMPLAUSIBILITY_RECOVERABLE`. |
| `THROTTLE_STATUS_IMPLAUSIBILITY_RECOVERABLE` | Mantiene l'ultima corsa valida. Se le misure rientrano prima del timeout, ferma il timer e torna a `OK`. |
| `THROTTLE_STATUS_IMPLAUSIBILITY_ERROR` | Timeout di implausibilità; stato persistente fino a reinizializzazione/reset. |
| `THROTTLE_STATUS_CALLBACK_ERROR` | Errore nell'avvio/arresto del timer; anch'esso persistente. |

TIM1 scade dopo 100 ms e la callback HAL pone `is_implausibility_timeout`. Il main loop consuma il flag al successivo aggiornamento di throttle. Lo stato throttle è distinto dallo stato della FSM principale: un errore throttle non porta automaticamente a `STATE_ERROR`.

In errore la corsa memorizzata non viene azzerata: resta l'ultimo valore valido e viene trasmessa insieme al campo `plausibility`. Chi riceve il messaggio deve quindi interpretare anche lo stato. Questo modulo non comanda direttamente attuatori.

`throttle_api_send_status()` serializza APPS1/2/3, corsa combinata e plausibilità nel messaggio `PEDALSTHROTTLE`. La funzione `throttle_api_send_apps()` è dichiarata nell'header ma non ha un'implementazione: gli APPS sono già inclusi nel messaggio di stato.

## Freno e BOTS

### `brake`

`BrakeHandler` conserva corsa freno, pressione anteriore, pressione posteriore e tick dell'ultimo invio. Non ha una FSM né una funzione di inizializzazione esplicita: lo stato statico parte da zero.

`brake_api_update_pedal_travel_percentage()` accetta una piccola tolleranza sui finecorsa: per valori strettamente fra -0.05 e 1.05 applica un clamp a `[0, 1]`; oltre questi limiti memorizza `-1`. I valori esattamente -0.05 e 1.05 non rientrano in nessuno dei due rami e restano invariati. I setter delle pressioni memorizzano direttamente il valore ricevuto, senza validarlo.

`brake_api_send_status()` costruisce `PEDALSBRAKE` con:

- `pressurefl` e `pressurefr`: entrambe dalla misura anteriore.
- `pressurerl` e `pressurerr`: entrambe dalla misura posteriore.
- `travel`: corsa freno memorizzata, attualmente non aggiornata dall'ADC.
- `botsvoltage`: tensione letta tramite `bots_get_voltage()`.

I quattro campi di pressione del messaggio non rappresentano quattro sensori indipendenti: la scheda usa due misure.

### `bots`

`bots_set_voltage()` memorizza la tensione; `bots_get_voltage()` restituisce l'ultima misura. `bots_is_triggered()` restituisce `true` quando la tensione è inferiore a 0.5 V.

Il controllo è a livello: chiamate ripetute continuano a restituire `true` finché la tensione resta bassa. Il commento nell'header che descrive un evento consumabile non corrisponde all'implementazione. Nel ciclo applicativo attuale `bots_is_triggered()` non viene chiamata: viene trasmessa la tensione tramite brake, senza una transizione della FSM o un messaggio di errore dedicato.

## Comunicazione CAN e router

### Code e trasporto

Il modulo gestisce una sola rete, la primaria, con un `CanCommunicationsHandler`, un'arena e un contesto PAL. I riferimenti nei commenti a più reti, steering-wheel o STM32H7 sono residui di codice riutilizzato.

`CanCommunicationFrame` contiene `id`, `length` e al massimo 8 byte di payload. Le code RX e TX hanno capacità di 32 frame ciascuna. PAL gestisce l'accodamento e le copie; `libcan` gestisce la rappresentazione dei messaggi sul bus. La struttura C del frame è usata internamente dalle code, non inviata integralmente su CAN.

Il percorso TX è: modulo produttore → `can_primary_api_serialize_from_id()` → `can_communications_api_add_to_tx_buffer()` → `can_communications_api_process_tx()` → callback `fdcan_send_primary()` → HAL. Un accodamento riuscito non equivale a una trasmissione già completata sul bus.

Il percorso RX è: callback HAL della FIFO → `can_communications_api_add_to_rx_buffer()` → `can_communications_api_process_rx()` → router. Le funzioni `process_*()` elaborano fino a 32 elementi per chiamata e terminano prima se la coda è vuota. Gli errori delle callback vengono riportati continuando, dove previsto, a processare gli altri frame.

La configurazione in [fdcan.c](Core/Src/fdcan.c) usa CAN classico, ID standard in trasmissione, payload fino a 8 byte e 1 Mbit/s: `48 MHz / (3 × (1 + 13 + 2))`. I pin sono PA11 RX e PA12 TX. La ritrasmissione automatica è disabilitata. Non sono configurati filtri standard/estesi dedicati all'applicazione.

In `STATE_FLASH` la FSM non chiama i produttori dei messaggi periodici né `can_communications_api_process_tx()`, quindi non genera nuovo traffico applicativo. La ricezione continua normalmente. Un frame già consegnato alla periferica FDCAN prima della transizione può comunque terminare; il silenzio non riguarda gli ACK generati dal controller CAN.

Le callback FIFO0/FIFO1 trasferiscono un messaggio alla coda software per invocazione. Non eseguono la logica del router in interrupt. Un errore restituito da `process_rx()` porta la FSM in `STATE_ERROR`; i codici di ritorno delle callback HAL di lettura/accodamento e di `process_tx()` non vengono invece gestiti automaticamente.

### Modulo bootloader, router e stato flashing

Il router delega a `bootloader_receive()`. Il modulo riconosce un frame con ID `BOOTLOADER_CAN_RX_ID` (`0x20`), lunghezza esattamente 2 e primo byte `0xFF` (XCP CONNECT). Il secondo byte, modalità di connessione, viene ignorato. La richiesta imposta un flag persistente, letto con `bootloader_is_requested()`: sia `do_idle()` sia `do_flash()` danno priorità a questo flag e chiamano `system_reset()`.

Ogni frame con ID compreso fra `BOOTLOADER_CAN_FLASH_ID_MIN` e `BOOTLOADER_CAN_FLASH_ID_MAX`, estremi inclusi, rinnova il watchdog di inattività, indipendentemente dal comando XCP. Non basta controllare CONNECT: anche dati e risposte mantengono la scheda in `STATE_FLASH`. I frame fuori range non rinnovano il timeout e non provocano reset.

Il range è provvisoriamente `0x19–0x20`, basato sui soli ID OpenBLT presenti nel repository: **prima del collaudo con altre schede va impostato il range ufficiale assegnato a tutti i messaggi di flashing**, senza includere ID di telemetria ordinaria. Le due define e `BOOTLOADER_INACTIVITY_TIMEOUT_MS` (500 ms) sono in [bootloader.h](Core/Inc/pedals/bootloader/bootloader.h).

`bootloader_init()` azzera i flag e registra il proprio watchdog di inattività nello scheduler condiviso. TIM3 genera un interrupt ogni millisecondo e `HAL_TIM_PeriodElapsedCallback()` chiama `timebase_tick()`; il flashing non usa `HAL_GetTick()`. Il main loop chiama `watchdogs_update()` prima della FSM: la routine esegue le callback scadute di tutti i moduli, compresa quella che termina lo stato di flashing. Ogni frame nel range riavvia il watchdog posseduto dal bootloader. Questo watchdog software fa uscire da `STATE_FLASH`; non resetta la MCU.

In `STATE_FLASH` nessuna funzione di invio periodico viene chiamata; sensori e plausibilità throttle continuano ad aggiornarsi. Un errore nell'elaborazione RX o nella routine del watchdog porta a `STATE_ERROR`. Il dispatch di ulteriori comandi applicativi resta da implementare nel TODO del router.

### Messaggi periodici

ID, lunghezze, codifica e periodi sono definiti da `libcan`, tramite `CAN_PRIMARY_MESSAGE_FRAME_ID_*`, `can_primary_byte_size_*` e `can_primary_cycle_time_*`. I produttori mantengono ciascuno il tick dell'ultimo invio e non ritrasmettono a ogni giro della FSM.

| Messaggio | Produttore | Contenuto |
| --- | --- | --- |
| `PEDALSTHROTTLE` | throttle | Tre APPS, corsa combinata, plausibilità. |
| `PEDALSBRAKE` | brake | Pressioni, corsa freno, tensione BOTS. |
| `PEDALSFSM` | identity | Stato passato dalla FSM. |
| `PEDALSVERSION` / `PEDALSVERSIONINFO` | identity | Versione firmware e metadati build. |
| `PEDALSLIBCANVERSION` / `PEDALSLIBCANVERSIONINFO` | identity | Versione e metadati della libreria CAN. |

La dipendenza `libcan-sw` segue `master`: consultare gli header della versione effettivamente scaricata in `.pio/libdeps/<ambiente>/libcan-sw/` per i valori di protocollo, evitando di duplicarli nei moduli o modificare direttamente la cache.

## Identità e log

### `identity`

Le cinque funzioni `identity_api_send_*()` applicano il periodo del rispettivo messaggio, preparano una `union CanPrimaryMessages`, serializzano e accodano il frame. Un ritorno `IDENTITY_RC_OK` può anche significare che il periodo non è ancora trascorso.

La versione firmware è attualmente fissata a `0.1.0` in `identity-api.c`. Build time, commit hash e dirty flag del firmware sono segnaposto a zero. Per `libcan` versione e generation time provengono dalla libreria; hash e dirty flag sono ancora zero.

`do_idle()` passa a `identity_api_send_pedals_fsm()` lo stato `STATE_IDLE`. In `STATE_FLASH` anche questo messaggio è sospeso, per lasciare libero il bus durante la programmazione.

### `logger`

`logger_api_init()` collega il modulo a un `PalHandler` dedicato alla UART; `logger_api_set_state()` abilita o silenzia l'output; `logger_api_log(level, format, ...)` accetta una formattazione tipo `printf`.

I livelli disponibili sono `DEBUG`, `INFO`, `WARN`, `ERROR` ed `EMPTY`. Il buffer di riga è di 128 byte; il messaggio include prefisso, terminazione `\n\r` e byte nullo. Non è previsto un filtro per livello: lo stato abilita/disabilita globalmente i log.

In `main.c`, `LOGGER_ENABLED` è `true`, la coda TX ha capacità 10, quella RX capacità 1 e la dimensione massima del messaggio PAL è 128 byte. L'istanza PAL e l'arena del logger sono separate da quelle CAN. La capacità RX, pur non usata per comandi, è mantenuta positiva per l'allocazione.

Il logger accoda e processa immediatamente la trasmissione. [usart_logger_transmit()](Core/Src/usart.c) usa `HAL_UART_Transmit()` bloccante con timeout di 100 ms, su USART1 a 115200 baud, 8N1, PB6 TX e PB7 RX. Molte chiamate diagnostiche nei moduli sono commentate: abilitare il logger non le riattiva. Inserire log ad alta frequenza può rallentare il ciclo principale; evitare di usarli nelle ISR.

## Integrazioni nei file fuori da `pedals/`

| File | Elementi da conoscere quando si modifica l'applicazione |
| --- | --- |
| [main.c](Core/Src/main.c), [main.h](Core/Inc/main.h) | Composizione dei moduli, configurazione logger, callback, aggiornamento globale dei watchdog, ciclo FSM, rimappatura vettori; nomi e pin dei segnali analogici. `Error_Handler()` disabilita gli interrupt e resta in un ciclo infinito, distinto da `STATE_ERROR`. |
| [adc.c](Core/Src/adc.c), [adc.h](Core/Inc/adc.h), [adc_conversion.h](Core/Inc/adc_conversion.h) | Buffer, ordine canali, callback DMA, calibrazioni e aggiornamento dei moduli. |
| [tim.c](Core/Src/tim.c), [tim.h](Core/Inc/tim.h) | TIM3: prescaler 47 e periodo 999, trigger ADC e tick della timebase globale ogni 1 ms. TIM1: prescaler 4799 e periodo 999, timeout throttle di 100 ms. Start azzera contatore e flag; stop ferma e azzera il contatore; la callback di scadenza segnala il timeout e ferma il timer. |
| [fdcan.c](Core/Src/fdcan.c), [fdcan.h](Core/Inc/fdcan.h) | Adattatore tra frame applicativo e HAL, conversione lunghezza→DLC in TX, ricezione dalle FIFO e accodamento; cancellazione delle richieste TX pendenti all'ingresso in flashing. |
| [usart.c](Core/Src/usart.c), [usart.h](Core/Inc/usart.h) | Callback PAL→UART e configurazione del canale di log. |
| [stm32c0xx_it.c](Core/Src/stm32c0xx_it.c), [stm32c0xx_it.h](Core/Inc/stm32c0xx_it.h) | Instradano gli interrupt DMA1 Channel1, TIM1, USART1 e FDCAN1 ai rispettivi handler HAL; SysTick aggiorna il tick HAL. Le callback applicative si trovano nei file delle periferiche. |
| [dma.c](Core/Src/dma.c), [dma.h](Core/Inc/dma.h) | Clock DMA e abilitazione dell'interrupt DMA1 Channel1. Configurazione e collegamento del canale ADC si trovano in `HAL_ADC_MspInit()` dentro `adc.c`. |
| [gpio.c](Core/Src/gpio.c), [gpio.h](Core/Inc/gpio.h), [stm32c0xx_hal_msp.c](Core/Src/stm32c0xx_hal_msp.c) | Inizializzazione GPIO e supporto HAL di base. Pin analogici e alternate function sono configurati negli MSP delle rispettive periferiche. |
| [system_stm32c0xx.c](Core/Src/system_stm32c0xx.c), [stm32c0xx_hal_conf.h](Core/Inc/stm32c0xx_hal_conf.h) | Supporto clock/vettori e selezione dei driver HAL. L'applicazione reimposta VTOR in `main` prima di `HAL_Init()`. |
| [syscalls.c](Core/Src/syscalls.c), [sysmem.c](Core/Src/sysmem.c) | Supporto runtime C generato; il percorso dei log applicativi passa dal modulo logger e dalla sua callback UART. |

Per modificare la configurazione hardware, mantenere coerente anche [pedals-module-sw.ioc](pedals-module-sw.ioc). Le integrazioni nei file generati sono principalmente nei blocchi `USER CODE`: dopo una rigenerazione CubeMX verificare callback, include, configurazione dei timer e inizializzazioni.

I buffer ADC e il flag di timeout sono condivisi fra interrupt e ciclo principale. Il codice attuale non espone uno snapshot atomico delle otto tensioni: una nuova ISR può interrompere `adc_update_modules()` durante la lettura. Non assumere quindi che tutti i valori elaborati provengano necessariamente dalla stessa scansione quando si estende la logica.

## Compilazione e test

La configurazione applicativa completa è in [platformio.ini](platformio.ini). Occorrono PlatformIO e, per l'upload SWD configurato, `STM32_Programmer_CLI` nel `PATH` e un programmatore collegato. La toolchain ARM è fissata al pacchetto `1.140201.0`; i test nativi richiedono un compilatore host compatibile con `-std=gnu23`.

| Ambiente | Scopo |
| --- | --- |
| `release` (default) | Applicazione autonoma a `0x08000000`, senza spazio riservato al bootloader. |
| `release-bootloader` | Applicazione a `0x08003000`, checksum OpenBLT e immagine S-record. |
| `bootloader` | Compila il progetto OpenBLT separato nei primi 12 KiB. |
| `tests` | Compila i moduli `pedals` sul PC con Unity, senza HAL. |

Comandi dalla radice del repository:

```sh
pio run -e release
pio run -e bootloader
pio run -e release-bootloader
pio test -e tests
pio check -e release
```

Gli artefatti sono in `.pio/build/<ambiente>/`. Il [Makefile](Makefile) generato da CubeMX non include i moduli `pedals` e le dipendenze/configurazioni di PlatformIO: non è equivalente alla build applicativa sopra descritta.

Le dipendenze principali sono `libcan-sw` per il protocollo, `libpal-sw` per code e astrazione delle comunicazioni e `libeagletrt-sw` per utility e macro comuni. [build_script.py](scripts/build_script.py) aggiunge `-Wall -Wextra -Wpedantic -Werror` ai sorgenti sotto `Core/`. L'ambiente `release` configura inoltre l'analisi clang-tidy dei moduli applicativi.

I test esistenti si trovano in [test/](test/):

- `test_throttle`: inizializzazione, combinazione degli APPS, stati e callback timer simulate con FFF.
- `test_brake`: gestione dei valori di corsa freno.
- `test_bots`: memorizzazione tensione e soglia di attivazione.
- `test_can_router`: riconoscimento XCP CONNECT e rifiuto delle combinazioni non corrispondenti.
- `test_timebase_watchdogs`: timebase comune, lettura del tick aggiornato tramite callback, più watchdog nello stesso scheduler, scadenza, restart, pet, stop, reset senza riavvio e argomenti non validi.
- `test_bootloader`: richiesta locale, limiti inclusivi del range, rinnovo e scadenza tramite i servizi temporali condivisi.
- `test_fsm_flashing`: percorso integrato code→router→FSM, assenza di nuova telemetria, ripresa dopo inattività e reset locale da `IDLE`/`FLASH`.

È possibile selezionare una suite, per esempio `pio test -e tests -f test_timebase_watchdogs -f test_bootloader -f test_can_router -f test_fsm_flashing`. I test accedono ad alcuni handler grazie a `-DEAGLETRT_STATIC=`. I test nativi non verificano calibrazioni, timing hardware o collegamenti CAN.

Verifica con `libtimebase-sw` sul branch `dev`, commit `9643d9f`: i 28 test delle quattro suite interessate (`test_timebase_watchdogs`, `test_bootloader`, `test_can_router` e `test_fsm_flashing`) passano; compilano anche le immagini `release` e `release-bootloader`. La suite completa passa 52 test su 54: restano i due fallimenti già presenti in throttle (`one_value_valid` e `no_value_valid`), relativi al clamp provvisorio degli APPS fuori range e non a questa modifica. Firmware e test usano entrambi `#dev` in `platformio.ini`; il riferimento al branch può avanzare con i successivi aggiornamenti delle dipendenze.

Collaudo su scheda ancora da eseguire, dopo aver configurato il range di rete:

1. Verificare la telemetria normale e inviare un frame nel range diverso dal CONNECT locale: pedals deve smettere di trasmettere messaggi applicativi.
2. Continuare con frame nel range a intervalli inferiori a 500 ms: la scheda deve restare silenziosa, con ADC e plausibilità ancora attivi.
3. Interrompere il traffico di flashing mantenendo eventualmente traffico fuori range: la telemetria deve riprendere dopo 500 ms.
4. Ripetere con gli estremi del range e con ID immediatamente esterni.
5. Inviare CONNECT per pedals sia dal funzionamento normale sia mentre è silenziosa, quindi verificare l'aggiornamento completo con il tool OpenBLT.

## Bootloader: organizzazione essenziale

Il bootloader effettivo si trova in [pedals-module-bootloader-sw/](pedals-module-bootloader-sw/). È un'immagine distinta basata sul port OpenBLT per STM32C0, con proprie copie di startup, HAL, `Core/Src` e `Core/Inc`. Si compila con l'ambiente `bootloader` del `platformio.ini` nella radice.

| File nel progetto bootloader | Ruolo |
| --- | --- |
| [main.c](pedals-module-bootloader-sw/Core/Src/main.c) | Inizializza clock, GPIO e UART; chiama `BootInit()` e poi continuamente `BootTask()`. Contiene anche diagnostica UART dei registri CAN. |
| [boot.c](pedals-module-bootloader-sw/Core/Src/boot.c), [boot.h](pedals-module-bootloader-sw/Core/Inc/boot.h) | Coordinano inizializzazione e ciclo OpenBLT. Sono distinti dal modulo applicativo `pedals/bootloader`. |
| [blt_conf.h](pedals-module-bootloader-sw/Core/Inc/blt_conf.h) | Configurazione del port: CAN classico a 1 Mbit/s, RX `0x20`, TX `0x19`, finestra iniziale 500 ms, Flash 256 KiB. Trasporto UART disabilitato. |
| `com.c`, `xcp.c`, `can.c` | Sessione di comunicazione, protocollo XCP e trasporto CAN. `CanInit()` inizializza FDCAN riusando lo MSP di `fdcan.c`; `main` non chiama `MX_FDCAN1_Init()`. |
| `backdoor.c` | Gestisce la finestra di ingresso: senza connessione prova ad avviare l'applicazione dopo 500 ms; una connessione mantiene attivo il bootloader. |
| `nvm.c`, `flash.c`, [flash_layout.c](pedals-module-bootloader-sw/Core/Src/flash_layout.c) | Cancellazione, scrittura e verifica della firma dell'applicazione; layout scrivibile che esclude i primi 12 KiB. `flash_layout.c` è incluso da `flash.c` e non va compilato separatamente. |
| `cpu.c`, `cpu_comp.c`, `timer.c` | Gestione CPU, primitive del compilatore e salto al reset handler applicativo. Il bootloader lavora in polling con interrupt globali disabilitati; il tick usa TIM1 e ridefinisce `HAL_GetTick()`. |
| `cop.c`, `events.c`, `file.c`, `infotable.c`, `asserts.c` | Supporto OpenBLT per watchdog, eventi, aggiornamento da file, metadati e assert, secondo le opzioni abilitate. La presenza di un sorgente non implica che quella funzione sia attiva. |

### Memoria e immagini

| Immagine | Intervallo Flash | Linker script |
| --- | --- | --- |
| Bootloader | `0x08000000–0x08002FFF`, 12 KiB | [Linker bootloader](pedals-module-bootloader-sw/STM32C092XX_FLASH.ld) |
| Applicazione con bootloader | `0x08003000–0x0803FFFF`, 244 KiB | [STM32C092XX_FLASH_SHIFTED.ld](STM32C092XX_FLASH_SHIFTED.ld) |
| Applicazione autonoma | Da `0x08000000`, fino a 256 KiB | [STM32C092XX_FLASH.ld](STM32C092XX_FLASH.ld) |

All'avvio OpenBLT verifica la firma dell'applicazione; se valida, rilascia le risorse usate, riposiziona VTOR e chiama il reset handler applicativo. Se non è valida rimane disponibile per la programmazione.

La firma è il complemento a due della somma delle prime sette word della tabella vettori, memorizzato all'offset `0xC0` dell'applicazione. Non è un CRC dell'intera immagine. [startup_stm32c092xx.s](startup_stm32c092xx.s) riserva la word `0x55AA11EE`; [openblt_image.py](scripts/openblt_image.py), nell'ambiente `release-bootloader`, la sostituisce nel `firmware.bin` e genera `firmware.srec` con base `0x08003000`. Il file ELF non viene modificato da questo passaggio. Durante la programmazione CAN, OpenBLT scrive la firma attraverso il proprio percorso di finalizzazione.

### Sequenza di aggiornamento

Per installare le due immagini via SWD, i target configurati sono:

```sh
pio run -e bootloader -t upload
pio run -e release-bootloader -t upload
```

Il secondo comando usa il BIN con checksum all'indirizzo `0x08003000`. L'upload dell'ambiente `release`, invece, scrive l'applicazione a `0x08000000` e sovrascrive l'area del bootloader: scegliere l'ambiente in base alla configurazione desiderata.

Per l'aggiornamento via CAN, l'immagine destinata a un host OpenBLT compatibile, come BootCommander/MicroBoot, è `.pio/build/release-bootloader/firmware.srec`. Il target ascolta sull'ID `0x20` e risponde su `0x19` a 1 Mbit/s. Gli upload PlatformIO sopra sono via SWD, non via CAN.

Con l'applicazione già in esecuzione, l'host invia XCP CONNECT; il modulo applicativo bootloader lo riconosce e la FSM resetta la MCU, anche se si trova già in `STATE_FLASH`. OpenBLT parte e riceve un successivo tentativo di CONNECT nella finestra di 500 ms. La richiesta iniziale non viene conservata attraverso il reset: l'host deve riprovare la connessione. `STATE_FLASH` gestisce il silenzio dell'applicazione durante il traffico di flashing, mentre la riscrittura della Flash avviene nel progetto OpenBLT separato.

Se si cambia l'indirizzo applicativo, mantenere allineati linker script, `flash_layout.c`, `platformio.ini` e `openblt_image.py`; per gli ID CAN allineare `blt_conf.h` e `pedals/bootloader/bootloader.h`.

## Stato attuale e punti da completare

Questi comportamenti sono rilevabili nei sorgenti e aiutano a evitare assunzioni errate durante lo sviluppo:

| Punto | Conseguenza pratica / dove intervenire |
| --- | --- |
| APPS3 con estremi entrambi a zero | La normalizzazione divide per zero pur essendo chiamata. Completare calibrazione e gestione del canale in `adc_conversion.h` / `adc.c`. |
| Clamp provvisorio degli APPS | Il valore sentinella `-1` e altri fuori range possono diventare valori validi. Rivedere il blocco temporaneo in `throttle_api_update_internal_status()` insieme ai finecorsa. |
| BPPS non inoltrato al modulo brake | La corsa freno trasmessa resta zero; non indica una misura verificata di pedale rilasciato. |
| Pressioni senza validazione | Il mapping e i setter non segnalano esplicitamente un sensore fuori range. Definire il trattamento prima di usare le misure per nuove decisioni. |
| BOTS solo acquisito e trasmesso | La soglia non provoca azioni nella FSM; l'API non implementa l'evento consumabile descritto nel commento. |
| `STATE_ERROR` vuoto | Non esistono recupero o diagnostica periodica in errore. |
| Range flashing provvisorio | Allineare gli estremi in `bootloader.h` all'assegnazione di rete prima di verificare il silenzio durante la programmazione delle altre schede. |
| Metadati identity provvisori | Versione fissa e campi a zero non identificano univocamente la build. |
| Router applicativo ancora TODO | I messaggi diversi dalla richiesta OpenBLT non producono azioni. |
| Diversi return code ignorati | L'inizializzazione di alcune periferiche, il logger e il percorso CAN non hanno una gestione applicativa completa degli errori. |

Per una modifica ai sensori partire da `adc_conversion.h` e `adc_update_modules()`; per una regola sull'acceleratore da throttle e dai suoi test; per un comando ricevuto dal router; per scheduling e reazioni globali dalla FSM. Un nuovo modulo deve avere header raggiungibili nei `build_flags` degli ambienti interessati e, se necessario, callback collegate in `main`/POST e un aggiornamento nel ciclo operativo.
