
#include <Ethernet.h>
#include <SPI.h>

// Configuracion del Ethernet Shield
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFF, 0xEE}; // Direccion MAC
byte ip[] = { 192, 168, 10, 100 }; // Direccion IP del Arduino
byte server[] = { 192, 168, 10, 1 }; // Direccion IP del servidor

EthernetClient client;


volatile int NbTopsFan; //La medición de los flancos de subida de la señal
int Calc;               //Variable donde guardaremos los calculos de arduino
int hallsensor = 2;     //El pin donde se encuentra el sensor


void rpm ()
{
  NbTopsFan++;  //This function measures the rising and falling edge of the hall effect sensors signal
}


void setup()
{
  pinMode(hallsensor, INPUT);     //initializes digital pin 2 as an input
  Serial.begin(9600);             //This is the setup function where the serial port is initialised,
  attachInterrupt(0, rpm, RISING);//and the interrupt is attached

  Ethernet.begin(mac, ip);        // Inicializamos el Ethernet Shield
  delay(1000);                    // Esperamos 1 segundo de cortesia
}



void loop ()
{
  NbTopsFan = 0;   //Set NbTops to 0 ready for calculations
  sei();      //Enables interrupts
  delay (1000);   //Wait 1 second
  cli();      //Disable interrupts
  Calc = (NbTopsFan * 60 / 7.5); //(Pulse frequency x 60) / 7.5Q, = flow rate in L/hour
  Serial.print (Calc, DEC); //Prints the number calculated above
  Serial.print (" L/hour\r\n"); //Prints "L/hour" and returns a  new line
  Serial.println("Connecting...");

  if (client.connect(server, 80) > 0) { // Conexion con el servidor
    client.print("GET /iot.php?origen=arduino&valor="); // Enviamos los datos por GET
    client.print(Calc, DEC);
    client.println(" HTTP/1.0");
    client.println("User-Agent: Arduino 1.0");
    client.println();
    Serial.println("Conectado");
  } else {
    Serial.println("Fallo en la conexion");
  }
  if (!client.connected()) {
    Serial.println("Disconnected!");
  }
  client.stop();
  client.flush();
}

