#include <SoftwareSerial.h>
#include <String.h>

SoftwareSerial gprsSerial(7, 8);

//Set the pin to receive the signal.
const int pwPin = 3;
//variables needed to store values
int arraysize = 9; //quantity of values to find the median (sample size). Needs to be an odd number
//declare an array to store the samples. not necessary to zero the array values here, it just makes the code clearer
int rangevalue[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0};
long pulse;
int modE;
String inicio = "AT+HTTPPARA=\"URL\",\"http://www.satah-uno.org/rn.php&n=";
String fin =  "\"";

void setup()
{
  gprsSerial.begin(19200);
  Serial.begin(9600);
  delay(30000);

  Serial.println("Config SIM900...");
  delay(2000);
  Serial.println("Done!...");
  gprsSerial.flush();
  Serial.flush();

  // conectar o desconectar dle servicio GPRS 
  gprsSerial.println("AT+CGATT?");
  delay(100);
  toSerial();

  // Ajustes de operador
  gprsSerial.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
  delay(2000);
  toSerial();

  // Se añade el APN movistar del chip
  gprsSerial.println("AT+SAPBR=3,1,\"APN\",\"movistar.pe\"");
  delay(2000);
  toSerial();

  // Ajustes del operador
  gprsSerial.println("AT+SAPBR=1,1");
  delay(2000);
  toSerial();
}

void loop()
{

  //obteniendo el valor del sonar en cm
  pinMode(pwPin, INPUT);
    for(int i = 0; i < arraysize; i++)
    {                   
      pulse = pulseIn(pwPin, HIGH);
      rangevalue[i] = pulse/58;
      delay(10);
    }
  Serial.print("Sin clasificar: ");
  printArray(rangevalue,arraysize);
  isort(rangevalue,arraysize);
  Serial.print("Clasificado: ");
  printArray(rangevalue,arraysize);
  modE = mode(rangevalue,arraysize);
  Serial.print("La moda/mediana es: ");
  Serial.print(modE);

  Serial.println();
  delay(1000);

   // iniciando servicio HTTP
   gprsSerial.println("AT+HTTPINIT");
   delay(2000); 
   toSerial();

   // ENVIAR VALOR POR HTTP
   // se enviara a la columna dist
   gprsSerial.println(inicio+modE+fin);
   delay(2000);
   toSerial();

   // tipo de set http  0 = GET, 1 = POST, 2 = HEAD
   gprsSerial.println("AT+HTTPACTION=0");
   delay(6000);
   toSerial();

   // leer respuesta del server
   gprsSerial.println("AT+HTTPREAD"); 
   delay(1000);
   toSerial();

   gprsSerial.println("");
   gprsSerial.println("AT+HTTPTERM");
   toSerial();
   delay(300);


   //Vuelve a repetir cada 30 segundos
   gprsSerial.println("");
   delay(30000);
}

void toSerial()
{
  while(gprsSerial.available()!=0)
  {
    Serial.write(gprsSerial.read());
  }
}

/*-----------Functions------------*/ //Imprimir el arreglo.
void printArray(int *a, int n) {
  for (int i = 0; i < n; i++)
  {
    Serial.print(a[i], DEC);
    Serial.print(' ');
  }
  Serial.println();
}
//Función de clasificación
// (Author: Bill Gentles, Nov. 12, 2010)
void isort(int *a, int n){
// *a es una función de puntero de lista
  for (int i = 1; i < n; ++i)
  {
    int j = a[i];
    int k;
    for (k = i - 1; (k >= 0) && (j < a[k]); k--)
    {
      a[k + 1] = a[k];
    }
    a[k + 1] = j;
  }
}
//Mode función
int mode(int *x,int n){
  int i = 0;
  int count = 0;
  int maxCount = 0;
  int mode = 0;
  int bimodal;
  int prevCount = 0;
  while(i<(n-1)){
    prevCount=count;
    count=0;
    while(x[i]==x[i+1]){
      count++;
      i++;
    }
    if(count>prevCount&count>maxCount){
      mode=x[i];
      maxCount=count;
      bimodal=0;
    }
    if(count==0){
      i++;
    }
    if(count==maxCount){//si el conjunto de datos tiene 2 o mas modas.
      bimodal=1;
    }
    if(mode==0||bimodal==1){//Retorna la mediana si no hay moda.
      mode=x[(n/2)];
    }
    return mode;
  }
}


